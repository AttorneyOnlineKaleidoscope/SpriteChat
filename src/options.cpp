#include "options.h"

#include "assetpathresolver.h"
#include "file_functions.h"

#include <QCoreApplication>
#include <QDebug>
#include <QFile>
#include <QLocale>
#include <QObject>
#include <QRegularExpression>
#include <QSize>

Options *Options::self = nullptr;

Options::Options(QObject *parent)
    : QObject(parent)
    , m_config("base/config.ini", QSettings::IniFormat)
    , m_favorite_servers("base/favorite_servers.ini", QSettings::IniFormat)
{
  Q_ASSERT_X(self == nullptr, Q_FUNC_INFO, "Only one instance is allowed");
  self = this;
}

Options::~Options()
{
  self = nullptr;
}

QString Options::theme() const
{
  return m_config.value("theme", "AceAttorney2x").toString();
}

void Options::setTheme(QString value)
{
  m_config.setValue("theme", value);
}

int Options::themeScalingFactor() const
{
  int value = m_config.value("theme_scaling_factor", "1").toInt();
  if (value <= 0)
  {
    value = 1;
  }
  return value;
}

/*! Migrate old configuration keys/values to a relevant format. */
void Options::setThemeScalingFactor(int value)
{
  m_config.setValue("theme_scaling_factor", value);
}

int Options::blipRate() const
{
  return m_config.value("blip_rate", 2).toInt();
}

void Options::setBlipRate(int value)
{
  m_config.setValue("blip_rate", value);
}

bool Options::blankBlip() const
{
  return m_config.value("blank_blip", false).toBool();
}

void Options::setBlankBlip(bool value)
{
  m_config.setValue("blank_blip", value);
}

bool Options::loopingSfx() const
{
  return m_config.value("looping_sfx", true).toBool();
}

void Options::setLoopingSfx(bool value)
{
  m_config.setValue("looping_sfx", value);
}

bool Options::objectionStopMusic() const
{
  return m_config.value("objection_stop_music", false).toBool();
}

void Options::setObjectionStopMusic(bool value)
{
  m_config.setValue("objection_stop_music", value);
}

bool Options::streamingEnabled() const
{
  return m_config.value("streaming_enabled", true).toBool();
}

void Options::setStreamingEnabled(bool value)
{
  m_config.setValue("streaming_enabled", value);
}

int Options::musicVolume() const
{
  return m_config.value("default_music", 50).toInt();
}

void Options::setMusicVolume(int value)
{
  m_config.setValue("default_music", value);
}

int Options::sfxVolume() const
{
  return m_config.value("default_sfx", 50).toInt();
}

void Options::setSfxVolume(int value)
{
  m_config.setValue("default_sfx", value);
}

int Options::blipVolume() const
{
  return m_config.value("default_blip", 50).toInt();
}

void Options::setBlipVolume(int value)
{
  m_config.setValue("default_blip", value);
}

int Options::defaultSuppressAudio() const
{
  return m_config.value("suppress_audio", 50).toInt();
}

void Options::setDefaultSupressedAudio(int value)
{
  m_config.setValue("suppress_audio", value);
}

bool Options::objectionSkipQueueEnabled() const
{
  return m_config.value("instant_objection", true).toBool();
}

void Options::setObjectionSkipQueueEnabled(bool value)
{
  m_config.setValue("instant_objection", value);
}

bool Options::desynchronisedLogsEnabled() const
{
  return m_config.value("desync_logs", false).toBool();
}

void Options::setDesynchronisedLogsEnabled(bool value)
{
  m_config.setValue("desync_logs", value);
}

bool Options::discordEnabled() const
{
  return m_config.value("discord", true).toBool();
}

void Options::setDiscordEnabled(bool value)
{
  m_config.setValue("discord", value);
}

bool Options::shakeEnabled() const
{
  return m_config.value("shake", true).toBool();
}

void Options::setShakeEnabled(bool value)
{
  m_config.setValue("shake", value);
}

bool Options::effectsEnabled() const
{
  return m_config.value("effects", true).toBool();
}

void Options::setEffectsEnabled(bool value)
{
  m_config.setValue("effects", value);
}

bool Options::networkedFrameSfxEnabled() const
{
  return m_config.value("framenetwork", true).toBool();
}

void Options::setNetworkedFrameSfxEnabled(bool value)
{
  m_config.setValue("framenetwork", value);
}

