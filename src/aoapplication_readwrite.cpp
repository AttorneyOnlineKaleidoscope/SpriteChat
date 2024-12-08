#include "aoapplication.h"

#include "spritechatcommon.h"
#include "file_functions.h"
#include "options.h"

#include <QCollator>
#include <QColor>
#include <QDebug>
#include <QList>
#include <QSettings>
#include <QStringList>
#include <QTextStream>

QStringList AOApplication::get_list_file(QString p_file)
{
  QStringList list;
  QFile file(p_file);
  if (file.open(QIODevice::ReadOnly))
  {
    QTextStream in(&file);
    while (!in.atEnd())
    {
      list.append(in.readLine());
    }
  }
  else
  {
    kWarning() << "Failed to read" << p_file << "(" << file.errorString() << ")";
  }

  return list;
}

QString AOApplication::read_file(QString filename)
{
  if (filename.isEmpty())
  {
    return QString();
  }

  QFile f_log(filename);

  if (!f_log.open(QIODevice::ReadOnly | QIODevice::Text))
  {
    kWarning() << "Couldn't open" << filename << "for reading";
    return QString();
  }

  QTextStream in(&f_log);
  QString text = in.readAll();
  f_log.close();
  return text;
}

bool AOApplication::write_to_file(QString p_text, QString p_file, bool make_dir)
{
  QString path = QFileInfo(p_file).path();
  if (make_dir)
  {
    // Create the dir if it doesn't exist yet
    QDir dir(path);
    if (!dir.exists())
    {
      if (!dir.mkpath("."))
      {
        return false;
      }
    }
  }

  QFile f_log(p_file);
  if (f_log.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate))
  {
    QTextStream out(&f_log);
    out << p_text;

    f_log.flush();
    f_log.close();
    return true;
  }
  return false;
}

bool AOApplication::append_to_file(QString p_text, QString p_file, bool make_dir)
{
  if (!file_exists(p_file)) // Don't create a newline if file didn't exist before now
  {
    return write_to_file(p_text, p_file, make_dir);
  }
  QString path = QFileInfo(p_file).path();
  // Create the dir if it doesn't exist yet
  if (make_dir)
  {
    QDir dir(path);
    if (!dir.exists())
    {
      if (!dir.mkpath("."))
      {
        return false;
      }
    }
  }

  QFile f_log(p_file);
  if (f_log.open(QIODevice::WriteOnly | QIODevice::Append))
  {
    QTextStream out(&f_log);
    out << "\r\n" << p_text;

    f_log.flush();
    f_log.close();
    return true;
  }
  return false;
}

QString AOApplication::read_design_ini(QString p_identifier, QString p_design_path)
{
  QSettings settings(p_design_path, QSettings::IniFormat);
  QVariant value = settings.value(p_identifier);
  if (value.typeId() == QMetaType::QStringList)
  {
    return value.toStringList().join(",");
  }
  else if (!value.isNull())
  {
    return value.toString();
  }
  return "";
}

QPoint AOApplication::get_button_spacing(QString p_identifier, QString p_file)
{
  auto path = m_resolver.currentThemeFilePath(p_file);
  if (!path)
  {
    return QPoint();
  }

  QString raw_point = get_config_value(path.value(), p_identifier);
  if (raw_point.isEmpty())
  {
    return QPoint();
  }

  QStringList coordinates = raw_point.split(",");
  if (coordinates.size() != 2)
  {
    return QPoint();
  }

  QPoint point;
  point.setX(coordinates[0].toInt() * options.themeScalingFactor());
  point.setY(coordinates[1].toInt() * options.themeScalingFactor());
  return point;
}

