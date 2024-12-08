#pragma once

#include "network/masterapiclient.h"
#include "network/serverinfo.h"
#include "options.h"

#include <QVersionNumber>

namespace kal
{
class MasterApiGateway : public QObject
{
  Q_OBJECT

public:
  explicit MasterApiGateway(Options &options, QObject *parent = nullptr);
  virtual ~MasterApiGateway();

  QString messageOfTheDay() const;
  QList<kal::ServerInfo> serverList() const;
  QVersionNumber version() const;
  QString privacyPolicy() const;

  void request(const QString &path, const MasterApiClient::Callback &callback);

public Q_SLOTS:
  void requestMessageOfTheDay();
  void requestServerList();
  void requestVersion();
  void requestPrivacyPolicy();

Q_SIGNALS:
  void errorOccurred(const QString &error);
  void messageOfTheDayChanged();
  void serverListChanged();
  void versionChanged();
  void privacyPolicyChanged();

private:
  Options &options;
  MasterApiClient m_client;
  QString m_motd;
  QList<kal::ServerInfo> m_server_list;
  QVersionNumber m_version;
  QString m_privacy_policy;

  void notifyError(const QString &path, const QString &what);
};
} // namespace kal
