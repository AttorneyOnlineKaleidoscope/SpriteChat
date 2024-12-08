#pragma once

#include "aoapplication.h"

#include <QMovie>
#include <QPushButton>

class AOButton : public QPushButton
{
  Q_OBJECT

public:
  explicit AOButton(Options &options, kal::AssetPathResolver &assetPathResolver, QWidget *parent = nullptr);
  virtual ~AOButton();

  void setImage(QString image_name);

private:
  Options &options;
  kal::AssetPathResolver &m_resolver;

  QMovie *m_movie = nullptr;

  void deleteMovie();

private Q_SLOTS:
  void handleNextFrame();

  void updateIcon(QPixmap icon);
};
