#pragma once

#include "aoapplication.h"
#include "aoimage.h"
#include "gameglobal.h"

#include <QEnterEvent>
#include <QFile>
#include <QPushButton>
#include <QString>
#include <QWidget>

class AOCharButton : public QPushButton
{
  Q_OBJECT

public:
  AOCharButton(Options &options, kal::AssetPathResolver &assetPathResolver, QWidget *parent);

  void setCharacterId(kal::CharacterId id);
  void setCharacter(QString character);
  void setTaken(bool enabled);

Q_SIGNALS:
  void characterClicked(kal::CharacterId);

protected:
  void enterEvent(QEnterEvent *event) override;
  void leaveEvent(QEvent *event) override;

private:
  Options &options;
  kal::AssetPathResolver &m_resolver;
  kal::CharacterId m_id = kal::NoCharacterId;
  bool m_taken = false;
  AOImage *ui_taken;
  AOImage *ui_selector;

private Q_SLOTS:
  void notifyClick();
};
