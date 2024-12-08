#include "aoapplication.h"

#include "courtroom.h"
#include "debug_functions.h"
#include "gui/guiutils.h"
#include "hardware_functions.h"
#include "lobby.h"
#include "network/packet/handshake_packets.h"
#include "networkmanager.h"
#include "options.h"
#include "spritechatversion.h"
#include "turnaboutversion.h"
#include "widget/optionswindow.h"

static QtMessageHandler original_message_handler;

AOApplication *AOApplication::self = nullptr;

AOApplication::AOApplication(Options &options, kal::AssetPathResolver &assetPathResolver, QObject *parent)
    : QObject(parent)
    , options(options)
    , m_resolver(assetPathResolver)
{
  Q_ASSERT_X(self == nullptr, Q_FUNC_INFO, "Only one instance is allowed");
  self = this;

  original_message_handler = qInstallMessageHandler(&AOApplication::handleMessage);

  m_discord = new DiscordClient(this);

  connect(&m_network, &kal::NetworkManager::statusChanged, this, &AOApplication::processNetworkStatus);
  connect(&m_network, &kal::NetworkManager::packetReceived, this, &AOApplication::processNetworkPacket);

  createLobby(true);
}

AOApplication::~AOApplication()
{
  disconnect(&m_network, &kal::NetworkManager::packetReceived, this, &AOApplication::processNetworkPacket);
  disconnect(&m_network, &kal::NetworkManager::statusChanged, this, &AOApplication::processNetworkStatus);

  destroyCourtroom();
  destroyLoadingWindow();
  destroyLobby();

  m_network.disconnectFromServer();

  qInstallMessageHandler(original_message_handler);

  self = nullptr;
}

void AOApplication::createLobby(bool visible)
{
  destroyLobby();
  w_lobby = new Lobby(options, *this, m_resolver, m_network);
  restoreWindowPosition(w_lobby);
  w_lobby->setVisible(visible);
}

void AOApplication::createLoadingWindow(bool visible)
{
  destroyLoadingWindow();
  w_loading = new LoadingWindow(m_network);
  kal::center_widget(w_loading);
  w_loading->setVisible(visible);
}

void AOApplication::createCourtroom(bool visible)
{
  destroyCourtroom();
  w_courtroom = new Courtroom(options, *this, m_resolver, m_session);
  restoreWindowPosition(w_courtroom);
  w_courtroom->setWindowTitle(m_network.server().nameOrDefault());

  connect(w_courtroom, &Courtroom::on_back_to_lobby_clicked, this, [this]() {
    w_courtroom->close();
    m_network.disconnectFromServer();
  });

  w_courtroom->setVisible(visible);
}

void AOApplication::destroyLobby()
{
  if (w_lobby)
  {
    w_lobby->deleteLater();
    w_lobby = nullptr;
  }
}

void AOApplication::destroyLoadingWindow()
{
  if (w_loading)
  {
    w_loading->deleteLater();
    w_loading = nullptr;
  }
}

void AOApplication::destroyCourtroom()
{
  if (w_courtroom)
  {
    w_courtroom->deleteLater();
    w_courtroom = nullptr;
  }
}

void AOApplication::shipPacket(const kal::Packet &packet)
{
  m_network.shipPacket(packet);
}

void AOApplication::call_settings_menu()
{
  OptionsWindow dialog(options, m_resolver);
  connect(&dialog, &OptionsWindow::reloadThemeRequest, w_courtroom, &Courtroom::on_reload_theme_clicked);
  dialog.exec();
  w_courtroom->playerList()->reloadPlayers();
}

QString AOApplication::find_image(QStringList p_list)
{
  QString image_path;
  for (const QString &path : p_list)
  {
    if (file_exists(path))
    {
      image_path = path;
      break;
    }
  }
  return image_path;
}

bool AOApplication::pointExistsOnScreen(const QPoint &point)
{
  for (QScreen *screen : QApplication::screens())
  {
    if (screen->availableGeometry().contains(point))
    {
      return true;
    }
  }
  return false;
}

