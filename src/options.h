#pragma once

#include "datatypes.h"
#include "network/serverinfo.h"

#include <QObject>
#include <QPoint>
#include <QSettings>

class Options : public QObject
{
  Q_OBJECT

public:
  explicit Options(QObject *parent = nullptr);
  virtual ~Options();

  // Reads the theme from config.ini and loads it into the currenttheme
  // variable
  QString theme() const;
  void setTheme(QString value);

  int themeScalingFactor() const;
  void setThemeScalingFactor(int value);

  // Returns the blip rate from config.ini (once per X symbols)
  int blipRate() const;
  void setBlipRate(int value);

  // Returns true if blank blips is enabled in config.ini and false otherwise
  bool blankBlip() const;
  void setBlankBlip(bool value);

  // Returns true if looping sound effects are enabled in the config.ini
  bool loopingSfx() const;
  void setLoopingSfx(bool value);

  // Returns true if stop music on objection is enabled in the config.ini
  bool objectionStopMusic() const;
  void setObjectionStopMusic(bool value);

  // Returns true if streaming is enabled in the config.ini
  bool streamingEnabled() const;
  void setStreamingEnabled(bool value);

  // Returns the value of defaultmusic in config.ini
  int musicVolume() const;
  void setMusicVolume(int value);

  // Returns the value of defaultsfx in config.ini
  int sfxVolume() const;
  void setSfxVolume(int value);

  // Returns the value of defaultblip in config.ini
  int blipVolume() const;
  void setBlipVolume(int value);

  // Returns the value of suppressaudio in config.ini
  int defaultSuppressAudio() const;
  void setDefaultSupressedAudio(int value);

  // Returns the value if objections interrupt and skip the message queue
  // from the config.ini.
  bool objectionSkipQueueEnabled() const;
  void setObjectionSkipQueueEnabled(bool value);

  // returns if log will show messages as-received, while viewport will parse
  // according to the queue (Text Stay Time) from the config.ini
  bool desynchronisedLogsEnabled() const;
  void setDesynchronisedLogsEnabled(bool value);

  // Returns the value of whether Discord should be enabled on startup
  // from the config.ini.
  bool discordEnabled() const;
  void setDiscordEnabled(bool value);

  // Returns the value of whether shaking should be enabled.
  // from the config.ini.
  bool shakeEnabled() const;
  void setShakeEnabled(bool value);

  // Returns the value of whether effects should be Enabled.
  // from the config.ini.
  bool effectsEnabled() const;
  void setEffectsEnabled(bool value);

  // Returns the value of whether frame-specific effects defined in char.ini
  // should be sent/received over the network. from the config.ini.
  bool networkedFrameSfxEnabled() const;
  void setNetworkedFrameSfxEnabled(bool value);

  // Returns the value of whether courtroom slide animations should be played
  // on this client.
  bool slidesEnabled() const;
  void setSlidesEnabled(bool value);

  // Returns the value of whether colored ic log should be a thing.
  // from the config.ini.
  bool colorLogEnabled() const;
  void setColorLogEnabled(bool value);

  // Returns the value of whether sticky sounds should be a thing.
  // from the config.ini.
  bool clearSoundsDropdownOnPlayEnabled() const;
  void setClearSoundsDropdownOnPlayEnabled(bool value);

  // Returns the value of whether sticky effects should be a thing.
  // from the config.ini.
  bool clearEffectsDropdownOnPlayEnabled() const;
  void setClearEffectsDropdownOnPlayEnabled(bool value);

  // Returns the value of whether sticky preanims should be a thing.
  // from the config.ini.
  bool clearPreOnPlayEnabled() const;
  void setClearPreOnPlayEnabled(bool value);

  // Returns the value of whether custom chatboxes should be a thing.
  // from the config.ini.
  // I am increasingly maddened by the lack of dynamic auto-generation system
  // for settings.
  bool customChatboxEnabled() const;
  void setCustomChatboxEnabled(bool value);

  // Returns the value of whether continuous playback should be used
  // from the config.ini.
  bool continuousPlaybackEnabled() const;
  void setContinuousPlaybackEnabled(bool value);

  // Returns the value of whether stopping music by double clicking category
  // should be used from the config.ini.
  bool stopMusicOnCategoryEnabled() const;
  void setStopMusicOnCategoryEnabled(bool value);

