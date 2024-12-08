#include "aoevidencebutton.h"

#include "file_functions.h"
#include "spritechatcommon.h"

AOEvidenceButton::AOEvidenceButton(int id, int width, int height, Options &options, kal::AssetPathResolver &assetPathResolver, QWidget *parent)
    : QPushButton(parent)
    , options(options)
    , m_resolver(assetPathResolver)
    , m_id(id)
{
  resize(width, height);

  ui_selected = new AOImage(assetPathResolver, this);
  ui_selected->resize(width, height);
  ui_selected->setImage("evidence_selected");
  ui_selected->setAttribute(Qt::WA_TransparentForMouseEvents);
  ui_selected->hide();

  ui_selector = new AOImage(assetPathResolver, this);
  ui_selector->resize(width, height);
  ui_selector->setImage("evidence_selector");
  ui_selector->setAttribute(Qt::WA_TransparentForMouseEvents);
  ui_selector->hide();

  connect(this, &AOEvidenceButton::clicked, this, &AOEvidenceButton::on_clicked);
}

void AOEvidenceButton::setEvidenceImage(QString fileName)
{
  setImage(m_resolver.evidenceFilePath(fileName));
}

void AOEvidenceButton::setThemeImage(QString fileName)
{
  setImage(m_resolver.themeFilePath(fileName, options.theme()));
}

void AOEvidenceButton::setSelected(bool p_selected)
{
  if (p_selected)
  {
    ui_selected->show();
  }
  else
  {
    ui_selected->hide();
  }
}

void AOEvidenceButton::on_clicked()
{
  Q_EMIT evidenceClicked(m_id);
}

void AOEvidenceButton::mouseDoubleClickEvent(QMouseEvent *e)
{
  QPushButton::mouseDoubleClickEvent(e);
  Q_EMIT evidenceDoubleClicked(m_id);
}

void AOEvidenceButton::setImage(const kal::MaybePath &fileName)
{
  if (fileName)
  {
    setStyleSheet(QStringLiteral(R"(QPushButton { border-image: url("%1") 0 0 0 0 stretch stretch; }")").arg(fileName.value()));
  }
  else
  {
    setStyleSheet("QPushButton { border-image: url(); background-color: #ffffff; border: 0px; }");
  }
}

void AOEvidenceButton::enterEvent(QEnterEvent *e)
{
  ui_selector->show();

  Q_EMIT mouseoverUpdated(m_id, true);

  setFlat(false);
  QPushButton::enterEvent(e);
}

void AOEvidenceButton::leaveEvent(QEvent *e)
{
  ui_selector->hide();

  Q_EMIT mouseoverUpdated(m_id, false);
  QPushButton::leaveEvent(e);
}