pos_size_type AOApplication::get_element_dimensions(QString p_identifier, QString p_file, QString p_misc)
{
  pos_size_type return_value;
  return_value.x = 0;
  return_value.y = 0;
  return_value.width = -1;
  return_value.height = -1;
  QString f_result = get_design_element(p_identifier, p_file, p_misc);

  QStringList sub_line_elements = f_result.split(",");

  if (sub_line_elements.size() < 4)
  {
    return return_value;
  }

  int scale = options.themeScalingFactor();

  return_value.x = sub_line_elements.at(0).toInt() * scale;
  return_value.y = sub_line_elements.at(1).toInt() * scale;
  return_value.width = sub_line_elements.at(2).toInt() * scale;
  return_value.height = sub_line_elements.at(3).toInt() * scale;

  return return_value;
}

QString AOApplication::get_design_element(QString p_identifier, QString p_file, QString p_misc)
{
  if (auto path = m_resolver.currentThemeFilePath(p_file))
  {
    return get_config_value(path.value(), p_identifier);
  }

  return QString();
}

QColor AOApplication::get_color(const QString &identifier, const QString &fileName, const QColor &defaultColor)
{
  auto path = m_resolver.currentThemeFilePath(fileName);
  if (!path)
  {
    kWarning() << "Failed to read color from" << fileName << "(file not found)";
    return defaultColor;
  }

  QString raw_color = get_config_value(path.value(), identifier);
  if (raw_color.isEmpty())
  {
    kWarning() << "Failed to read color from" << fileName << "(no color found)";
    return defaultColor;
  }

  QStringList color_channels = raw_color.split(",");
  if (color_channels.size() < 3)
  {
    kWarning() << "Failed to read color from" << fileName << "(invalid color)";
    return defaultColor;
  }

  QColor color;
  color.setRed(color_channels.at(0).toInt());
  color.setGreen(color_channels.at(1).toInt());
  color.setBlue(color_channels.at(2).toInt());
  if (color_channels.size() == 4)
  {
    color.setAlpha(color_channels.at(3).toInt());
  }

  if (!color.isValid())
  {
    kWarning() << "Failed to read color from" << fileName << "(invalid color)";
    return defaultColor;
  }

  return color;
}

QString AOApplication::get_chat_markup(QString p_identifier, QString p_chat)
{
  if (auto path = m_resolver.chatboxFilePath(p_chat, "chat_config.ini"))
  {
    return get_config_value(path.value(), p_identifier);
  }

  return QString();
}

QColor AOApplication::get_chat_color(QString p_identifier, QString p_chat)
{
  QColor return_color(255, 255, 255);
  QString f_result = get_chat_markup(p_identifier, p_chat);
  if (f_result == "")
  {
    return return_color;
  }

  QStringList color_list = f_result.split(",");

  if (color_list.size() < 3)
  {
    return return_color;
  }

  return_color.setRed(color_list.at(0).toInt());
  return_color.setGreen(color_list.at(1).toInt());
  return_color.setBlue(color_list.at(2).toInt());

  return return_color;
}

QString AOApplication::get_penalty_value(QString p_identifier)
{
  if (auto path = m_resolver.currentThemeFilePath("penalty/penalty.ini"))
  {
    return get_config_value(path.value(), p_identifier);
  }

  return QString();
}

QString AOApplication::get_court_sfx(QString p_identifier, QString p_misc)
{
  if (auto path = m_resolver.currentThemeFilePath("courtroom_design.ini"))
  {
    return get_config_value(path.value(), p_identifier);
  }

  return QString();
}

QList<SfxItem> AOApplication::get_sfx_list(QString file_name)
{
  QList<SfxItem> list;

  QFile file(file_name);
  if (!file.open(QIODevice::ReadOnly))
  {
    kWarning() << "Failed to open sfx file" << file_name << ":" << file.errorString();
    return list;
  }

  QTextStream in(&file);
  while (!in.atEnd())
  {
    QStringList raw = in.readLine().split("=");
    if (raw.isEmpty())
    {
      continue;
    }

    SfxItem item;
    item.filename = raw.value(0);
    list.append(item);
  }
  file.close();

  return list;
}

