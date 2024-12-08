#include "aoapplication.h"

#include "courtroom.h"
#include "debug_functions.h"
#include "widget/messagedialog.h"

#include "network/packet/area_packets.h"
#include "network/packet/background_packets.h"
#include "network/packet/character_packets.h"
#include "network/packet/chat_packets.h"
#include "network/packet/evidence_packets.h"
#include "network/packet/handshake_packets.h"
#include "network/packet/judge_packets.h"
#include "network/packet/login_packets.h"
#include "network/packet/moderation_packets.h"
#include "network/packet/music_packets.h"
#include "network/packet/splash_packets.h"
#include "network/packet/system_packets.h"
#include "network/packet/timer_packets.h"

void AOApplication::cargo(kal::HelloPacket &cargo)
{}

void AOApplication::cargo(kal::WelcomePacket &cargo)
{
  if (cargo.asset_url.isValid())
  {
    m_session.setAssetUrl(cargo.asset_url);
  }

  // TODO figure out why this is here
  this->log_filename = "";

  kal::ServerInfo server = m_network.server();
  QString address = QString("%1:%2").arg(server.address, QString::number(server.port));
  QCryptographicHash hash(QCryptographicHash::Algorithm::Sha256);
  hash.addData(address.toUtf8());
  if (options.discordEnabled())
  {
    m_discord->state_server(server.nameOrDefault().toStdString(), hash.result().toBase64().toStdString());
  }

  m_session.setPlayerId(cargo.player_id);
  w_courtroom->show();

  completeHandshake();
}

void AOApplication::cargo(kal::AreaListPacket &cargo)
{
  w_courtroom->setAreaList(cargo.area_list);
}

void AOApplication::cargo(kal::UpdateAreaPacket &cargo)
{
  // TODO implement area updates
}

void AOApplication::cargo(kal::JoinAreaPacket &cargo)
{}

void AOApplication::cargo(kal::BackgroundPacket &cargo)
{
  if (cargo.position)
  {
    w_courtroom->set_side(cargo.position.value());
  }
  w_courtroom->set_background(cargo.background);
}

void AOApplication::cargo(kal::PositionPacket &cargo)
{
  w_courtroom->set_side(cargo.position);
}

void AOApplication::cargo(kal::CasePacket_CASEA &cargo)
{}

void AOApplication::cargo(kal::CharacterListPacket &cargo)
{
  w_courtroom->setCharacterList(cargo.character_list);
}

void AOApplication::cargo(kal::SelectCharacterPacket &cargo)
{
  w_courtroom->update_character(cargo.character_id);
}

void AOApplication::cargo(kal::CharacterSelectStatePacket &cargo)
{
  for (int i = 0; i < cargo.available.length(); ++i)
  {
    w_courtroom->set_taken(i, cargo.available.at(i));
  }
}

void AOApplication::cargo(kal::ChatPacket &cargo)
{
  w_courtroom->append_server_chatmessage(cargo.name, cargo.message, cargo.from_server);
}

void AOApplication::cargo(kal::EvidenceListPacket &cargo)
{
  w_courtroom->set_evidence_list(cargo.evidence_list);
}

void AOApplication::cargo(kal::CreateEvidencePacket &cargo)
{}

void AOApplication::cargo(kal::EditEvidencePacket &cargo)
{}

void AOApplication::cargo(kal::DeleteEvidencePacket &cargo)
{}

void AOApplication::cargo(kal::JudgeStatePacket &cargo)
{
  w_courtroom->set_judge_state(cargo.mode);
  if (w_courtroom->get_judge_state() != kal::OptionalJudgeMode)
  {
    w_courtroom->show_judge_controls(w_courtroom->get_judge_state() == kal::ActiveJudgeMode);
  }
  else
  {
    w_courtroom->set_judge_buttons(); // client-side judge behavior
  }
}

void AOApplication::cargo(kal::HealthStatePacket &cargo)
{
  w_courtroom->set_hp_bar(cargo.type, cargo.health);
}

void AOApplication::cargo(kal::SplashPacket &cargo)
{
  w_courtroom->handle_wtce(cargo.splash_id);
}

void AOApplication::cargo(kal::LoginPacket &cargo)
{
  w_courtroom->on_authentication_state_received(cargo.result);
}

void AOApplication::cargo(kal::MessagePacket &cargo)
{
  // TODO implement message packet logic
}

void AOApplication::cargo(kal::ModerationAction &cargo)
{}

void AOApplication::cargo(kal::ModerationNotice &cargo)
{
  QString markdown;
  if (cargo.duration > 0)
  {
    markdown.append(tr("You have been banned from the server."));
  }
  else
  {
    markdown.append(tr("You have been kicked from the server."));
  }

  markdown.append(tr("\n\nReason: "));
  if (cargo.reason.isEmpty())
  {
    markdown.append(tr("No reason given."));
  }
  else
  {
    markdown.append(cargo.reason);
  }

  if (cargo.duration > 0)
  {
    QDateTime timestamp = QDateTime::currentDateTime().addSecs(cargo.duration);
    markdown.append(tr("\nUntil: %1").arg(timestamp.toString("MM/dd/yyyy, hh:mm")));
  }

  m_network.disconnectFromServer();
  kal::MessageDialog::information(markdown, QString(), kal::MessageDialog::Markdown);
}

void AOApplication::cargo(kal::MusicListPacket &cargo)
{
  w_courtroom->setMusicList(cargo.music_list);
  createAndShipPacket<kal::ChatPacket>(options.username(), QString());
}

void AOApplication::cargo(kal::PlayTrackPacket &cargo)
{
  w_courtroom->handle_song(cargo.track, cargo.character_id, cargo.character_name, cargo.channel_id, cargo.loop, cargo.effects);
}

void AOApplication::cargo(kal::RegisterPlayerPacket &cargo)
{
  w_courtroom->playerList()->registerPlayer(cargo);
}

void AOApplication::cargo(kal::UpdatePlayerPacket &cargo)
{
  w_courtroom->playerList()->updatePlayer(cargo);
}

void AOApplication::cargo(kal::HelpMePacket &cargo)
{
  w_courtroom->mod_called(cargo.message);
}

void AOApplication::cargo(kal::NoticePacket &cargo)
{
  if (cargo.message.isEmpty())
  {
    return;
  }

  call_notice(cargo.message);
}

void AOApplication::cargo(kal::SetTimerStatePacket &cargo)
{
  int id = cargo.timer_id;
  qint64 timer_value = cargo.time;
  switch (cargo.command)
  {
  default:
    return;

  case kal::StartTimer:
  case kal::StopTimer:
    if (timer_value > 0)
    {
      if (cargo.command == kal::StartTimer)
      {
        timer_value -= m_network.latency() / 2;
        w_courtroom->start_clock(id, timer_value);
      }
      else
      {
        w_courtroom->pause_clock(id);
        w_courtroom->set_clock(id, timer_value);
      }
    }
    else
    {
      w_courtroom->stop_clock(id);
    }
    break;

  case kal::ShowTimer:
    w_courtroom->set_clock_visibility(id, true);
    break;

  case kal::HideTimer:
    w_courtroom->set_clock_visibility(id, false);
    break;
  }
}
