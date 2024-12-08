#pragma once

#include "network/packet.h"
#include "network/packettransport.h"
#include "network/serverinfo.h"
#include "network/socketclient.h"

#include <QElapsedTimer>
#include <QObject>
#include <QQueue>
#include <QTimer>

namespace kal
{
class NetworkManager : public QObject, public PacketTransport
{
  Q_OBJECT

public:
  enum Status
  {
    NotConnected,
    Connecting,
    Connected,
  };

  explicit NetworkManager(QObject *parent = nullptr);
  ~NetworkManager();

  Status status() const;

  kal::ServerInfo server() const;
  void setServer(const kal::ServerInfo &server);
  void setAndConnectToServer(const kal::ServerInfo &server);

  int latency() const;

  void shipPacket(const Packet &packet) override;

  QQueue<Packet> takePacketQueue();

public Q_SLOTS:
  void connectToServer();
  void disconnectFromServer();

Q_SIGNALS:
  void statusChanged(Status);
  void packetReceived();
  void latencyChanged(int);

private:
  Status m_status = NotConnected;
  kal::ServerInfo m_server;
  SocketClient *m_socket = nullptr;
  QQueue<Packet> m_queue;
  QTimer *m_ping_timer = nullptr;
  QElapsedTimer m_latency_timer;
  int m_latency = 0;

  void setStatus(Status status);
  void startPinging();
  void stopPinging();

private Q_SLOTS:
  void checkSocketState();
  void processSocketError(const QString &error);
  void processPacketQueue();
  void ping();
  void updateLatency();
};
} // namespace kal
