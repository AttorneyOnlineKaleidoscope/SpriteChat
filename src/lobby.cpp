#include "lobby.h"

#include "aoapplication.h"
#include "debug_functions.h"
#include "gui/guiloader.h"
#include "networkmanager.h"
#include "spritechatversion.h"
#include "widget/directconnectdialog.h"
#include "widget/messagedialog.h"
#include "widget/servereditordialog.h"

#include <QDialog>
#include <QHeaderView>
#include <QImageReader>
#include <QLayout>
#include <QMessageBox>
#include <QRegularExpression>
#include <QRegularExpressionMatch>
#include <QUiLoader>
#include <QUrl>
#include <QVersionNumber>

Lobby::Lobby(Options &options, AOApplication &application, kal::AssetPathResolver &assetPathResolver, kal::NetworkManager &network, QWidget *parent)
    : QMainWindow(parent)
    , options(options)
    , ao_app(application)
    , m_resolver(assetPathResolver)
    , m_network(network)
{
  setupInterface();

  connect(&m_master, &kal::MasterApiGateway::errorOccurred, this, &Lobby::displayMasterError);
  connect(&m_master, &kal::MasterApiGateway::messageOfTheDayChanged, this, &Lobby::updateMessageOfTheDay);
  connect(&m_master, &kal::MasterApiGateway::serverListChanged, this, &Lobby::updatePublicList);
  connect(&m_master, &kal::MasterApiGateway::versionChanged, this, &Lobby::checkMasterVersion);

  refreshServerList();
}

Lobby::~Lobby()
{}

Lobby::ServerType Lobby::currentServerType() const
{
  return ServerType(ui_tabs->currentIndex());
}

std::optional<kal::ServerInfo> Lobby::currentServer() const
{
  QTreeWidgetItem *item = nullptr;
  switch (currentServerType())
  {
  case Public:
    item = ui_public_tree->currentItem();
    if (item && item->isSelected())
    {
      return m_public_list.at(item->data(0, Qt::UserRole).toInt());
    }
    break;

  case Favorite:
    item = ui_favorite_tree->currentItem();
    if (item && item->isSelected())
    {
      return m_favorite_list.at(item->data(0, Qt::UserRole).toInt());
    }
    break;
  }

  return std::nullopt;
}

void Lobby::closeEvent(QCloseEvent *event)
{
  options.setWindowPosition(objectName(), pos());
  QMainWindow::closeEvent(event);
}

void Lobby::updateFavoriteList()
{
  m_favorite_list = options.favorites();
  updateServerList(ui_favorite_tree, m_favorite_list);
}

void Lobby::resetCurrentServerSelection()
{
  ui_public_tree->clearSelection();
  ui_favorite_tree->clearSelection();
}

void Lobby::setupInterface()
{
  setObjectName("lobby");

  kal::GuiLoader loader(*this);
  loader.load(":/resource/ui/lobby.ui");

  loader.assign(ui_version, "version");
  ui_version->setText(tr("Version: %1").arg(kal::SpriteChat::softwareVersionString()));

  loader.assign(ui_options, "options");
  connect(ui_options, &QPushButton::clicked, this, &Lobby::openOptions);

  loader.assign(ui_about, "about");
  connect(ui_about, &QPushButton::clicked, this, &Lobby::openAbout);

  loader.assign(ui_tabs, "tabs");
  ui_tabs->tabBar()->setExpanding(true);
  connect(ui_tabs, &QTabWidget::currentChanged, this, &Lobby::resetCurrentServerSelection);

  loader.assign(ui_public_tree, "public_tree");
  loader.assign(ui_public_search, "public_search");
  ui_public_tree->setColumnHidden(1, true);
  connect(ui_public_tree, &QTreeWidget::itemSelectionChanged, this, &Lobby::setupCurrentServer);
  connect(ui_public_tree, &QTreeWidget::itemDoubleClicked, this, &Lobby::setupCurrentServer);
  connect(ui_public_tree->header(), &QHeaderView::sectionClicked, this, &Lobby::nextServerSortMode);
  connect(ui_public_search, &QLineEdit::textChanged, this, &Lobby::filterPublicList);

  loader.assign(ui_favorite_tree, "favorite_tree");
  ui_favorite_tree->setColumnHidden(1, true);
  connect(ui_favorite_tree, &QTreeWidget::itemSelectionChanged, this, &Lobby::setupCurrentServer);
  connect(ui_favorite_tree, &QTreeWidget::itemDoubleClicked, this, &Lobby::setupCurrentServer);
  connect(ui_favorite_tree->header(), &QHeaderView::sectionClicked, this, &Lobby::nextServerSortMode);

  loader.assign(ui_refresh, "refresh");
  connect(ui_refresh, &QPushButton::released, this, &Lobby::refreshServerList);

  loader.assign(ui_direct_connect, "direct_connect");
  connect(ui_direct_connect, &QPushButton::released, this, &Lobby::openDirectConnect);

  loader.assign(ui_add_to_favorite, "add_to_favorite");
  connect(ui_add_to_favorite, &QPushButton::released, this, &Lobby::addToFavorite);

  loader.assign(ui_add_favorite, "add_favorite");
  connect(ui_add_favorite, &QPushButton::released, this, &Lobby::addFavorite);

  loader.assign(ui_edit_favorite, "edit_favorite");
  connect(ui_edit_favorite, &QPushButton::released, this, &Lobby::editFavorite);

  loader.assign(ui_remove_favorite, "remove_favorite");
  connect(ui_remove_favorite, &QPushButton::released, this, &Lobby::removeFavorite);

  loader.assign(ui_description, "description");
  loader.assign(ui_connect, "connect");
  connect(ui_connect, &QPushButton::released, this, &Lobby::connectToCurrentServer);

  loader.assign(ui_motd, "motd");
}

