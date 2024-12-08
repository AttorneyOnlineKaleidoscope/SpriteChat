#include "loadingwindow.h"

#include "gui/guiloader.h"

LoadingWindow::LoadingWindow(kal::NetworkManager &network, QWidget *parent)
    : QMainWindow(parent)
    , m_network(network)
{
  setWindowFlags(Qt::WindowTitleHint);

  kal::GuiLoader loader(*this);
  loader.load(":/resource/ui/loading_window.ui");
  setFixedSize(400, 66);

  loader.assign(ui_status, "status");
  loader.assign(ui_cancel, "cancel");

  connect(&m_timer, &QTimer::timeout, this, &LoadingWindow::updateStatus);

  connect(ui_cancel, &QPushButton::clicked, this, &LoadingWindow::cancel);

  m_timer.setInterval(1000);
  m_timer.setSingleShot(false);
  m_timer.start();

  updateStatus();
}

LoadingWindow::~LoadingWindow()
{
  if (m_timer.isActive())
  {
    m_timer.stop();
  }
}

void LoadingWindow::updateStatus()
{
  QString message = m_network.server().nameOrDefault();
  if (m_connecting)
  {
    message = tr("Connecting to %1").arg(message);
  }
  else
  {
    message = tr("Disconnecting from %1").arg(message);
  }
  message += QString(m_counter, '.');
  ui_status->setText(message);
  m_counter = ++m_counter % 4;
}

void LoadingWindow::cancel()
{
  m_connecting = false;
  m_counter = 0;
  updateStatus();
  m_network.disconnectFromServer();
}
