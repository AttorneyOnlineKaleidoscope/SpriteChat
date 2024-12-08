#pragma once

#include "gameglobal.h"

#include <QObject>
#include <QString>
#include <QUrl>

namespace kal
{
class NetworkSession
{
public:
  kal::PlayerId playerId() const;
  kal::AreaId areaId();
  QUrl assetUrl() const;
  QString assetUrlString() const;

  void setPlayerId(kal::PlayerId playerId);
  void setAreaId(kal::AreaId areaId);
  void setAssetUrl(const QUrl &assetUrl);

private:
  QUrl m_asset_url;
  kal::PlayerId m_player_id = kal::NoPlayerId;
  kal::AreaId m_area_id = kal::NoAreaId;
};
} // namespace kal
