#include "directconnectdialog.h"

#include "debug_functions.h"
#include "gui/guiloader.h"

#include <QRegularExpression>

DirectConnectDialog::DirectConnectDialog(kal::NetworkManager &network, QWidget *parent)
    : QDialog(parent)
    , m_network(network)
{
  kal::GuiLoader loader(*this);
  loader.load(":/resource/ui/direct_connect_dialog.ui");

  loader.assign(ui_address, "address");
  loader.assign(ui_connect, "connect");
  connect(ui_connect, &QPushButton::clicked, this, &DirectConnectDialog::parseAndConnectToServer);
  loader.assign(ui_cancel, "cancel");
  connect(ui_cancel, &QPushButton::clicked, this, &DirectConnectDialog::close);
}

void DirectConnectDialog::parseAndConnectToServer()
{
  QString address = ui_address->text();
  static const QRegularExpression ADDRESS_PATTERN(R"(\w+://)");
  if (!address.contains(ADDRESS_PATTERN))
  {
    address = "ws://" + address;
  }

  QUrl url(address);
  if (url.scheme() != "ws")
  {
    call_error(tr("Invalid URL scheme. Only ws:// is supported."));
    return;
  }

  if (!url.isValid())
  {
    call_error(tr("Invalid URL."));
    return;
  }

  if (url.port() == -1)
  {
    call_error(tr("Invalid server port."));
    return;
  }

  kal::ServerInfo server;
  server.address = url.host();
  server.port = url.port();
  server.name = "Direct Connection";
  m_network.setAndConnectToServer(server);

  close();
}
