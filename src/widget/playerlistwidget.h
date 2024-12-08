#pragma once

#include "assetpathresolver.h"
#include "game/player.h"
#include "network/packet/player_packets.h"
#include "network/packettransport.h"
#include "networksession.h"

#include <QList>
#include <QListWidget>
#include <QMap>

class PlayerListWidget : public QListWidget
{
public:
  explicit PlayerListWidget(Options &options, kal::PacketTransport &transport, kal::AssetPathResolver &assetPathResolver, const kal::NetworkSession &session, QWidget *parent = nullptr);
  virtual ~PlayerListWidget();

  void registerPlayer(const kal::RegisterPlayerPacket &update);
  void updatePlayer(const kal::UpdatePlayerPacket &update);
  void reloadPlayers();

  void setAuthenticated(bool f_state);

private:
  Options &options;
  kal::PacketTransport &m_transport;
  kal::AssetPathResolver &m_resolver;
  const kal::NetworkSession &m_session;
  QMap<int, kal::PlayerData> m_player_map;
  QMap<int, QListWidgetItem *> m_item_map;
  bool m_is_authenticated = false;

  void addPlayer(int playerId);
  void removePlayer(int playerId);
  void updatePlayer(int playerId, bool updateIcon);

  QString formatLabel(const kal::PlayerData &data);

  void filterPlayerList();

private Q_SLOTS:
  void onCustomContextMenuRequested(const QPoint &pos);
};
