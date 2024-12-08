#pragma once

#include "network/packettransport.h"

#include <QCheckBox>
#include <QComboBox>
#include <QDialogButtonBox>
#include <QLabel>
#include <QSpinBox>
#include <QTextEdit>
#include <QWidget>

class ModeratorDialog : public QWidget
{
  Q_OBJECT

public:
  explicit ModeratorDialog(int clientId, bool ban, kal::PacketTransport &transport, QWidget *parent = nullptr);
  virtual ~ModeratorDialog();

private:
  kal::PacketTransport &m_transport;
  int m_client_id;
  bool m_ban;

  QWidget *ui_widget;
  QComboBox *ui_action;
  QSpinBox *ui_duration_mm;
  QSpinBox *ui_duration_hh;
  QSpinBox *ui_duration_dd;
  QLabel *ui_duration_label;
  QCheckBox *ui_permanent;
  QTextEdit *ui_details;
  QDialogButtonBox *ui_button_box;

private Q_SLOTS:
  void onAcceptedClicked();
};