QString AOApplication::read_char_ini(QString p_char, QString p_search_line, QString target_tag)
{
  kal::MaybePath path = m_resolver.characterFilePath(p_char, kal::CHARACTER_DATA_FILE);
  if (!path)
  {
    return QString();
  }

  QSettings settings(path.value(), QSettings::IniFormat);
  settings.beginGroup(target_tag);
  QString value = settings.value(p_search_line).value<QString>();
  settings.endGroup();
  return value;
}

QStringList AOApplication::read_ini_tags(QString p_path, QString target_tag)
{
  QStringList r_values;
  QSettings settings(p_path, QSettings::IniFormat);
  if (!target_tag.isEmpty())
  {
    settings.beginGroup(target_tag);
  }
  QStringList keys = settings.allKeys();
  for (QString key : keys)
  {
    QString value = settings.value(key).value<QString>();
    r_values << key + "=" + value;
  }
  if (!settings.group().isEmpty())
  {
    settings.endGroup();
  }
  return r_values;
}

// returns whatever is to the right of "search_line =" within target_tag and
// terminator_tag, trimmed returns the empty string if the search line couldnt
// be found
QString AOApplication::get_stylesheet(QString p_file)
{
  auto path = m_resolver.currentThemeFilePath(p_file);
  if (path)
  {
    QFile file;
    file.setFileName(path.value());
    if (!file.open(QIODevice::ReadOnly))
    {
      kWarning() << "Failed to open file" << path.value() << "(" << file.errorString() << ")";
      return QString();
    }

    return QString(file.readAll());
  }

  return QString();
}

// returns all the values of target_tag
QString AOApplication::get_char_side(QString p_char)
{
  QString f_result = read_char_ini(p_char, "side", "Options");

  if (f_result == "")
  {
    return "wit";
  }
  return f_result;
}

QString AOApplication::get_showname(QString p_char, int p_emote)
{
  QString f_result = read_char_ini(p_char, "showname", "Options");
  QString f_needed = read_char_ini(p_char, "needs_showname", "Options");

  if (p_emote != -1)
  {
    int override_idx = read_char_ini(p_char, QString::number(p_emote + 1), "OptionsN").toInt();
    if (override_idx > 0)
    {
      QString override_key = "Options" + QString::number(override_idx);
      QString temp_f_result = read_char_ini(p_char, "showname", override_key);
      if (!temp_f_result.isEmpty())
      {
        f_result = temp_f_result;
      }
    }
  }

  if (f_needed.startsWith("false"))
  {
    return "";
  }
  if (f_result == "")
  {
    return p_char;
  }
  return f_result;
}

QString AOApplication::get_category(QString p_char)
{
  QString f_result = read_char_ini(p_char, "category", "Options");
  return f_result;
}

QString AOApplication::get_chat(QString p_char)
{
  if (p_char == "default")
  {
    return "default";
  }
  QString f_result = read_char_ini(p_char, "chat", "Options");
  return f_result;
}
QString AOApplication::get_chat_font(QString p_char)
{
  QString f_result = read_char_ini(p_char, "chat_font", "Options");

  return f_result;
}

int AOApplication::get_chat_size(QString p_char)
{
  QString f_result = read_char_ini(p_char, "chat_size", "Options");

  if (f_result == "")
  {
    return -1;
  }
  return f_result.toInt();
}

int AOApplication::get_preanim_duration(QString p_char, QString p_emote)
{
  QString f_result = read_char_ini(p_char, p_emote, "Time");

  if (f_result == "")
  {
    return -1;
  }
  return f_result.toInt();
}

int AOApplication::get_text_delay(QString p_char, QString p_emote)
{
  QString f_result = read_char_ini(p_char, p_emote, "stay_time");

  if (f_result == "")
  {
    return -1;
  }
  return f_result.toInt();
}

