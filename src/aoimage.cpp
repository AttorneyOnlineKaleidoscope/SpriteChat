#include "aoimage.h"

#include <QPixmap>

AOImage::AOImage(kal::AssetPathResolver &assetPathResolver, QWidget *parent)
    : QLabel(parent)
    , m_resolver(assetPathResolver)
{}

QString AOImage::image()
{
  return m_file_name;
}

bool AOImage::setImage(QString fileName, QString miscellaneous)
{
  auto p_image_resolved = m_resolver.currentThemeFilePath(fileName, kal::AnimatedImageAssetType);
  if (!p_image_resolved)
  {
    kWarning() << "could not find image" << fileName;
    return false;
  }

  m_file_name = p_image_resolved.value();
  QPixmap f_pixmap(m_file_name);
  f_pixmap = f_pixmap.scaled(size(), Qt::IgnoreAspectRatio);
  setPixmap(f_pixmap);

  return true;
}

bool AOImage::setImage(QString fileName)
{
  return setImage(fileName, QString());
}
