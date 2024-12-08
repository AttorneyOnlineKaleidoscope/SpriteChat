#include "playerlistwidget.h"

#include "aoapplication.h"
#include "moderation_functions.h"
#include "network/packet/moderation_packets.h"
#include "widget/moderatordialog.h"

#include <QListWidgetItem>
#include <QMenu>

PlayerListWidget::PlayerListWidget(Options &options, kal::PacketTransport &transport, kal::AssetPathResolver &assetPathResolver, const kal::NetworkSession &session, QWidget *parent)
    : QListWidget(parent)
    , options(options)
    , m_transport(transport)
    , m_resolver(assetPathResolver)
    , m_session(session)
{
  setContextMenuPolicy(Qt::CustomContextMenu);

  connect(this, &PlayerListWidget::customContextMenuRequested, this, &PlayerListWidget::onCustomContextMenuRequested);
}

PlayerListWidget::~PlayerListWidget()
{}

void PlayerListWidget::registerPlayer(const kal::RegisterPlayerPacket &update)
{
  switch (update.mode)
  {
  default:
    Q_UNREACHABLE();
    break;

  case kal::RegisterPlayerPacket::Add:
    addPlayer(update.player_id);
    break;

  case kal::RegisterPlayerPacket::Remove:
    removePlayer(update.player_id);
    break;
  }
}

void PlayerListWidget::updatePlayer(const kal::UpdatePlayerPacket &update)
{
  kal::PlayerData &player = m_player_map[update.player_id];

  bool update_icon = false;
  switch (update.update)
  {
  default:
    Q_UNREACHABLE();
    break;

  case kal::UpdatePlayerPacket::Name:
    player.name = update.value.toString();
    break;

  case kal::UpdatePlayerPacket::Character:
    if (update.value.isNull())
    {
      player.character.reset();
    }
    else
    {
      player.character = update.value.toString();
    }
    update_icon = true;
    break;

  case kal::UpdatePlayerPacket::CharacterName:
    if (update.value.isNull())
    {
      player.character_name.reset();
    }
    else
    {
      player.character_name = update.value.toString();
    }
    break;

  case kal::UpdatePlayerPacket::AreaId:
    player.area_id = update.value.toInt();
    break;
  }
  updatePlayer(player.id, update_icon);

  filterPlayerList();
}

void PlayerListWidget::reloadPlayers()
{
  for (const kal::PlayerData &player : qAsConst(m_player_map))
  {
    updatePlayer(player.id, false);
  }
}

void PlayerListWidget::setAuthenticated(bool f_state)
{
  m_is_authenticated = f_state;
  for (const kal::PlayerData &data : qAsConst(m_player_map))
  {
    updatePlayer(data.id, false);
    filterPlayerList();
  }
}

void PlayerListWidget::onCustomContextMenuRequested(const QPoint &pos)
{
  QListWidgetItem *item = itemAt(pos);
  if (item == nullptr)
  {
    return;
  }
  int id = item->data(Qt::UserRole).toInt();
  QString name = item->text();

  QMenu *menu = new QMenu(this);
  menu->setAttribute(Qt::WA_DeleteOnClose);

  QAction *report_player_action = menu->addAction("Report Player");
  connect(report_player_action, &QAction::triggered, this, [this, id, name] {
    auto maybe_reason = call_moderator_support(name);
    if (maybe_reason.has_value())
    {
      kal::HelpMePacket packet;
      packet.message = maybe_reason.value();
      packet.player_id = id;
      m_transport.shipPacket(packet);
    }
  });

  if (m_is_authenticated)
  {
    QAction *kick_player_action = menu->addAction("Kick");
    connect(kick_player_action, &QAction::triggered, this, [this, id, name] {
      ModeratorDialog *dialog = new ModeratorDialog(id, false, m_transport);
      dialog->setWindowTitle(tr("Kick %1").arg(name));
      connect(this, &PlayerListWidget::destroyed, dialog, &ModeratorDialog::deleteLater);
      dialog->show();
    });

    QAction *ban_player_action = menu->addAction("Ban");
    connect(ban_player_action, &QAction::triggered, this, [this, id, name] {
      ModeratorDialog *dialog = new ModeratorDialog(id, true, m_transport);
      dialog->setWindowTitle(tr("Ban %1").arg(name));
      connect(this, &PlayerListWidget::destroyed, dialog, &ModeratorDialog::deleteLater);
      dialog->show();
    });
  }

  menu->popup(mapToGlobal(pos));
}

void PlayerListWidget::addPlayer(int playerId)
{
  m_player_map.insert(playerId, kal::PlayerData{.id = playerId});
  QListWidgetItem *item = new QListWidgetItem(this);
  item->setData(Qt::UserRole, playerId);
  m_item_map.insert(playerId, item);
  updatePlayer(playerId, false);
}

void PlayerListWidget::removePlayer(int playerId)
{
  delete takeItem(row(m_item_map.take(playerId)));
  m_player_map.remove(playerId);
}

void PlayerListWidget::filterPlayerList()
{
  kal::AreaId area_id = m_player_map.value(m_session.playerId()).area_id;
  for (auto it = m_player_map.begin(); it != m_player_map.end(); ++it)
  {
    QListWidgetItem *item = m_item_map.value(it.key());
    item->setHidden(!m_is_authenticated && it->area_id != area_id);
  }
}

void PlayerListWidget::updatePlayer(int playerId, bool updateIcon)
{
  kal::PlayerData &data = m_player_map[playerId];
  QListWidgetItem *item = m_item_map[playerId];

  if (!item)
  {
    kWarning() << "Invalid player id" << playerId;
    return;
  }

  item->setText(data.name);
  if (data.character)
  {
    QString tooltip = data.character.value();
    if (data.character_name)
    {
      tooltip = QObject::tr("%1 aka %2").arg(tooltip, data.character_name.value());
    }
    item->setToolTip(tooltip);

    if (updateIcon)
    {
      item->setIcon(QIcon(m_resolver.characterFilePath(data.character.value(), "char_icon", kal::ImageAssetType).value_or(QString())));
    }
  }
  else
  {
    item->setToolTip(QString());
    item->setIcon(QIcon());
  }
}

QString PlayerListWidget::formatLabel(const kal::PlayerData &data)
{
  QString format = options.playerlistFormatString();
  format.replace("{id}", QString::number(data.id));
  format.replace("{name}", data.name);
  format.replace("{character}", data.character.value_or(tr("Spectator")));
  format.replace("{character_name}", data.character_name.value_or(tr("Unnamed")));
  return format;
}
