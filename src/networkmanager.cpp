#include "networkmanager.h"

#include "datatypes.h"
#include "debug_functions.h"
#include "lobby.h"
#include "options.h"

#include <QAbstractSocket>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QNetworkReply>

constinit int PING_INTERVAL = 5000;

kal::NetworkManager::NetworkManager(QObject *parent)
    : QObject(parent)
{}

kal::NetworkManager::~NetworkManager()
{}

kal::NetworkManager::Status kal::NetworkManager::status() const
{
  return m_status;
}

kal::ServerInfo kal::NetworkManager::server() const
{
  return m_server;
}

void kal::NetworkManager::setServer(const kal::ServerInfo &server)
{
  disconnectFromServer();
  m_server = server;
}

void kal::NetworkManager::setAndConnectToServer(const kal::ServerInfo &server)
{
  setServer(server);
  connectToServer();
}

int kal::NetworkManager::latency() const
{
  return m_latency;
}

void kal::NetworkManager::shipPacket(const kal::Packet &packet)
{
  if (!m_socket)
  {
    kCritical() << "Failed to ship packet; no connection.";
    return;
  }

  m_socket->shipPacket(packet);
}

QQueue<kal::Packet> kal::NetworkManager::takePacketQueue()
{
  return QQueue<kal::Packet>(std::move(m_queue));
}

void kal::NetworkManager::connectToServer()
{
  disconnectFromServer();
  kInfo() << QObject::tr("Connecting to %1").arg(m_server.toString());

  m_socket = new kal::SocketClient(this);
  connect(m_socket, &kal::SocketClient::stateChanged, this, &kal::NetworkManager::checkSocketState);
  connect(m_socket, &kal::SocketClient::errorOccurred, this, &NetworkManager::processSocketError);
  connect(m_socket, &kal::SocketClient::pendingPacketAvailable, this, &kal::NetworkManager::processPacketQueue);

  QUrl url;
  url.setScheme("ws");
  url.setHost(m_server.address);
  url.setPort(m_server.port);

  setStatus(Connecting);
  m_socket->open(url);
}

void kal::NetworkManager::disconnectFromServer()
{
  stopPinging();

  if (m_socket)
  {
    m_socket->close();
    m_socket->deleteLater();
    m_socket = nullptr;
  }
}

void kal::NetworkManager::setStatus(Status status)
{
  if (m_status == status)
  {
    return;
  }
  m_status = status;
  Q_EMIT statusChanged(status);
}

void kal::NetworkManager::startPinging()
{
  stopPinging();
  m_latency = 0;
  m_ping_timer = new QTimer(this);
  m_ping_timer->setSingleShot(false);
  m_ping_timer->setInterval(PING_INTERVAL);
  QWebSocket *socket = m_socket->websocket();
  connect(socket, &QWebSocket::pong, this, &kal::NetworkManager::updateLatency);
  connect(m_ping_timer, &QTimer::timeout, this, &kal::NetworkManager::ping);
  m_ping_timer->start();
  ping();
}

void kal::NetworkManager::stopPinging()
{
  if (m_ping_timer)
  {
    if (m_ping_timer->isActive())
    {
      m_ping_timer->stop();
    }
    m_ping_timer->deleteLater();
    m_ping_timer = nullptr;
  }
}

void kal::NetworkManager::checkSocketState()
{
  switch (m_socket->state())
  {
  default:
    break;

  case QAbstractSocket::UnconnectedState:
    stopPinging();
    setStatus(NotConnected);
    break;

  case QAbstractSocket::ConnectingState:
    setStatus(Connecting);
    break;

  case QAbstractSocket::ConnectedState:
    setStatus(Connected);
    startPinging();
    break;
  }
}

void kal::NetworkManager::processSocketError(const QString &error)
{
  kWarning() << "Socket error:" << error;
}

void kal::NetworkManager::processPacketQueue()
{
  while (m_socket->hasPendingPacket())
  {
    m_queue.enqueue(m_socket->nextPendingPacket());
  }
  Q_EMIT packetReceived();
}

void kal::NetworkManager::ping()
{
  QWebSocket *socket = m_socket->websocket();
  m_latency_timer.restart();
  socket->ping();
}

void kal::NetworkManager::updateLatency()
{
  m_latency = m_latency_timer.elapsed();
  Q_EMIT latencyChanged(m_latency);
}