void AOApplication::restoreWindowPosition(QWidget *window)
{
  if (options.restoreWindowPositionEnabled())
  {
    const auto maybe_pos = options.windowPosition(window->objectName());
    if (maybe_pos.has_value())
    {
      const auto pos = maybe_pos.value();
      if (pointExistsOnScreen(pos))
      {
        window->move(pos);
        return;
      }
    }
  }

  kal::center_widget(window);
}

// Callback for when BASS device is lost
// Only actually used for music syncs
void AOApplication::initBASS()
{
  BASS_SetConfig(BASS_CONFIG_DEV_DEFAULT, 1);
  BASS_Free();
  // Change the default audio output device to be the one the user has given
  // in his config.ini file for now.
  unsigned int a = 0;
  BASS_DEVICEINFO info;

  if (options.audioOutputDevice() == "default")
  {
    BASS_Init(-1, 48000, BASS_DEVICE_LATENCY, nullptr, nullptr);
    load_bass_plugins();
  }
  else
  {
    for (a = 0; BASS_GetDeviceInfo(a, &info); a++)
    {
      if (options.audioOutputDevice() == info.name)
      {
        BASS_SetDevice(a);
        BASS_Init(static_cast<int>(a), 48000, BASS_DEVICE_LATENCY, nullptr, nullptr);
        load_bass_plugins();
        kInfo() << info.name << "was set as the default audio output device.";
        return;
      }
    }
    BASS_Init(-1, 48000, BASS_DEVICE_LATENCY, nullptr, nullptr);
    load_bass_plugins();
  }
}

void AOApplication::load_bass_plugins()
{
  BASS_PluginLoad("bassopus.dll", 0);
}

void CALLBACK AOApplication::BASSreset(HSTREAM handle, DWORD channel, DWORD data, void *user)
{
  Q_UNUSED(handle);
  Q_UNUSED(channel);
  Q_UNUSED(data);
  Q_UNUSED(user);
  doBASSreset();
}

void AOApplication::doBASSreset()
{
  BASS_Free();
  BASS_Init(-1, 48000, BASS_DEVICE_LATENCY, nullptr, nullptr);
  load_bass_plugins();
}

void AOApplication::handleMessage(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
  Q_EMIT self->messageReceived(type, context, msg);
  original_message_handler(type, context, msg);
}

void AOApplication::beginHandshake()
{
  kal::HelloPacket packet;
  packet.hwid = get_hdid();
  packet.software_name = "AO2";
  packet.software_version = kal::SpriteChat::softwareVersion();
  packet.protocol_version = kal::Turnabout::softwareVersion();
  shipPacket(packet);
}

void AOApplication::completeHandshake()
{
  w_courtroom->show();
  destroyLoadingWindow();
}

void AOApplication::processNetworkStatus(kal::NetworkManager::Status status)
{
  switch (status)
  {
  case kal::NetworkManager::NotConnected:
    m_session = kal::NetworkSession();
    createLobby(true);
    if (w_courtroom && w_courtroom->isVisible())
    {
      w_courtroom->hide();
      call_notice(tr("Lost connection to server."));
    }
    destroyCourtroom();
    destroyLoadingWindow();
    if (options.discordEnabled())
    {
      m_discord->state_lobby();
    }
    break;

  case kal::NetworkManager::Connecting:
    createLoadingWindow(true);
    destroyLobby();
    break;

  case kal::NetworkManager::Connected:
    createCourtroom(false);
    { // authenticate
      kal::HelloPacket packet;
      packet.hwid = get_hdid();
      packet.software_name = "AO2";
      packet.software_version = kal::SpriteChat::softwareVersion();
      packet.protocol_version = kal::Turnabout::softwareVersion();
      shipPacket(packet);
    }
    break;
  }
}

#if (defined(_WIN32) || defined(_WIN64))
void AOApplication::processNetworkPacket()
{
  QQueue<kal::Packet> queue = m_network.takePacketQueue();
  while (!queue.isEmpty())
  {
    kal::Packet packet = queue.dequeue();
    packet.process(*this);
  }
}
#elif defined __APPLE__
void AOApplication::load_bass_plugins()
{
  BASS_PluginLoad("libbassopus.dylib", 0);
}
#elif (defined(LINUX) || defined(__linux__))
void AOApplication::load_bass_plugins()
{
  BASS_PluginLoad("libbassopus.so", 0);
}
#else
#error This operating system is unsupported for BASS plugins.
#endif
