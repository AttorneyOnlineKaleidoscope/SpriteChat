#pragma once

#include <QHash>
#include <QMap>
#include <QString>

enum CHAT_MESSAGE
{
  DESK_MOD = 0,
  PRE_EMOTE,
  CHAR_NAME,
  EMOTE,
  MESSAGE,
  SIDE,
  SFX_NAME,
  EMOTE_MOD,
  CHAR_ID,
  SFX_DELAY,
  OBJECTION_MOD,
  EVIDENCE_ID,
  FLIP,
  REALIZATION,
  TEXT_COLOR,
  SHOWNAME,
  OTHER_CHARID,
  OTHER_NAME,
  OTHER_EMOTE,
  SELF_OFFSET,
  OTHER_OFFSET,
  OTHER_FLIP,
  IMMEDIATE,
  LOOPING_SFX,
  SCREENSHAKE,
  FRAME_SCREENSHAKE,
  FRAME_REALIZATION,
  FRAME_SFX,
  ADDITIVE,
  EFFECTS,
  BLIPNAME,
  SLIDE,
  CHAT_MESSAGE_SIZE,
};

enum EMOTE_MOD_TYPE
{
  IDLE = 0,
  PREANIM = 1,
  LEGACY_ZOOM = 4,
  ZOOM = 5,
  PREANIM_ZOOM = 6,
};

enum DESK_MOD_TYPE
{
  DESK_HIDE,
  DESK_SHOW,
  DESK_EMOTE_ONLY,
  DESK_PRE_ONLY,
  DESK_EMOTE_ONLY_EX,
  DESK_PRE_ONLY_EX,
};

enum RESIZE_MODE
{
  AUTO_RESIZE_MODE,
  PIXEL_RESIZE_MODE,
  SMOOTH_RESIZE_MODE,
};
RESIZE_MODE resizeModeFromString(const QString &mode);

struct CharacterSlot
{
  QString name;
  bool taken = false;
};

struct AreaSlot
{
  QString name;
  int player_count = 0;
  QString status;
  QString cm;
  bool locked = false;
};

class BackgroundPosition
{
public:
  QString background;
  QString desk;
  std::optional<int> origin;
};

struct pos_size_type
{
  int x = 0;
  int y = 0;
  int width = 0;
  int height = 0;
};

class SfxItem
{
public:
  QString name;
  QString filename;

  inline QString nameOrFilename() const { return name.isEmpty() ? filename : name; }
};

enum Layer
{
  StageLayer,
  CharacterLayer,
  DeskLayer,
  EffectLayer,
  SplashLayer,
};
Layer layerFromString(const QString &layer);