bool Options::slidesEnabled() const
{
  return m_config.value("slides", true).toBool();
}

void Options::setSlidesEnabled(bool value)
{
  m_config.setValue("slides", value);
}

bool Options::colorLogEnabled() const
{
  return m_config.value("colorlog", true).toBool();
}

void Options::setColorLogEnabled(bool value)
{
  m_config.setValue("colorlog", value);
}

bool Options::clearSoundsDropdownOnPlayEnabled() const
{
  return m_config.value("stickysounds", true).toBool();
}

void Options::setClearSoundsDropdownOnPlayEnabled(bool value)
{
  m_config.setValue("stickysounds", value);
}

bool Options::clearEffectsDropdownOnPlayEnabled() const
{
  return m_config.value("stickyeffects", true).toBool();
}

void Options::setClearEffectsDropdownOnPlayEnabled(bool value)
{
  m_config.setValue("stickyeffects", value);
}

bool Options::clearPreOnPlayEnabled() const
{
  return m_config.value("stickypres", true).toBool();
}

void Options::setClearPreOnPlayEnabled(bool value)
{
  m_config.setValue("stickypres", value);
}

bool Options::customChatboxEnabled() const
{
  return m_config.value("customchat", true).toBool();
}

void Options::setCustomChatboxEnabled(bool value)
{
  m_config.setValue("customchat", value);
}

bool Options::continuousPlaybackEnabled() const
{
  return m_config.value("continuous_playback", true).toBool();
}

void Options::setContinuousPlaybackEnabled(bool value)
{
  m_config.setValue("continuous_playback", value);
}

bool Options::stopMusicOnCategoryEnabled() const
{
  return m_config.value("category_stop", true).toBool();
}

void Options::setStopMusicOnCategoryEnabled(bool value)
{
  m_config.setValue("category_stop", value);
}

int Options::maxLogSize() const
{
  return m_config.value("log_maximum", 200).toInt();
}

void Options::setMaxLogSize(int value)
{
  m_config.setValue("log_maximum", value);
}

int Options::textStayTime() const
{
  return m_config.value("stay_time", 200).toInt();
}

void Options::setTextStayTime(int value)
{
  m_config.setValue("stay_time", value);
}

int Options::textCrawlSpeed() const
{
  return m_config.value("text_crawl", 40).toInt();
}

void Options::setTextCrawlSpeed(int value)
{
  m_config.setValue("text_crawl", value);
}

int Options::chatRateLimit() const
{
  return m_config.value("chat_ratelimit", 300).toInt();
}

void Options::setChatRateLimit(int value)
{
  m_config.setValue("chat_ratelimit", value);
}

bool Options::logDirectionDownwards() const
{
  return m_config.value("log_goes_downwards", true).toBool();
}

void Options::setLogDirectionDownwards(bool value)
{
  m_config.setValue("log_goes_downwards", value);
}

bool Options::logNewline() const
{
  return m_config.value("log_newline", false).toBool();
}

void Options::setLogNewline(bool value)
{
  m_config.setValue("log_newline", value);
}

int Options::logMargin() const
{
  return m_config.value("log_margin", 0).toInt();
}

void Options::setLogMargin(int value)
{
  m_config.setValue("log_margin", value);
}

bool Options::logTimestampEnabled() const
{
  return m_config.value("log_timestamp", false).toBool();
}

void Options::setLogTimestampEnabled(bool value)
{
  m_config.setValue("log_timestamp", value);
}

QString Options::logTimestampFormat() const
{
  return m_config.value("log_timestamp_format", "h:mm:ss AP").toString();
}

void Options::setLogTimestampFormat(QString value)
{
  m_config.setValue("log_timestamp_format", value);
}

bool Options::logIcActions() const
{
  return m_config.value("log_ic_actions", true).toBool();
}

void Options::setLogIcActions(bool value)
{
  m_config.setValue("log_ic_actions", value);
}

QString Options::username() const
{
  return m_config.value("default_username", "").value<QString>();
}

void Options::setUsername(QString value)
{
  m_config.setValue("default_username", value);
}

QString Options::audioOutputDevice() const
{
  return m_config.value("default_audio_device", "default").toString();
}

void Options::setAudioOutputDevice(QString value)
{
  m_config.setValue("default_audio_device", value);
}

bool Options::customShownameEnabled() const
{
  return m_config.value("show_custom_shownames", true).toBool();
}

void Options::setCustomShownameEnabled(bool value)
{
  m_config.setValue("show_custom_shownames", value);
}

