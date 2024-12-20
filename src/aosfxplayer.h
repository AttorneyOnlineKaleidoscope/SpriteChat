#pragma once

#include "aoapplication.h"

#include <bass.h>
#include <bassopus.h>

#include <QDebug>
#include <QWidget>

class AOSfxPlayer
{
public:
  static constexpr int STREAM_COUNT = 5;

  AOSfxPlayer(AOApplication &ao_app, kal::AssetPathResolver &assetPathResolver);

  int volume();
  void setVolume(int value);

  void play(QString path);
  void stop(int streamId = -1);
  void stopAll();
  void stopAllLoopingStream();

  void findAndPlaySfx(QString sfx);
  void findAndPlayCharacterSfx(QString sfx, QString character);
  void findAndPlayCharacterShout(QString shout, QString character, QString pack);

  void setMuted(bool toggle);
  void setLooping(bool toggle, int streamId = -1);

private:
  AOApplication &ao_app;
  kal::AssetPathResolver &m_resolver;

  int m_volume = 0;
  bool m_muted = false;
  bool m_looping = true;
  HSTREAM m_stream[STREAM_COUNT]{};
  int m_current_stream_id = 0;

  int maybeFetchCurrentStreamId(int streamId);
  bool ensureValidStreamId(int streamId);
  void updateInternalVolume();
};
