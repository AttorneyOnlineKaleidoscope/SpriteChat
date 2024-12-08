#pragma once

#include <QObject>

class DiscordClient : public QObject
{
  Q_OBJECT

public:
  explicit DiscordClient(QObject *parent = nullptr);
  ~DiscordClient();

  void state_lobby();
  void state_server(std::string name, std::string server_id);
  void state_character(std::string name);
  void state_spectate();

private:
  std::string m_server_id;
  std::string m_server_name;
  int64_t m_timestamp;
};
