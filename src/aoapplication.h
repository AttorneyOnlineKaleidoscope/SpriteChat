#pragma once

#include "assetpathresolver.h"
#include "datatypes.h"
#include "discord_rich_presence.h"
#include "gameglobal.h"
#include "network/cargohandler.h"
#include "network/packet.h"
#include "network/packettransport.h"
#include "networkmanager.h"
#include "networksession.h"
#include "widget/loadingwindow.h"
#include "widget/optionswindow.h"

#include <bass.h>

#include <QColor>
#include <QCryptographicHash>
#include <QDebug>
#include <QDir>
#include <QElapsedTimer>
#include <QFile>
#include <QList>
#include <QObject>
#include <QRect>
#include <QScreen>
#include <QSettings>
#include <QStandardPaths>
#include <QStringList>
#include <QTextStream>
#include <QTime>
#include <QVersionNumber>

class Lobby;
class Courtroom;
class Options;

class AOApplication : public QObject, public kal::PacketTransport, private kal::CargoHandler
{
  Q_OBJECT

public:
  static void load_bass_plugins();
  static void CALLBACK BASSreset(HSTREAM handle, DWORD channel, DWORD data, void *user);
  static void doBASSreset();

  explicit AOApplication(Options &options, kal::AssetPathResolver &assetPathResolver, QObject *parent = nullptr);
  ~AOApplication();

  DiscordClient *m_discord = nullptr;

  QFont default_font;

  void shipPacket(const kal::Packet &packet) override;

  void call_settings_menu();

  QString get_config_value(const QString &fileName, const QString &identifier);

  BackgroundPosition get_pos_path(const QString &background, const QString &position);

  ////// Functions for reading and writing files //////
  // Implementations file_functions.cpp

  // returns all of the file's lines in a QStringList
  QStringList get_list_file(QString p_file);

  // Process a file and return its text as a QString
  QString read_file(QString filename);

  // Write text to file. make_dir would auto-create the directory if it doesn't
  // exist.
  bool write_to_file(QString p_text, QString p_file, bool make_dir = false);

  // Append text to the end of the file. make_dir would auto-create the
  // directory if it doesn't exist.
  bool append_to_file(QString p_text, QString p_file, bool make_dir = false);

  // Returns the value of p_identifier in the design.ini file in p_design_path
  QString read_design_ini(QString p_identifier, QString p_design_path);

  // Returns the coordinates of widget with p_identifier from p_file
  QPoint get_button_spacing(QString p_identifier, QString p_file);

  // Returns the dimensions of widget with specified identifier from p_file
  pos_size_type get_element_dimensions(QString p_identifier, QString p_file, QString p_misc = QString());

  // Returns the value to you
  QString get_design_element(QString p_identifier, QString p_file, QString p_misc = QString());

  // Returns the color with p_identifier from p_file
  QColor get_color(const QString &identifier, const QString &fileName, const QColor &defaultColor = Qt::black);

  // Returns the markup symbol used for specified p_identifier such as colors
  QString get_chat_markup(QString p_identifier, QString p_file);

  // Returns the color from the misc folder.
  QColor get_chat_color(QString p_identifier, QString p_chat);

  // Returns the value with p_identifier from penalty/penalty.ini in the current
  // theme path
  QString get_penalty_value(QString p_identifier);

  // Returns the sfx with p_identifier from courtroom_sounds.ini in the current theme path
  QString get_court_sfx(QString p_identifier, QString p_misc = QString());

  QList<SfxItem> get_sfx_list(QString file_name);

  // Returns the value of p_search_line within target_tag and terminator_tag
  QString read_char_ini(QString p_char, QString p_search_line, QString target_tag);

  // Returns a QStringList of all key=value definitions on a given tag.
  QStringList read_ini_tags(QString p_file, QString target_tag = QString());

  // Returns the text between target_tag and terminator_tag in p_file
  QString get_stylesheet(QString p_file);

  // Returns the side of the p_char character from that characters ini file
  QString get_char_side(QString p_char);

  // Returns the showname from the ini of p_char
  QString get_showname(QString p_char, int p_emote = -1);

  // Returns the category of this character
  QString get_category(QString p_char);

  // Returns the value of chat image from the specific p_char's ini file
  QString get_chat(QString p_char);

  // Returns the value of chat font from the specific p_char's ini file
  QString get_chat_font(QString p_char);

  // Returns the value of chat font size from the specific p_char's ini file
  int get_chat_size(QString p_char);

  // Returns the preanim duration of p_char's p_emote
  int get_preanim_duration(QString p_char, QString p_emote);

  // Not in use
  int get_text_delay(QString p_char, QString p_emote);

  // Returns whether the given pos is a judge position
  bool get_pos_is_judge(const QString &background, const QString &p_pos);

  /**
   * @brief Returns the duration of the transition animation between the two
   * given positions, if it exists
   */
  int get_pos_transition_duration(const QString &background, const QString &old_pos, const QString &new_pos);

