#include "aoblipplayer.h"

AOBlipPlayer::AOBlipPlayer(kal::AssetPathResolver &assetPacketTransport)
    : m_resolver(assetPacketTransport)
{}

void AOBlipPlayer::setVolume(int value)
{
  m_volume = value;
  updateInternalVolume();
}

void AOBlipPlayer::setMuted(bool enabled)
{
  m_muted = enabled;
  updateInternalVolume();
}

void AOBlipPlayer::setBlip(QString blip)
{
  QString path = m_resolver.blipFilePath(blip).value_or(QString());
  for (int i = 0; i < STREAM_COUNT; ++i)
  {
    BASS_StreamFree(m_stream[i]);

    if (path.endsWith(".opus"))
    {
      m_stream[i] = BASS_OPUS_StreamCreateFile(FALSE, path.utf16(), 0, 0, BASS_UNICODE | BASS_ASYNCFILE);
    }
    else
    {
      m_stream[i] = BASS_StreamCreateFile(FALSE, path.utf16(), 0, 0, BASS_UNICODE | BASS_ASYNCFILE);
    }
  }

  updateInternalVolume();
}

void AOBlipPlayer::playBlip()
{
  HSTREAM stream = m_stream[m_cycle];
  BASS_ChannelSetDevice(stream, BASS_GetDevice());
  BASS_ChannelPlay(stream, false);
  m_cycle = ++m_cycle % STREAM_COUNT;
}

void AOBlipPlayer::updateInternalVolume()
{
  float volume = m_muted ? 0.0f : (m_volume * 0.01);
  for (int i = 0; i < STREAM_COUNT; ++i)
  {
    BASS_ChannelSetAttribute(m_stream[i], BASS_ATTRIB_VOL, volume);
  }
}
