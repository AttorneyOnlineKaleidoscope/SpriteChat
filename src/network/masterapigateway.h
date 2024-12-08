#pragma once

#include "network/masterapiclient.h"
#include "network/serverinfo.h"

#include <QVersionNumber>

namespace kal
{
class MasterApiGateway : public QObject
{
  Q_OBJECT

public:
  explicit MasterApiGateway(QObject *parent = nullptr);
  virtual ~MasterApiGateway() {}

  void setOrigin(const QUrl &origin);

  QString messageOfTheDay() const;
  QList<kal::ServerInfo> serverList() const;
  QVersionNumber version() const;

  void request(const QString &path, const MasterApiClient::Callback &callback);

public Q_SLOTS:
  void requestMessageOfTheDay();
  void requestServerList();
  void requestVersion();

Q_SIGNALS:
  void errorOccurred(const QString &error);
  void messageOfTheDayChanged();
  void serverListChanged();
  void versionChanged();

private:
  QUrl m_origin;
  MasterApiClient m_client;
  QString m_motd;
  QList<kal::ServerInfo> m_server_list;
  QVersionNumber m_version;

  void notifyError(const QString &path, const QString &what);
};
} // namespace kal
