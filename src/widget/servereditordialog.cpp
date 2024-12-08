#include "servereditordialog.h"

#include "datatypes.h"
#include "debug_functions.h"
#include "gui/guiloader.h"
#include "options.h"

#include <QDebug>
#include <QFile>
#include <QUiLoader>
#include <QVBoxLayout>

ServerEditorDialog::ServerEditorDialog(QWidget *parent)
    : QDialog(parent)
{
  kal::GuiLoader loader(*this);
  loader.load(":/resource/ui/favorite_server_dialog.ui");

  loader.assign(ui_name, "name");
  loader.assign(ui_address, "address");
  loader.assign(ui_port, "port");
  loader.assign(ui_description, "description");
  loader.assign(ui_button_box, "button_box");

  loader.assign(ui_legacy_edit, "legacy_edit");
  loader.assign(ui_parse_legacy, "parse_legacy");

  connect(ui_parse_legacy, &QPushButton::released, this, &ServerEditorDialog::parseLegacyEntry);

  connect(ui_button_box, &QDialogButtonBox::accepted, this, &ServerEditorDialog::accept);
  connect(ui_button_box, &QDialogButtonBox::rejected, this, &ServerEditorDialog::reject);
}

ServerEditorDialog::ServerEditorDialog(const kal::ServerInfo &server, QWidget *parent)
    : ServerEditorDialog(parent)
{
  ui_name->setText(server.name);
  ui_description->setPlainText(server.description);
  ui_address->setText(server.address);
  ui_port->setValue(server.port);
}

kal::ServerInfo ServerEditorDialog::currentServerInfo() const
{
  kal::ServerInfo server;
  server.name = ui_name->text();
  server.description = ui_description->toPlainText();
  server.address = ui_address->text();
  server.port = ui_port->value();
  return server;
}

void ServerEditorDialog::parseLegacyEntry()
{
  QStringList entry = ui_legacy_edit->text().split(":");
  if (entry.size() < 3)
  {
    call_error("Invalid legacy server entry");
    return;
  }

  ui_address->setText(entry.at(0));
  ui_port->setValue(entry.at(1).toInt());
  ui_name->setText(entry.at(2));
}
