#pragma once

#include "networkmanager.h"
#include "options.h"

#include <QLabel>
#include <QMainWindow>
#include <QProgressBar>
#include <QPushButton>
#include <QTimer>

class LoadingWindow : public QMainWindow
{
  Q_OBJECT

public:
  explicit LoadingWindow(kal::NetworkManager &network, QWidget *parent = nullptr);
  ~LoadingWindow();

private:
  kal::NetworkManager &m_network;
  QTimer m_timer;
  bool m_connecting = true;
  int m_counter = 0;

  QLabel *ui_status;
  QPushButton *ui_cancel;

private Q_SLOTS:
  void updateStatus();
  void cancel();
};
