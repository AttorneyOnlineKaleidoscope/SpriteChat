#include "effect.h"

#include <QCollator>
#include <QSettings>

QStringList EffectDataPack::idList() const
{
  return m_id_list;
}

bool EffectDataPack::hasEffect(const QString &id) const
{
  return m_map.contains(id);
}

EffectData EffectDataPack::effect(const QString &id) const
{
  return m_map.value(id);
}

/*!
 * \brief EffectDataPack::read
 * \param fileName The data to load.
 * \note Use reset() before calling this function to clear the existing data.
 */
void EffectDataPack::read(const QString &fileName)
{
  QSettings settings(fileName, QSettings::IniFormat);
  if (settings.status() != QSettings::NoError)
  {
    kWarning() << "Failed to read effects from " % fileName;
    return;
  }

  QStringList group_list = settings.childGroups();
  group_list.removeIf([](const QString &key) { return key.toLower() == "version"; });

  {
    QCollator sorter;
    sorter.setNumericMode(true);
    std::stable_sort(group_list.begin(), group_list.end(), sorter);
  }

  for (const QString &group : group_list)
  {
    settings.beginGroup(group);

    QString id = settings.value("name").toString().toLower();
    if (id.isEmpty())
    {
      kWarning() << QStringLiteral("Invalid effect ID (%1) in %2").arg(group, fileName);
      continue;
    }

    if (!m_map.contains(id))
    {
      m_id_list.append(id);
    }

    EffectData &effect = m_map[id];
    effect.caption = settings.value("caption", id).toString();
    effect.layer = settings.value("layer").toString();
    effect.duration = qMax(0, settings.value("max_duration").toInt());
    effect.loop = settings.value("loop").toBool();
    effect.cull = settings.value("cull").toBool();
    effect.sticky = settings.value("sticky").toBool();
    effect.stretch = settings.value("stretch").toBool();
    effect.resize_mode = resizeModeFromString(settings.value("scaling").toString());
    effect.respect_flip = settings.value("respect_flip").toBool();
    effect.respect_offset = settings.value("respect_offset").toBool();

    if (QString sound = settings.value("sound").toString(); !sound.isEmpty())
    {
      effect.sound = sound;
    }

    settings.endGroup();
  }
}

void EffectDataPack::reset()
{
  m_map.clear();
  m_id_list.clear();
}
