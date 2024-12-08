#pragma once

#include "animationlayer.h"

#include <QWidget>

class AOEmotePreview : public QWidget
{
  Q_OBJECT

public:
  AOEmotePreview(AOApplication &application, kal::AssetPathResolver &assetPathResolver, QWidget *parent = nullptr);

  void display(QString character, QString emote, kal::EmoteType emoteType, bool flipped = false, int xOffset = 0, int yOffset = 0);

  void updateViewportGeometry();

protected:
  void resizeEvent(QResizeEvent *event);

private:
  QString m_character;
  QString m_emote;

  QWidget *ui_viewport;
  kal::BackgroundAnimationLayer *ui_vp_background;
  kal::SplashAnimationLayer *ui_vp_speedlines;
  kal::CharacterAnimationLayer *ui_vp_player_char;
  kal::BackgroundAnimationLayer *ui_vp_desk;
  QLabel *ui_size_label;
};
