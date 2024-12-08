#include "masterapistatuspublisher.h"

#include "spritechatversion.h"

#include <QNetworkReply>

kal::MasterApiStatusPublisher::MasterApiStatusPublisher(Options &options, QObject *parent)
    : QObject{parent}
    , options{options}
{
  m_timer.setInterval(5000);
  m_timer.setSingleShot(false);

  connect(&options, &Options::playerCountOptoutToggled, this, &MasterApiStatusPublisher::setReporting);

  connect(&m_timer, &QTimer::timeout, this, &MasterApiStatusPublisher ::postPlayStatus);

  setReporting(options.playerCountOptout());
}

kal::MasterApiStatusPublisher::~MasterApiStatusPublisher()
{}

void kal::MasterApiStatusPublisher::post(const QUrl &url, const QString &path, const QByteArray &payload)
{
  if (!url.isValid())
  {
    qWarning().noquote() << "Invalid URL:" << url;
    return;
  }

  QUrl destination(url);
  destination.setPath(path);
  QNetworkRequest request(destination);
  request.setRawHeader("User-Agent", kal::SpriteChat::softwareAgent().toUtf8());
  QNetworkReply *reply = m_manager.post(request, payload);
  connect(reply, &QNetworkReply::finished, reply, &QNetworkReply::deleteLater);
}

void kal::MasterApiStatusPublisher::postPlayStatus()
{
  post(options.alternativeMasterserver(), "/playing");
}

void kal::MasterApiStatusPublisher::setReporting(bool enabled)
{
  if (m_timer.isActive())
  {
    m_timer.stop();
  }

  if (enabled)
  {
    m_timer.start();
  }
}
