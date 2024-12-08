#pragma once

#include "aoapplication.h"
#include "aoimage.h"

#include <QDebug>
#include <QEnterEvent>
#include <QPushButton>
#include <QString>

class AOEvidenceButton : public QPushButton
{
  Q_OBJECT

public:
  AOEvidenceButton(int id, int width, int height, Options &options, kal::AssetPathResolver &assetPathResolver, QWidget *parent = nullptr);

  void setEvidenceImage(QString fileName);

  void setThemeImage(QString fileName);

  void setSelected(bool enabled);

Q_SIGNALS:
  void evidenceClicked(int id);
  void evidenceDoubleClicked(int id);

  void mouseoverUpdated(int id, bool state);

protected:
  void enterEvent(QEnterEvent *e) override;
  void leaveEvent(QEvent *e) override;

  void mouseDoubleClickEvent(QMouseEvent *e) override;

private:
  Options &options;
  kal::AssetPathResolver &m_resolver;

  int m_id = 0;

  AOImage *ui_selected;
  AOImage *ui_selector;

  void setImage(const kal::MaybePath &fileName);

private Q_SLOTS:
  void on_clicked();
};
