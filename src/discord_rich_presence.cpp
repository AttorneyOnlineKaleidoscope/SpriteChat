#include "discord_rich_presence.h"

#include <QCoreApplication>
#include <QDebug>

#ifdef KAL_SPRITECHAT_ENABLE_DISCORD_RPC
#include <discord_rpc.h>
#endif

#include <ctime>

constinit const char *APPLICATION_ID = "399779271737868288";

#if defined(KAL_SPRITECHAT_ENABLE_DISCORD_RPC) && !defined(ANDROID)
DiscordClient::DiscordClient(QObject *parent)
    : QObject(parent)
{
  DiscordEventHandlers handlers;
  std::memset(&handlers, 0, sizeof(handlers));
  handlers = {};
  handlers.ready = [](const DiscordUser *user) {
    kInfo() << "DiscordClient ready";
  };
  handlers.disconnected = [](int errorCode, const char *message) {
    kInfo() << "DiscordClient disconnected! " << message << errorCode;
  };
  handlers.errored = [](int errorCode, const char *message) {
    kWarning() << "DiscordClient errored out! " << message << errorCode;
  };
  kInfo() << "Initializing DiscordClient";
  Discord_Initialize(APPLICATION_ID, &handlers, 1, nullptr);
}

DiscordClient::~DiscordClient()
{
  Discord_Shutdown();
}

void DiscordClient::state_lobby()
{
  DiscordRichPresence presence;
  std::memset(&presence, 0, sizeof(presence));
  presence.largeImageKey = "ao2-logo";
  presence.largeImageText = "Objection!";
  presence.instance = 1;

  presence.state = "In Lobby";
  presence.details = "Idle";
  Discord_UpdatePresence(&presence);
}

void DiscordClient::state_server(std::string name, std::string server_id)
{
  kDebug() << "DiscordClient: Setting server state";

  DiscordRichPresence presence;
  std::memset(&presence, 0, sizeof(presence));
  presence.largeImageKey = "ao2-logo";
  presence.largeImageText = "Objection!";
  presence.instance = 1;

  auto timestamp = static_cast<int64_t>(std::time(nullptr));

  presence.state = "In a Server";
  presence.details = name.c_str();
  presence.matchSecret = server_id.c_str();
  presence.startTimestamp = this->m_timestamp;

  this->m_server_id = server_id;
  this->m_server_name = name;
  this->m_timestamp = timestamp;
  Discord_UpdatePresence(&presence);
}

void DiscordClient::state_character(std::string name)
{
  auto name_internal = QString(name.c_str()).toLower().replace(' ', '_').toStdString();
  auto name_friendly = QString(name.c_str()).replace('_', ' ').toStdString();
  const std::string playing_as = "Playing as " + name_friendly;
  kDebug() << "DiscordClient: Setting character state (\"" << playing_as.c_str() << "\")";

  DiscordRichPresence presence;
  std::memset(&presence, 0, sizeof(presence));
  presence.largeImageKey = "ao2-logo";
  presence.largeImageText = "Objection!";
  presence.instance = 1;
  presence.details = this->m_server_name.c_str();
  presence.matchSecret = this->m_server_id.c_str();
  presence.startTimestamp = this->m_timestamp;

  presence.state = playing_as.c_str();
  presence.smallImageKey = name_internal.c_str();
  // presence.smallImageText = name_internal.c_str();
  Discord_UpdatePresence(&presence);
}

void DiscordClient::state_spectate()
{
  kDebug() << "DiscordClient: Setting specator state";

  DiscordRichPresence presence;
  std::memset(&presence, 0, sizeof(presence));
  presence.largeImageKey = "ao2-logo";
  presence.largeImageText = "Objection!";
  presence.instance = 1;
  presence.details = this->m_server_name.c_str();
  presence.matchSecret = this->m_server_id.c_str();
  presence.startTimestamp = this->m_timestamp;

  presence.state = "Spectating";
  Discord_UpdatePresence(&presence);
}
#else
DiscordClient::DiscordClient()
{}

DiscordClient::~DiscordClient()
{}

void DiscordClient::state_lobby()
{}

void DiscordClient::state_server(std::string name, std::string server_id)
{
  Q_UNUSED(name);
  Q_UNUSED(server_id);
  kDebug() << "DiscordClient: Setting server state";
}

void DiscordClient::state_character(std::string name)
{
  Q_UNUSED(name);
  kDebug() << "DiscordClient: Setting character state";
}

void DiscordClient::state_spectate()
{
  kDebug() << "DiscordClient: Setting specator state";
}
#endif
