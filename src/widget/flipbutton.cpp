#include "flipbutton.h"

#include "aoapplication.h"
#include "options.h"
#include "spritechatcommon.h"

FlipButton::FlipButton(Options &options, kal::AssetPathResolver &assetPathResolver, QWidget *parent)
    : QPushButton(parent)
    , options(options)
    , m_resolver(assetPathResolver)
{
  setCheckable(true);

  connect(this, &QPushButton::toggled, this, &FlipButton::displayCurrentImage);
}

FlipButton::~FlipButton()
{
  resetMovie();
}

void FlipButton::setImage(const QString &enabledImage, const QString &disabledImage)
{
  m_image[0] = enabledImage;
  m_image[1] = disabledImage;
  displayCurrentImage();
}

void FlipButton::setPlaceholderText(const QString &text)
{
  m_placeholder_text = text;
  setToolTip(m_placeholder_text);
}

void FlipButton::displayCurrentImage()
{
  QString image_name = m_image[isChecked()];

  kal::MaybePath image_path = m_resolver.currentThemeFilePath(image_name, options.animatedThemeEnabled() ? kal::AnimatedImageAssetType : kal::ImageAssetType);
  if (image_path && m_image_path == image_path)
  {
    return;
  }
  m_image_path = image_path.value_or(QString());

  resetMovie();
  if (m_image_path.isEmpty())
  {
    setIcon(QIcon());
    setText(m_placeholder_text);
    setStyleSheet(QString());
  }
  else
  {
    setText(QString());
    setStyleSheet("QPushButton { background-color: transparent; border: 0px }");

    auto set_icon = [this](const QPixmap &pixmap) {
      setIcon(pixmap.scaled(size(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation));
      setIconSize(size());
    };

    if (options.animatedThemeEnabled())
    {
      m_movie = new QMovie;
      m_movie->setFileName(m_image_path);
      connect(m_movie, &QMovie::frameChanged, this, [this, set_icon]() { set_icon(m_movie->currentPixmap()); });
      m_movie->start();
    }
    else
    {
      set_icon(QPixmap(m_image_path));
    }
  }
}
