#include "networksession.h"

kal::PlayerId kal::NetworkSession::playerId() const
{
  return m_player_id;
}

kal::AreaId kal::NetworkSession::areaId()
{
  return m_area_id;
}

QUrl kal::NetworkSession::assetUrl() const
{
  return m_asset_url;
}

QString kal::NetworkSession::assetUrlString() const
{
  return m_asset_url.toString();
}

void kal::NetworkSession::setPlayerId(PlayerId playerId)
{
  m_player_id = playerId;
}

void kal::NetworkSession::setAreaId(AreaId areaId)
{
  m_area_id = areaId;
}

void kal::NetworkSession::setAssetUrl(const QUrl &assetUrl)
{
  m_asset_url = assetUrl;
}
