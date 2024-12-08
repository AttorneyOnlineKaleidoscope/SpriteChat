#pragma once

#include "options.h"

#include <QNetworkAccessManager>
#include <QObject>
#include <QTimer>

namespace kal
{
class MasterApiStatusPublisher : public QObject
{
  Q_OBJECT

public:
  explicit MasterApiStatusPublisher(Options &options, QObject *parent = nullptr);
  virtual ~MasterApiStatusPublisher();

  void post(const QUrl &url, const QString &path, const QByteArray &payload = QByteArray());

public Q_SLOTS:
  void setReporting(bool enabled);
  void postPlayStatus();

private:
  Options &options;
  QNetworkAccessManager m_manager;
  QTimer m_timer;
};
} // namespace kal
