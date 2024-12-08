#include "moderatordialog.h"

#include "aoapplication.h"
#include "gui/guiloader.h"
#include "network/packet/moderation_packets.h"
#include "options.h"

#include <QDebug>
#include <QFile>
#include <QMessageBox>
#include <QUiLoader>
#include <QVBoxLayout>
#include <chrono>

ModeratorDialog::ModeratorDialog(int clientId, bool ban, kal::PacketTransport &transport, QWidget *parent)
    : QWidget{parent}
    , m_transport(transport)
    , m_client_id(clientId)
    , m_ban(ban)
{
  kal::GuiLoader loader(*this);
  loader.load(":/resource/ui/moderator_action_dialog.ui");

  ui_widget = this;
  auto layout = new QVBoxLayout(this);
  layout->addWidget(ui_widget);

  loader.assign(ui_action, "action");
  loader.assign(ui_duration_mm, "duration_mm");
  loader.assign(ui_duration_hh, "duration_hh");
  loader.assign(ui_duration_dd, "duration_dd");
  loader.assign(ui_duration_label, "duration_label");
  loader.assign(ui_permanent, "permanent");
  loader.assign(ui_details, "details");
  loader.assign(ui_button_box, "button_box");

  if (m_ban)
  {
    ui_action->addItem(tr("Ban"));
  }
  else
  {
    ui_action->addItem(tr("Kick"));
  }

  ui_duration_mm->setVisible(m_ban);
  ui_duration_hh->setVisible(m_ban);
  ui_duration_dd->setVisible(m_ban);
  ui_duration_label->setVisible(m_ban);
  ui_permanent->setVisible(m_ban);

  connect(ui_button_box, &QDialogButtonBox::accepted, this, &ModeratorDialog::onAcceptedClicked);
  connect(ui_button_box, &QDialogButtonBox::rejected, this, &ModeratorDialog::close);
}

ModeratorDialog::~ModeratorDialog()
{}

void ModeratorDialog::onAcceptedClicked()
{
  QString reason = ui_details->toPlainText();
  if (reason.isEmpty())
  {
    if (QMessageBox::question(this, tr("Confirmation"), tr("Are you sure you want to confirm without a reason?"), QMessageBox::Yes | QMessageBox::No) == QMessageBox::No)
    {
      return;
    }
  }

  bool permanent = ui_permanent->isChecked();
  if (permanent)
  {
    if (QMessageBox::question(this, tr("Confirmation"), tr("Are you sure you want to ban permanently?"), QMessageBox::Yes | QMessageBox::No) == QMessageBox::No)
    {
      return;
    }
  }

  kal::ModerationAction packet;
  packet.player_id = m_client_id;
  if (m_ban)
  {
    if (permanent)
    {
      packet.duration = kal::PermanentModerationActionDuration;
    }
    else
    {
      qint64 duration = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::days(ui_duration_dd->value())).count();
      duration = duration + std::chrono::duration_cast<std::chrono::seconds>(std::chrono::hours(ui_duration_hh->value())).count();
      duration = duration + std::chrono::duration_cast<std::chrono::seconds>(std::chrono::minutes(ui_duration_mm->value())).count();
      packet.duration = duration;
    }
  }
  else
  {
    packet.duration = kal::KickModerationDuration;
  }
  packet.reason = reason;

  m_transport.shipPacket(packet);

  close();
}
