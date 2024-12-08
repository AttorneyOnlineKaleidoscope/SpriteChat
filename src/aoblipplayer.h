#pragma once

#include "assetpathresolver.h"

#include <bass.h>
#include <bassopus.h>

#include <QDebug>
#include <QElapsedTimer>
#include <QWidget>

class AOBlipPlayer
{
public:
  static constexpr int STREAM_COUNT = 5;

  AOBlipPlayer(kal::AssetPathResolver &assetPacketTransport);

  void setVolume(int value);
  void setMuted(bool enabled);

  void setBlip(QString blip);

  void playBlip();

private:
  kal::AssetPathResolver &m_resolver;

  int m_volume = 0;
  bool m_muted = false;
  HSTREAM m_stream[STREAM_COUNT]{};
  int m_cycle = 0;

  void updateInternalVolume();
};
