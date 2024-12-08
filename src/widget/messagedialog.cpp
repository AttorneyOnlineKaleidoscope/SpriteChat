#include "messagedialog.h"

#include "gui/guiloader.h"

#include <QPushButton>
#include <QTimer>

int kal::MessageDialog::information(const QString &text, const QString &title, TextFormat format, QWidget *parent)
{
  return display(QStyle::SP_MessageBoxInformation, text, title, format, parent);
}

int kal::MessageDialog::question(const QString &text, const QString &title, TextFormat format, QWidget *parent)
{
  return display(QStyle::SP_MessageBoxQuestion, text, title, format, parent);
}

int kal::MessageDialog::warning(const QString &text, const QString &title, TextFormat format, QWidget *parent)
{
  return display(QStyle::SP_MessageBoxWarning, text, title, format, parent);
}

int kal::MessageDialog::critical(const QString &text, const QString &title, TextFormat format, QWidget *parent)
{
  return display(QStyle::SP_MessageBoxCritical, text, title, format, parent);
}

kal::MessageDialog::MessageDialog::MessageDialog(QStyle::StandardPixmap icon, QWidget *parent)
    : QDialog{parent}
{
  kal::GuiLoader loader(*this);
  loader.load(":/resource/ui/message_dialog.ui");

  loader.assign(ui_icon, "icon");
  loader.assign(ui_error, "error");
  loader.assign(ui_button, "button");

  ui_button->setStandardButtons(QDialogButtonBox::Ok);

  connect(ui_button->button(QDialogButtonBox::Ok), &QPushButton::clicked, this, &QDialog::accept);

  setModal(true);
}

void kal::MessageDialog::setIcon(QStyle::StandardPixmap icon)
{
  ui_icon->setPixmap(style()->standardPixmap(icon));
}

void kal::MessageDialog::setText(const QString &text)
{
  ui_error->setPlainText(text);
}

void kal::MessageDialog::setHtml(const QString &html)
{
  ui_error->setHtml(html);
}

void kal::MessageDialog::setMarkdown(const QString &markdown)
{
  ui_error->setMarkdown(markdown);
}

void kal::MessageDialog::setGracePeriod(int seconds)
{
  resetTimer();
  m_counter = qMax(0, seconds);
  if (seconds > 0)
  {
    m_timer = new QTimer(this);
    m_timer->setInterval(1000);
    m_timer->setSingleShot(false);
    connect(m_timer, &QTimer::timeout, this, [this]() {
      --m_counter;
      updateCounter();
    });
    m_timer->start();
  }
  updateCounter();
}

int kal::MessageDialog::exec()
{
  return QDialog::exec();
}

int kal::MessageDialog::display(QStyle::StandardPixmap icon, const QString &text, const QString &title, TextFormat format, QWidget *parent)
{
  MessageDialog *dialog = new MessageDialog(icon, parent);
  dialog->setAttribute(Qt::WA_DeleteOnClose);
  dialog->setIcon(icon);
  if (!title.isEmpty())
  {
    dialog->setWindowTitle(title);
  }
  else
  {
    switch (icon)
    {
    default:
    case QStyle::SP_MessageBoxInformation:
      dialog->setWindowTitle(tr("Information"));
      break;

    case QStyle::SP_MessageBoxQuestion:
      dialog->setWindowTitle(tr("Question"));
      break;

    case QStyle::SP_MessageBoxWarning:
      dialog->setWindowTitle(tr("Warning"));
      break;

    case QStyle::SP_MessageBoxCritical:
      dialog->setWindowTitle(tr("Error"));
      break;
    }
  }

  switch (format)
  {
  default:
  case Text:
    dialog->setText(text);
    break;

  case Html:
    dialog->setHtml(text);
    break;

  case Markdown:
    dialog->setMarkdown(text);
    break;
  }

  switch (icon)
  {
  default:
    break;

  case QStyle::SP_MessageBoxWarning:
  case QStyle::SP_MessageBoxCritical:
    dialog->setWindowFlags(dialog->windowFlags() & ~Qt::WindowCloseButtonHint);
    dialog->setGracePeriod(3);
    break;
  }

  return dialog->exec();
}

void kal::MessageDialog::resetTimer()
{
  if (m_timer)
  {
    if (m_timer->isActive())
    {
      m_timer->stop();
    }

    m_timer->deleteLater();
    m_timer = nullptr;
  }
}

void kal::MessageDialog::updateCounter()
{
  ui_button->setDisabled(m_counter > 0);
  QPushButton *button = ui_button->button(QDialogButtonBox::Ok);
  if (m_counter > 0)
  {
    button->setText(QStringLiteral("%1 (%2)").arg(tr("Ok")).arg(m_counter));
  }
  else
  {
    button->setText(tr("Ok"));
    resetTimer();
  }
}
