#pragma once

#include "datatypes.h"

#include <QList>
#include <QMap>
#include <QString>
#include <QStringList>

struct EffectData
{
  QString caption;
  QString layer;
  int duration = 0;  // previously duration
  bool loop = false; // previously loop
  bool cull = false; // previously cull
  bool stretch = false;
  bool sticky = false; // previously sticky
  bool respect_flip = false;
  bool respect_offset = false;
  RESIZE_MODE resize_mode = RESIZE_MODE::AUTO_RESIZE_MODE;
  std::optional<QString> sound;
};

class EffectDataPack
{
public:
  QStringList idList() const;

  bool hasEffect(const QString &id) const;
  EffectData effect(const QString &id) const;

  void read(const QString &fileName);
  void reset();

private:
  QHash<QString, EffectData> m_map;
  QStringList m_id_list;
};
