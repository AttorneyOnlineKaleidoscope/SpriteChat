#include "aoapplication.h"

#include "courtroom.h"

QString AOApplication::get_config_value(const QString &fileName, const QString &identifier)
{
  QSettings settings(fileName, QSettings::IniFormat);
  if (settings.status() != QSettings::NoError)
  {
    kDebug() << "Error reading" << fileName << ":" << settings.status();
    return QString();
  }

  QVariant value = settings.value(identifier);
  if (value.typeId() == QMetaType::QStringList)
  {
    return value.toStringList().join(",");
  }

  return value.toString();
}

BackgroundPosition AOApplication::get_pos_path(const QString &background, const QString &position)
{
  // witness is default if pos is invalid
  QString f_pos = position;

  // legacy overrides for new format if found
  if (m_resolver.backgroundFilePath(background, "court", kal::AnimatedImageAssetType))
  {
    if (!read_design_ini("court:" + f_pos + "/origin", m_resolver.backgroundFilePath(background, "design.ini").value_or(QString())).isEmpty())
    {
      f_pos = QString("court:%1").arg(f_pos);
    }
  }
  QStringList f_pos_split = f_pos.split(":");

  std::optional<int> origin;
  {
    bool ok;
    int result = read_design_ini(f_pos + "/origin", m_resolver.backgroundFilePath(background, "design.ini").value_or(QString())).toInt(&ok);
    if (ok)
    {
      origin = result;
    }
  }

  QString f_background;
  QString f_desk_image;
  if (m_resolver.backgroundFilePath(background, "witnessempty", kal::AnimatedImageAssetType))
  {
    f_background = "witnessempty";
    f_desk_image = "stand";
  }
  else
  {
    f_background = "wit";
    f_desk_image = "wit_overlay";
  }

  if (position == "def" && m_resolver.backgroundFilePath(background, "defenseempty", kal::AnimatedImageAssetType))
  {
    f_background = "defenseempty";
    f_desk_image = "defensedesk";
  }
  else if (position == "pro" && m_resolver.backgroundFilePath(background, "prosecutorempty", kal::AnimatedImageAssetType))
  {
    f_background = "prosecutorempty";
    f_desk_image = "prosecutiondesk";
  }
  else if (position == "jud" && m_resolver.backgroundFilePath(background, "judgestand", kal::AnimatedImageAssetType))
  {
    f_background = "judgestand";
    f_desk_image = "judgedesk";
  }
  else if (position == "hld" && m_resolver.backgroundFilePath(background, "helperstand", kal::AnimatedImageAssetType))
  {
    f_background = "helperstand";
    f_desk_image = "helperdesk";
  }
  else if (position == "hlp" && m_resolver.backgroundFilePath(background, "prohelperstand", kal::AnimatedImageAssetType))
  {
    f_background = "prohelperstand";
    f_desk_image = "prohelperdesk";
  }
  else if (position == "jur" && m_resolver.backgroundFilePath(background, "jurystand", kal::AnimatedImageAssetType))
  {
    f_background = "jurystand";
    f_desk_image = "jurydesk";
  }
  else if (position == "sea" && m_resolver.backgroundFilePath(background, "seancestand", kal::AnimatedImageAssetType))
  {
    f_background = "seancestand";
    f_desk_image = "seancedesk";
  }

  if (m_resolver.backgroundFilePath(background, f_pos_split[0], kal::AnimatedImageAssetType)) // Unique pos path
  {
    f_background = f_pos_split[0];
    f_desk_image = f_pos_split[0] + "_overlay";
  }

  QString desk_override = read_design_ini("overlays/" + f_background, m_resolver.backgroundFilePath(background, "design.ini").value_or(QString()));
  if (desk_override != "")
  {
    f_desk_image = desk_override;
  }

  return {f_background, f_desk_image, origin};
}