  // Returns the total amount of emotes of p_char
  int get_emote_number(QString p_char);

  // Returns the emote comment of p_char's p_emote
  QString get_emote_comment(QString p_char, int p_emote);

  // Returns the base name of p_char's p_emote
  QString get_emote(QString p_char, int p_emote);

  // Returns the preanimation name of p_char's p_emote
  QString get_pre_emote(QString p_char, int p_emote);

  // Returns the sfx of p_char's p_emote
  QString get_sfx_name(QString p_char, int p_emote);

  // Returns if the sfx is defined as looping in char.ini
  QString get_sfx_looping(QString p_char, int p_emote);

  // Returns if an emote has a frame specific SFX for it
  QString get_sfx_frame(QString p_char, QString p_emote, int n_frame);

  // Returns if an emote has a frame specific SFX for it
  QString get_flash_frame(QString p_char, QString p_emote, int n_frame);

  // Returns if an emote has a frame specific SFX for it
  QString get_screenshake_frame(QString p_char, QString p_emote, int n_frame);

  // Not in use
  int get_sfx_delay(QString p_char, int p_emote);

  // Returns the modifier for p_char's p_emote
  int get_emote_mod(QString p_char, int p_emote);

  // Returns the desk modifier for p_char's p_emote
  int get_desk_mod(QString p_char, int p_emote);

  // Returns p_char's blipname by reading char.ini for blips (previously called "gender")
  QString get_blipname(QString p_char, int p_emote = -1);

  // Get a property of a given emote, or get it from "options" if emote doesn't have it
  QString get_emote_property(QString p_char, QString p_emote, QString p_property);

  // Return a transformation mode from a string ("smooth" for smooth, anything else for fast)
  RESIZE_MODE get_scaling(QString p_scaling);

  // Returns the scaling type for p_miscname
  RESIZE_MODE get_chatbox_scaling(const QString &chatbox);

  // ======
  // These are all casing-related settings.
  // ======

  // The file name of the log file in base/logs.
  QString log_filename;

  bool pointExistsOnScreen(const QPoint &point);
  void restoreWindowPosition(QWidget *window);

  void initBASS();

private:
  static AOApplication *self;

  Options &options;
  kal::AssetPathResolver &m_resolver;

  kal::NetworkManager m_network;
  kal::NetworkSession m_session;

  Lobby *w_lobby = nullptr;
  LoadingWindow *w_loading = nullptr;
  Courtroom *w_courtroom = nullptr;

  kal::PlayerId m_player_id = 0;

  static void handleMessage(QtMsgType type, const QMessageLogContext &context, const QString &msg);

  void createLobby(bool visible);
  void createLoadingWindow(bool visible);
  void createCourtroom(bool visible);
  void destroyLobby();
  void destroyLoadingWindow();
  void destroyCourtroom();

  void beginHandshake();
  void completeHandshake();

  void cargo(kal::HelloPacket &cargo) override;
  void cargo(kal::WelcomePacket &cargo) override;
  void cargo(kal::AreaListPacket &cargo) override;
  void cargo(kal::UpdateAreaPacket &cargo) override;
  void cargo(kal::JoinAreaPacket &cargo) override;
  void cargo(kal::BackgroundPacket &cargo) override;
  void cargo(kal::PositionPacket &cargo) override;
  void cargo(kal::CasePacket_CASEA &cargo) override;
  void cargo(kal::CharacterListPacket &cargo) override;
  void cargo(kal::SelectCharacterPacket &cargo) override;
  void cargo(kal::CharacterSelectStatePacket &cargo) override;
  void cargo(kal::ChatPacket &cargo) override;
  void cargo(kal::EvidenceListPacket &cargo) override;
  void cargo(kal::CreateEvidencePacket &cargo) override;
  void cargo(kal::EditEvidencePacket &cargo) override;
  void cargo(kal::DeleteEvidencePacket &cargo) override;
  void cargo(kal::JudgeStatePacket &cargo) override;
  void cargo(kal::HealthStatePacket &cargo) override;
  void cargo(kal::SplashPacket &cargo) override;
  void cargo(kal::LoginPacket &cargo) override;
  void cargo(kal::MessagePacket &cargo) override;
  void cargo(kal::ModerationAction &cargo) override;
  void cargo(kal::ModerationNotice &cargo) override;
  void cargo(kal::MusicListPacket &cargo) override;
  void cargo(kal::PlayTrackPacket &cargo) override;
  void cargo(kal::RegisterPlayerPacket &cargo) override;
  void cargo(kal::UpdatePlayerPacket &cargo) override;
  void cargo(kal::HelpMePacket &cargo) override;
  void cargo(kal::NoticePacket &cargo) override;
  void cargo(kal::SetTimerStatePacket &cargo) override;

Q_SIGNALS:
  void messageReceived(QtMsgType type, const QMessageLogContext &context, const QString &msg);

private Q_SLOTS:
  void processNetworkStatus(kal::NetworkManager::Status status);
  void processNetworkPacket();
};
