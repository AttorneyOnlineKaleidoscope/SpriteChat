#pragma once

#include "assetpathresolver.h"

#include <QMovie>
#include <QPushButton>

class AOApplication;

class FlipButton : public QPushButton
{
  Q_OBJECT

public:
  explicit FlipButton(Options &options, kal::AssetPathResolver &assetPathResolver, QWidget *parent = nullptr);
  ~FlipButton();

  void setImage(const QString &enabledImage, const QString &disabledImage);
  void setPlaceholderText(const QString &text);

public Q_SLOTS:
  void displayCurrentImage();

private:
  Options &options;
  kal::AssetPathResolver &m_resolver;
  QString m_image[2];
  QString m_image_path;
  QString m_placeholder_text;
  QMovie *m_movie = nullptr;

  void resetMovie()
  {
    if (m_movie)
    {
      m_movie->disconnect(this);
      m_movie->deleteLater();
      m_movie = nullptr;
    }
  }
};