QString Options::shownameOnJoin() const
{
  return m_config.value("default_showname", "").toString();
}

void Options::setShownameOnJoin(QString value)
{
  m_config.setValue("default_showname", value);
}

bool Options::logToTextFileEnabled() const
{
  return m_config.value("automatic_logging_enabled", true).toBool();
}

void Options::setLogToTextFileEnabled(bool value)
{
  m_config.setValue("automatic_logging_enabled", value);
}

bool Options::animatedThemeEnabled() const
{
  return m_config.value("animated_theme", false).toBool();
}

void Options::setAnimatedThemeEnabled(bool value)
{
  m_config.setValue("animated_theme", value);
}

QStringList Options::mountPaths() const
{
  return m_config.value("mount_paths").value<QStringList>();
}

void Options::setMountPaths(QStringList value)
{
  m_config.setValue("mount_paths", value);
  Q_EMIT mountListChanged();
}

bool Options::playerCountOptout() const
{
  return m_config.value("player_count_optout", false).toBool();
}

void Options::setPlayerCountOptout(bool value)
{
  m_config.setValue("player_count_optout", value);
}

bool Options::playSelectedSFXOnIdle() const
{
  return m_config.value("sfx_on_idle", false).toBool();
}

void Options::setPlaySelectedSFXOnIdle(bool value)
{
  m_config.setValue("sfx_on_idle", value);
}

bool Options::evidenceDoubleClickEdit() const
{
  return m_config.value("evidence_double_click", true).toBool();
}

void Options::setEvidenceDoubleClickEdit(bool value)
{
  m_config.setValue("evidence_double_click", value);
}

QString Options::alternativeMasterserver() const
{
  QString address = m_config.value("master").toString();
  if (address.isEmpty())
  {
    address = QStringLiteral("http://servers.aceattorneyonline.com");
  }
  return address;
}

void Options::setAlternativeMasterserver(QString value)
{
  m_config.setValue("master", value);
}

QString Options::language() const
{
  return m_config.value("language", QLocale::system().name()).toString();
}

void Options::setLanguage(QString value)
{
  m_config.setValue("language", value);
}

RESIZE_MODE Options::resizeMode() const
{
  return RESIZE_MODE(m_config.value("resize_mode", AUTO_RESIZE_MODE).toInt());
}

void Options::setResizeMode(RESIZE_MODE value)
{
  m_config.setValue("resize_mode", value);
}

QStringList Options::callwords() const
{
  QStringList l_callwords = m_config.value("callwords", QStringList{}).toStringList();

  // Please someone explain to me how tf I am supposed to create an empty
  // QStringList using QSetting defaults.
  if (l_callwords.size() == 1 && l_callwords.at(0).isEmpty())
  {
    l_callwords.clear();
  }
  return l_callwords;
}

void Options::setCallwords(QStringList value)
{
  m_config.setValue("callwords", value);
}

QString Options::playerlistFormatString() const
{
  return m_config.value("visuals/playerlist_format", "[{id}] {character} {character_name} {name}").toString();
}

void Options::setPlayerlistFormatString(QString value)
{
  m_config.setValue("visuals/playerlist_format", value);
}

void Options::clearConfig()
{
  m_config.clear();
}

QList<kal::ServerInfo> Options::favorites()
{
  QList<kal::ServerInfo> serverlist;

  auto grouplist = m_favorite_servers.childGroups();
  { // remove all negative and non-numbers
    auto filtered_grouplist = grouplist;
    for (const QString &group : qAsConst(grouplist))
    {
      bool ok = false;
      const int l_num = group.toInt(&ok);
      if (ok && l_num >= 0)
      {
        continue;
      }
      filtered_grouplist.append(group);
    }
    std::sort(filtered_grouplist.begin(), filtered_grouplist.end(), [](const auto &a, const auto &b) -> bool { return a.toInt() < b.toInt(); });
    grouplist = std::move(filtered_grouplist);
  }

  for (const QString &group : qAsConst(grouplist))
  {
    kal::ServerInfo f_server;
    m_favorite_servers.beginGroup(group);
    readServer(f_server);
    m_favorite_servers.endGroup();
    serverlist.append(f_server);
  }

  return serverlist;
}