  // Returns the value of the maximum amount of lines the IC chatlog
  // may contain, from config.ini.
  int maxLogSize() const;
  void setMaxLogSize(int value);

  // Current wait time between messages for the queue system
  int textStayTime() const;
  void setTextStayTime(int value);

  // Returns the letter display speed during text crawl in in-character messages
  int textCrawlSpeed() const;
  void setTextCrawlSpeed(int value);

  // Returns Minimum amount of time (in miliseconds) that must pass before the
  // next Enter key press will send your IC message. (new behaviour)
  int chatRateLimit() const;
  void setChatRateLimit(int value);

  // Returns whether the log should go upwards (new behaviour)
  // or downwards (vanilla behaviour).
  bool logDirectionDownwards() const;
  void setLogDirectionDownwards(bool value);

  // Returns whether the log should separate name from text via newline or :
  bool logNewline() const;
  void setLogNewline(bool value);

  // Get spacing between IC log entries.
  int logMargin() const;
  void setLogMargin(int value);

  // Returns whether the log should have a timestamp.
  bool logTimestampEnabled() const;
  void setLogTimestampEnabled(bool value);

  // Returns the format string for the log timestamp
  QString logTimestampFormat() const;
  void setLogTimestampFormat(QString value);

  // Returns whether to log IC actions.
  bool logIcActions() const;
  void setLogIcActions(bool value);

  // Returns the username the user may have set in config.ini.
  QString username() const;
  void setUsername(QString value);

  // Returns the audio device used for the client.
  QString audioOutputDevice() const;
  void setAudioOutputDevice(QString value);

  // Returns whether the user would like to have custom shownames on by default.
  bool customShownameEnabled() const;
  void setCustomShownameEnabled(bool value);

  // Returns the showname the user may have set in config.ini.
  QString shownameOnJoin() const;
  void setShownameOnJoin(QString value);

  // Get if text file logging is Enabled
  bool logToTextFileEnabled() const;
  void setLogToTextFileEnabled(bool value);

  // Get if the theme is animated
  bool animatedThemeEnabled() const;
  void setAnimatedThemeEnabled(bool value);

  // Get a list of custom mount paths
  QStringList mountPaths() const;
  void setMountPaths(QStringList value);

  // Get whether to opt out of player count metrics sent to the master server
  bool playerCountOptout() const;
  void setPlayerCountOptout(bool value);

  // Get if sfx can be sent to play on idle
  bool playSelectedSFXOnIdle() const;
  void setPlaySelectedSFXOnIdle(bool value);

  // Whether opening evidence requires a single or double click
  bool evidenceDoubleClickEdit() const;
  void setEvidenceDoubleClickEdit(bool value);

  // Supplies an alternative masterserver URL
  QString alternativeMasterserver() const;
  void setAlternativeMasterserver(QString value);

  // Language the client loads on start.
  QString language() const;
  void setLanguage(QString value);

  // The scaling algorithm to use on images.
  RESIZE_MODE resizeMode() const;
  void setResizeMode(RESIZE_MODE value);

  // Callwords notify the user when the word/words are used in a game message.
  QStringList callwords() const;
  void setCallwords(QStringList value);

  QString playerlistFormatString() const;
  void setPlayerlistFormatString(QString value);

  // Clears the configuration file. Essentially restoring it to default.
  void clearConfig();

  // Loads the favorite servers
  QList<kal::ServerInfo> favorites();
  void setFavorites(QList<kal::ServerInfo> value);

  // Interactions with favorite servers
  void removeFavorite(int index);
  void addFavorite(kal::ServerInfo server);
  void updateFavorite(kal::ServerInfo server, int index);

  // Theming Nonesense!
  QString getUIAsset(QString f_asset_name);
  QString getInternalUIAsset(QString f_asset_name);

  void setWindowPosition(QString widget, QPoint position);
  std::optional<QPoint> windowPosition(QString widget);

  bool restoreWindowPositionEnabled() const;
  void setRestoreWindowPositionEnabled(bool state);

Q_SIGNALS:
  void mountListChanged();
  void playerCountOptoutToggled(bool);

private:
  static Options *self;

  QSettings m_config;
  QSettings m_favorite_servers;

  void migrate();

  void readServer(kal::ServerInfo &server);
  void writeServer(const kal::ServerInfo &server);
};