bool AOApplication::get_pos_is_judge(const QString &background, const QString &p_pos)
{
  QStringList positions = read_design_ini("judges", m_resolver.backgroundFilePath(background, "design.ini").value_or(QString())).split(",");
  if (positions.size() == 1 && positions[0] == "")
  {
    return p_pos == "jud"; // Hardcoded BS only if we have no judges= defined
  }
  return positions.contains(p_pos.trimmed());
}

int AOApplication::get_pos_transition_duration(const QString &background, const QString &old_pos, const QString &new_pos)
{
  if (old_pos.split(":").size() < 2 || new_pos.split(":").size() < 2)
  {
    return -1; // no subpositions
  }

  QString new_subpos = new_pos.split(":")[1];

  bool ok;
  int duration = read_design_ini(old_pos + "/slide_ms_" + new_subpos, m_resolver.backgroundFilePath(background, "design.ini").value_or(QString())).toInt(&ok);
  if (ok)
  {
    return duration;
  }
  else
  {
    return -1; // invalid
  }
}

int AOApplication::get_emote_number(QString p_char)
{
  QString f_result = read_char_ini(p_char, "number", "Emotions");

  if (f_result == "")
  {
    return 0;
  }
  return f_result.toInt();
}

QString AOApplication::get_emote_comment(QString p_char, int p_emote)
{
  QString f_result = read_char_ini(p_char, QString::number(p_emote + 1), "Emotions");

  QStringList result_contents = f_result.split("#");

  if (result_contents.size() < 4)
  {
    kWarning() << "misformatted char.ini: " << p_char << ", " << p_emote;
    return "normal";
  }
  return result_contents.at(0);
}

QString AOApplication::get_emote(QString p_char, int p_emote)
{
  QString f_result = read_char_ini(p_char, QString::number(p_emote + 1), "Emotions");

  QStringList result_contents = f_result.split("#");

  if (result_contents.size() < 4)
  {
    kWarning() << "misformatted char.ini: " << p_char << ", " << p_emote;
    return "normal";
  }
  return result_contents.at(2);
}

QString AOApplication::get_pre_emote(QString p_char, int p_emote)
{
  QString f_result = read_char_ini(p_char, QString::number(p_emote + 1), "Emotions");

  QStringList result_contents = f_result.split("#");

  if (result_contents.size() < 4)
  {
    kWarning() << "misformatted char.ini: " << p_char << ", " << p_emote;
    return "";
  }
  return result_contents.at(1);
}

QString AOApplication::get_sfx_name(QString p_char, int p_emote)
{
  QString f_result = read_char_ini(p_char, QString::number(p_emote + 1), "SoundN");

  if (f_result == "")
  {
    return "1";
  }
  return f_result;
}

QString AOApplication::get_sfx_looping(QString p_char, int p_emote)
{
  QString f_result = read_char_ini(p_char, QString::number(p_emote + 1), "SoundL");

  if (f_result == "")
  {
    return "0";
  }
  else
  {
    return f_result;
  }
}

QString AOApplication::get_sfx_frame(QString p_char, QString p_emote, int n_frame)
{
  QString f_result = read_char_ini(p_char, QString::number(n_frame), p_emote.append("_FrameSFX"));

  if (f_result == "")
  {
    return "";
  }
  return f_result;
}

QString AOApplication::get_flash_frame(QString p_char, QString p_emote, int n_frame)
{
  QString f_result = read_char_ini(p_char, QString::number(n_frame), p_emote.append("_FrameRealization"));

  if (f_result == "")
  {
    return "";
  }
  return f_result;
}

QString AOApplication::get_screenshake_frame(QString p_char, QString p_emote, int n_frame)
{
  QString f_result = read_char_ini(p_char, QString::number(n_frame), p_emote.append("_FrameScreenshake"));

  if (f_result == "")
  {
    return "";
  }
  return f_result;
}