void Options::setFavorites(QList<kal::ServerInfo> value)
{
  m_favorite_servers.clear();
  for (int i = 0; i < value.size(); ++i)
  {
    auto fav_server = value.at(i);
    m_favorite_servers.beginGroup(QString::number(i));
    writeServer(fav_server);
    m_favorite_servers.endGroup();
  }
  m_favorite_servers.sync();
}

void Options::removeFavorite(int index)
{
  QList<kal::ServerInfo> l_favorites = favorites();
  l_favorites.remove(index);
  setFavorites(l_favorites);
}

void Options::addFavorite(kal::ServerInfo server)
{
  updateFavorite(server, favorites().size());
}

void Options::updateFavorite(kal::ServerInfo server, int index)
{
  m_favorite_servers.beginGroup(QString::number(index));
  writeServer(server);
  m_favorite_servers.endGroup();
  m_favorite_servers.sync();
}

QString Options::getUIAsset(QString f_asset_name)
{
  QStringList l_paths{":/base/themes/" + theme() + "/" + f_asset_name};
  for (const QString &l_path : qAsConst(l_paths))
  {
    if (QFile::exists(l_path))
    {
      return l_path;
    }
  }
  kWarning() << "Unable to locate ui-asset" << f_asset_name << "in theme" << theme() << "Defaulting to embeeded asset.";
  return getInternalUIAsset(f_asset_name);
}

QString Options::getInternalUIAsset(QString f_asset_name)
{
  return QString(":/resource/ui/" + f_asset_name);
}

void Options::setWindowPosition(QString widget, QPoint position)
{
  m_config.setValue("windows/position_" + widget, position);
}

std::optional<QPoint> Options::windowPosition(QString widget)
{
  QPoint point = m_config.value("windows/position_" + widget, QPoint()).toPoint();
  if (point.isNull())
  {
    return std::nullopt;
  }
  return std::optional<QPoint>(point);
}

bool Options::restoreWindowPositionEnabled() const
{
  return m_config.value("windows/restore", true).toBool();
}

void Options::setRestoreWindowPositionEnabled(bool state)
{
  m_config.setValue("windows/restore", state);
}

void Options::migrate()
{
  if (m_config.contains("show_custom_shownames"))
  {
    m_config.remove("show_custom_shownames");
  }
  if (QFile::exists("base/callwords.ini"))
  {
    QFile file;
    file.setFileName("base/callwords.ini");
    if (!file.open(QIODevice::ReadOnly))
    {
      kWarning() << "Failed to open callwords.ini for migration -" << file.errorString();
      return;
    }

    QStringList l_callwords;
    QTextStream in(&file);
    while (!in.atEnd())
    {
      QString line = in.readLine();
      l_callwords.append(line);
    }
    file.close();
    file.remove();
  }
  if (m_config.contains("ooc_name"))
  {
    if (username().isEmpty())
    {
      m_config.setValue("default_username", m_config.value("ooc_name"));
    }
    m_config.remove("ooc_name");
  }

  if (m_config.contains("casing_enabled"))
  {
    m_config.remove("casing_enabled");
    m_config.remove("casing_defence_enabled");
    m_config.remove("casing_prosecution_enabled");
    m_config.remove("casing_judge_enabled");
    m_config.remove("casing_juror_enabled");
    m_config.remove("casing_steno_enabled");
    m_config.remove("casing_cm_enabled");
    m_config.remove("casing_can_host_cases");
  }
}

void Options::readServer(kal::ServerInfo &server)
{
  server.name = m_favorite_servers.value("name").toString();
  server.description = m_favorite_servers.value("desc").toString();
  server.address = m_favorite_servers.value("address").toString();
  server.port = m_favorite_servers.value("port").toInt();
  if (m_favorite_servers.contains("protocol"))
  {
    server.legacy = m_favorite_servers.value("protocol").toString() == QStringLiteral("tcp");
  }
  else if (m_favorite_servers.contains("legacy"))
  {
    server.legacy = m_favorite_servers.value("legacy").toBool();
  }
}

void Options::writeServer(const kal::ServerInfo &server)
{
  m_favorite_servers.setValue("address", server.address);
  m_favorite_servers.setValue("port", server.port);
  m_favorite_servers.setValue("legacy", server.legacy);
  if (server.name.isEmpty())
  {
    m_favorite_servers.remove("name");
  }
  else
  {
    m_favorite_servers.setValue("name", server.name);
  }
  if (server.description.isEmpty())
  {
    m_favorite_servers.remove("desc");
  }
  else
  {
    m_favorite_servers.setValue("desc", server.description);
  }
}
