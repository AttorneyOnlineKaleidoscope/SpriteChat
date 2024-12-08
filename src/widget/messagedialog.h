#pragma once

#include <QDialog>
#include <QDialogButtonBox>
#include <QLabel>
#include <QStyle>
#include <QTextBrowser>

namespace kal
{
class MessageDialog : public QDialog
{
  Q_OBJECT

public:
  enum TextFormat
  {
    Text,
    Html,
    Markdown,
  };

  static int information(const QString &text, const QString &title = QString(), TextFormat format = Text, QWidget *parent = nullptr);
  static int question(const QString &text, const QString &title = QString(), TextFormat format = Text, QWidget *parent = nullptr);
  static int warning(const QString &text, const QString &title = QString(), TextFormat format = Text, QWidget *parent = nullptr);
  static int critical(const QString &text, const QString &title = QString(), TextFormat format = Text, QWidget *parent = nullptr);

  explicit MessageDialog(QStyle::StandardPixmap icon, QWidget *parent = nullptr);

  void setIcon(QStyle::StandardPixmap icon);
  void setText(const QString &text);
  void setHtml(const QString &html);
  void setMarkdown(const QString &markdown);
  void setGracePeriod(int seconds);

  int exec() override;

private:
  QTimer *m_timer = nullptr;
  int m_counter = 0;

  QLabel *ui_icon;
  QTextBrowser *ui_error;
  QDialogButtonBox *ui_button;

  static int display(QStyle::StandardPixmap icon, const QString &text, const QString &title, TextFormat format, QWidget *parent);

  void resetTimer();

private Q_SLOTS:
  void updateCounter();
};
} // namespace kal