int AOApplication::get_sfx_delay(QString p_char, int p_emote)
{
  QString f_result = read_char_ini(p_char, QString::number(p_emote + 1), "SoundT");

  if (f_result == "")
  {
    return 1;
  }
  return f_result.toInt();
}

int AOApplication::get_emote_mod(QString p_char, int p_emote)
{
  QString f_result = read_char_ini(p_char, QString::number(p_emote + 1), "Emotions");

  QStringList result_contents = f_result.split("#");

  if (result_contents.size() < 4)
  {
    kWarning() << "misformatted char.ini: " << p_char << ", " << QString::number(p_emote);
    return IDLE;
  }

  bool result = false;
  int type = result_contents.at(3).toInt(&result);
  if (!result)
  {
    switch (type)
    {
    default:
      kWarning() << "Invalid emote type" << type << "for character" << p_char << ", emote" << p_emote;
      type = IDLE;
      break;

    case IDLE:
      break;

    case LEGACY_ZOOM:
      type = ZOOM;
      break;
    }
  }
  else
  {
    kWarning() << "Invalid emote type" << type << "for character" << p_char << ", emote" << p_emote;
    type = IDLE;
  }

  return type;
}

int AOApplication::get_desk_mod(QString p_char, int p_emote)
{
  QString f_result = read_char_ini(p_char, QString::number(p_emote + 1), "Emotions");

  QStringList result_contents = f_result.split("#");
  if (result_contents.size() < 5)
  {
    return DESK_SHOW;
  }

  bool result;
  int type = result_contents.at(4).toInt(&result);
  if (!result)
  {
    switch (type)
    {
    default:
      kWarning() << "Invalid desk type" << type << "for character" << p_char << ", emote" << p_emote;
      type = DESK_SHOW;
      break;

    case DESK_HIDE:
    case DESK_SHOW:
    case DESK_EMOTE_ONLY:
    case DESK_PRE_ONLY:
    case DESK_EMOTE_ONLY_EX:
    case DESK_PRE_ONLY_EX:
      break;
    }
  }
  else
  {
    kWarning() << "Invalid desk type" << type << "for character" << p_char << ", emote" << p_emote;
    type = DESK_SHOW;
  }

  return type;
}

QString AOApplication::get_blipname(QString p_char, int p_emote)
{
  QString f_result = read_char_ini(p_char, "blips", "Options");

  if (p_emote != -1)
  {
    int override_idx = read_char_ini(p_char, QString::number(p_emote + 1), "OptionsN").toInt();
    if (override_idx > 0)
    {
      QString override_key = "Options" + QString::number(override_idx);
      QString temp_f_result = read_char_ini(p_char, "blips", override_key);
      if (!temp_f_result.isEmpty())
      {
        f_result = temp_f_result;
      }
    }
  }

  if (f_result == "")
  {
    f_result = read_char_ini(p_char, "gender", "Options"); // not very PC, FanatSors
    if (f_result == "")
    {
      f_result = "male";
    }
  }
  return f_result;
}

QString AOApplication::get_emote_property(QString p_char, QString p_emote, QString p_property)
{
  QString f_result = read_char_ini(p_char, p_emote, p_property); // per-emote override
  if (f_result == "")
  {
    f_result = read_char_ini(p_char, p_property,
                             "Options"); // global for this character
  }
  return f_result;
}

RESIZE_MODE AOApplication::get_scaling(QString p_scaling)
{
  RESIZE_MODE mode = options.resizeMode();
  if (mode == AUTO_RESIZE_MODE)
  {
    mode = resizeModeFromString(p_scaling);
  }

  return mode;
}

RESIZE_MODE AOApplication::get_chatbox_scaling(const QString &chatbox)
{
  if (auto path = m_resolver.chatboxFilePath(chatbox, "config.ini"))
  {
    return resizeModeFromString(read_design_ini("scaling", path.value()));
  }

  return AUTO_RESIZE_MODE;
}
