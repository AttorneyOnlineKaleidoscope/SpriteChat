#include "datatypes.h"

#include <QHash>

RESIZE_MODE resizeModeFromString(const QString &mode)
{
  static const QHash<QString, RESIZE_MODE> RESIZE_MAP{
      {"pixel", PIXEL_RESIZE_MODE},
      {"smooth", SMOOTH_RESIZE_MODE},
      // legacy
      {"fast", SMOOTH_RESIZE_MODE},
  };
  return RESIZE_MAP.value(mode, AUTO_RESIZE_MODE);
}

Layer layerFromString(const QString &layer)
{
  static const QHash<QString, Layer> layerMap = {
    {"character", CharacterLayer},
    {"desk", DeskLayer},
    {"effect", EffectLayer},
    {"splash", SplashLayer},
    // legacy
    {"behind", StageLayer},
    {"over", DeskLayer},
  };
  return layerMap.value(layer, StageLayer);
}
