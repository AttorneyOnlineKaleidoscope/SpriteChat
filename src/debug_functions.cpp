#include "debug_functions.h"

#include "widget/messagedialog.h"

void call_error(QString p_message)
{
  kal::MessageDialog::warning(p_message, QObject::tr("Error"));
}

void call_notice(QString p_message)
{
  kal::MessageDialog::information(p_message, QObject::tr("Notice"));
}
