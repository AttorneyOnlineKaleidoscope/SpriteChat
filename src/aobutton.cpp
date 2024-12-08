#include "aobutton.h"

#include "options.h"
#include "spritechatcommon.h"

AOButton::AOButton(Options &options, kal::AssetPathResolver &assetPathResolver, QWidget *parent)
    : QPushButton(parent)
    , options(options)
    , m_resolver(assetPathResolver)
{
  m_movie = new QMovie(this);

  connect(m_movie, &QMovie::frameChanged, this, [this] {
    this->setIcon(m_movie->currentPixmap().scaled(this->size(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation));
    this->setIconSize(QSize(this->width(), this->height()));
  });
}

AOButton::~AOButton()
{
  deleteMovie();
}

void AOButton::setImage(QString image_name)
{
  deleteMovie();

  auto file_path = m_resolver.currentThemeFilePath(image_name, kal::AnimatedImageAssetType);
  if (!file_path)
  {
    setText(image_name);
    setStyleSheet(QString());
    setIcon(QIcon());
    return;
  }

  setText(QString());
  setStyleSheet("QPushButton { background-color: transparent; border: 0px }");

  if (options.animatedThemeEnabled())
  {
    m_movie = new QMovie;
    m_movie->setFileName(file_path.value());

    connect(m_movie, &QMovie::frameChanged, this, &AOButton::handleNextFrame);

    m_movie->start();
  }
  else
  {
    updateIcon(QPixmap(file_path.value()));
  }
}

void AOButton::deleteMovie()
{
  if (m_movie)
  {
    disconnect(m_movie, &QMovie::frameChanged, this, &AOButton::handleNextFrame);

    m_movie->stop();
    m_movie->deleteLater();
    m_movie = nullptr;
  }
}

void AOButton::handleNextFrame()
{
  updateIcon(m_movie->currentPixmap());
}

void AOButton::updateIcon(QPixmap icon)
{
  const QSize current_size = size();
  setIcon(icon.scaled(current_size, Qt::IgnoreAspectRatio, Qt::SmoothTransformation));
  setIconSize(current_size);
}