void Lobby::updateServerList(QTreeWidget *tree, const QList<kal::ServerInfo> &serverList)
{
  tree->clear();
  for (int i = 0; i < serverList.length(); ++i)
  {
    const kal::ServerInfo &i_server = serverList.at(i);
    QHostAddress address(QStringLiteral("%1:%2").arg(i_server.address).arg(i_server.port));
    if (!address.isNull())
    {
      kWarning() << "Invalid server address:" << i_server.toString();
      continue;
    }

    QTreeWidgetItem *item = new QTreeWidgetItem(tree);
    item->setText(0, i_server.nameOrDefault());
    item->setData(0, Qt::UserRole, i);
    item->setText(1, QString::number(i));
    if (i_server.legacy)
    {
      item->setText(0, tr("%1 (Incompatible)").arg(item->text(0)));
      item->setBackground(0, Qt::darkRed);
    }
  }
  sortServerList(tree);
}

void Lobby::refreshServerList()
{
  resetCurrentServerSelection();
  m_master.setOrigin(options.alternativeMasterserver());
  m_master.requestMessageOfTheDay();
  m_master.requestServerList();
  m_master.requestVersion();
  updateFavoriteList();
}

void Lobby::openDirectConnect()
{
  DirectConnectDialog dialog(m_network);
  dialog.exec();
}

void Lobby::addToFavorite()
{
  if (currentServerType() != Public)
  {
    return;
  }

  const auto maybe_server = currentServer();
  if (maybe_server)
  {
    options.addFavorite(maybe_server.value());
    updateFavoriteList();
  }
}

void Lobby::addFavorite()
{
  ServerEditorDialog dialog;
  if (dialog.exec())
  {
    options.addFavorite(dialog.currentServerInfo());
    updateFavoriteList();
  }
}

void Lobby::editFavorite()
{
  if (currentServerType() != Favorite)
  {
    return;
  }

  const auto maybe_server = currentServer();
  if (maybe_server)
  {
    ServerEditorDialog dialog(maybe_server.value());
    if (dialog.exec())
    {
      options.updateFavorite(dialog.currentServerInfo(), ui_favorite_tree->currentItem()->data(0, Qt::UserRole).toInt());
      updateFavoriteList();
    }
  }
}

void Lobby::removeFavorite()
{
  if (currentServerType() != Favorite)
  {
    return;
  }

  QTreeWidgetItem *item = ui_favorite_tree->currentItem();
  if (item)
  {
    const int index = item->data(0, Qt::UserRole).toInt();
    options.removeFavorite(index);
    updateFavoriteList();
  }
}

void Lobby::displayCurrentServerDescription()
{
  ui_description->clear();

  const auto maybe_server = currentServer();
  if (maybe_server)
  {
    if (maybe_server->description.isEmpty())
    {
      ui_description->setText(tr("No description available."));
    }
    else
    {
      QString description = maybe_server->description;
      description = description.replace('\n', "<br />");

      static QRegularExpression URL_PATTERN(R"(\b(https?://\S+))");
      QRegularExpressionMatchIterator matches = URL_PATTERN.globalMatch(description);
      while (matches.hasNext())
      {
        const QString address = matches.next().captured(1);
        if (QUrl(address).isValid())
        {
          description.replace(address, QStringLiteral(R"(<a href="%1">%1</a>)").arg(address));
        }
      }
      ui_description->setHtml(description);
    }
  }
  else
  {
    ui_description->setText(QString());
  }
}

