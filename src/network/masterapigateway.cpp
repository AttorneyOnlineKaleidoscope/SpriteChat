#include "masterapigateway.h"

#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>
#include <QNetworkRequest>

kal::MasterApiGateway::MasterApiGateway(Options &options, QObject *parent)
    : QObject{parent}
    , options{options}
{
  connect(&m_client, &MasterApiClient::errorOccurred, this, &MasterApiGateway::errorOccurred);
}

kal::MasterApiGateway::~MasterApiGateway()
{}

QString kal::MasterApiGateway::messageOfTheDay() const
{
  return m_motd;
}

QList<kal::ServerInfo> kal::MasterApiGateway::serverList() const
{
  return m_server_list;
}

QVersionNumber kal::MasterApiGateway::version() const
{
  return m_version;
}

QString kal::MasterApiGateway::privacyPolicy() const
{
  return m_privacy_policy;
}

void kal::MasterApiGateway::request(const QString &path, const MasterApiClient::Callback &callback)
{
  m_client.request(options.alternativeMasterserver(), path, callback);
}

void kal::MasterApiGateway::requestMessageOfTheDay()
{
  const QString path = QStringLiteral("/motd");
  request(path, [this, path](const QByteArray &payload) {
    m_motd = QString::fromUtf8(payload);
    Q_EMIT messageOfTheDayChanged();
  });
}

void kal::MasterApiGateway::requestServerList()
{
  const QString path = QStringLiteral("/servers");
  request(path, [this, path](const QByteArray &payload) {
    QJsonDocument document;
    {
      QJsonParseError error;
      document = QJsonDocument::fromJson(payload, &error);
      if (error.error)
      {
        notifyError(path, error.errorString());
        return;
      }
    }

    if (!document.isArray())
    {
      notifyError(path, "Invalid JSON document; expected array");
      return;
    }

    QList<kal::ServerInfo> server_list;
    QJsonArray array = document.array();
    for (auto it = array.begin(); it != array.end(); ++it)
    {
      if (!it->isObject())
      {
        notifyError(path, "Invalid JSON document; expected object");
        return;
      }
      QJsonObject raw_server = it->toObject();

      kal::ServerInfo server;
      server.name = raw_server.value("name").toString();
      server.description = raw_server.value("description").toString();
      server.address = raw_server.value("ip").toString();
      if (raw_server.contains("ws_port"))
      {
        server.port = raw_server.value("ws_port").toInt();
      }
      else
      {
        server.port = raw_server.value("port").toInt();
        server.legacy = true;
      }
      server_list.append(server);
    }
    m_server_list = std::move(server_list);
    Q_EMIT serverListChanged();
  });
}

void kal::MasterApiGateway::requestVersion()
{
  const QString path = QStringLiteral("/version");
  request(path, [this](const QByteArray &payload) {
    m_version = QVersionNumber::fromString(payload);
    Q_EMIT versionChanged();
  });
}

void kal::MasterApiGateway::requestPrivacyPolicy()
{
  request("/privacy", [this](const QByteArray &payload) {
    m_privacy_policy = QString(payload);
    Q_EMIT privacyPolicyChanged();
  });
}

void kal::MasterApiGateway::notifyError(const QString &path, const QString &what)
{
  Q_EMIT errorOccurred(QStringLiteral("API request %1 failed: %2").arg(path, what));
}
