#pragma once

#include "networkmanager.h"

#include <QDialog>
#include <QLineEdit>
#include <QPushButton>

class DirectConnectDialog : public QDialog
{
  Q_OBJECT

public:
  explicit DirectConnectDialog(kal::NetworkManager &network, QWidget *parent = nullptr);

private:
  kal::NetworkManager &m_network;

  QLineEdit *ui_address;
  QPushButton *ui_connect;
  QPushButton *ui_cancel;

private Q_SLOTS:
  void parseAndConnectToServer();
};