void Lobby::connectToCurrentServer()
{
  const auto maybe_server = currentServer();
  if (!maybe_server)
  {
    return;
  }

  m_network.setAndConnectToServer(maybe_server.value());
}

void Lobby::openAbout()
{
  QFile file(R"(:/resource/about.md)");
  if (!file.open(QIODevice::ReadOnly))
  {
    kCritical() << "Failed to open" << file.fileName();
    return;
  }

  QString about = QString::fromUtf8(file.readAll()).arg(kal::SpriteChat::softwareName()).arg(kal::SpriteChat::softwareVersionString()).arg(QLatin1String(__DATE__)).arg(QLatin1String(QT_VERSION_STR)).arg(BASSVERSIONTEXT);
  kal::MessageDialog::information(about, tr("About"), kal::MessageDialog::Markdown, this);
}

void Lobby::filterPublicList()
{
  const QString filter = ui_public_search->text();
  const bool is_filter_empty = filter.isEmpty();
  for (QTreeWidgetItemIterator it(ui_public_tree); *it; ++it)
  {
    QTreeWidgetItem *item = *it;
    if (is_filter_empty)
    {
      item->setHidden(false);
    }
    else
    {
      item->setHidden(!item->text(0).contains(filter));
    }
  }
}

void Lobby::setupCurrentServer()
{
  const auto maybe_server = currentServer();
  ui_connect->setEnabled(maybe_server.has_value());
  displayCurrentServerDescription();
}

// doubleclicked on an item in the serverlist so we'll connect right away
void Lobby::openOptions()
{
  OptionsWindow dialog(options, m_resolver);
  dialog.exec();
}

void Lobby::displayMasterError(const QString &error)
{
  call_error(error);
}

void Lobby::updateMessageOfTheDay()
{
  const QString motd = m_master.messageOfTheDay();
  if (motd.isEmpty())
  {
    ui_motd->setText(tr("No message of the day."));
  }
  else
  {
    ui_motd->setText(motd);
  }
}

void Lobby::updatePublicList()
{
  m_public_list = m_master.serverList();
  updateServerList(ui_public_tree, m_public_list);
}

void Lobby::checkMasterVersion()
{
  QVersionNumber local_version = kal::SpriteChat::softwareVersion();
  QVersionNumber master_version = m_master.version();
  if (local_version < master_version)
  {
    ui_version->setText(tr("Version: %1 [OUTDATED]").arg(local_version.toString()));
    setWindowTitle(tr("[Your client is outdated]"));
    const QString download_url = QString("https://github.com/AttorneyOnlineKaleidoscope/SpriteChat/releases/latest").replace(QRegularExpression("\\b(https?://\\S+\\.\\S+)\\b"), "<a href='\\1'>\\1</a>");
    const QString message = QString("Your client is outdated!<br>Your Version: %1<br>Current Version: %2<br>Download the latest version at<br>%3").arg(local_version.toString(), master_version.toString(), download_url);
    QMessageBox::warning(this, "Your client is outdated!", message);
  }
}

void Lobby::nextServerSortMode()
{
  switch (m_sort)
  {
  case NoSort:
    m_sort = Ascending;
    break;

  case Ascending:
    m_sort = Descending;
    break;

  case Descending:
    m_sort = NoSort;
    break;
  }
  sortServerList();
}

void Lobby::sortServerList()
{
  sortServerList(ui_public_tree);
  sortServerList(ui_favorite_tree);
}

void Lobby::sortServerList(QTreeWidget *tree)
{
  QHeaderView *header = tree->header();
  switch (m_sort)
  {
  case NoSort:
    tree->sortByColumn(1, Qt::AscendingOrder);
    header->setSortIndicator(1, Qt::AscendingOrder);
    break;

  case Ascending:
    tree->sortByColumn(0, Qt::AscendingOrder);
    header->setSortIndicator(0, Qt::AscendingOrder);
    break;

  case Descending:
    tree->sortByColumn(0, Qt::DescendingOrder);
    header->setSortIndicator(0, Qt::DescendingOrder);
    break;
  }
}
