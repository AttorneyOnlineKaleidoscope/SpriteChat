#include "assetpathresolver.h"

#include "options.h"
#include "widget/messagedialog.h"

#include <QCoreApplication>
#include <QDir>
#include <QDirIterator>
#include <QFileInfo>
#include <QStringList>

#ifdef Q_OS_WIN
#define ZEROAPI_CASE_SENSITIVE_FILE_SYSTEM false
#else
#define ZEROAPI_CASE_SENSITIVE_FILE_SYSTEM true
#endif

QString kal::AssetPathResolver::applicationPath()
{
  return PathResolver::applicationPath();
}

QString kal::AssetPathResolver::applicationFilePath(const QString &fileName)
{
  return PathResolver::applicationFilePath(fileName);
}

QString kal::AssetPathResolver::applicationAssetPath()
{
  static const QString path = []() {
    QString path = applicationPath();

    bool resolution_error = true;
    QDir dir(path);
    for (const QString &entry : dir.entryList(QDir::Dirs | QDir::NoDotAndDotDot))
    {
      if (entry.toLower() == QStringLiteral("base"))
      {
        resolution_error = false;
        path = dir.absoluteFilePath(entry);
        break;
      }
    }

    if (resolution_error)
    {
      QObject dummy;
      kal::MessageDialog::critical(QStringLiteral("No base directory found in %1. Verify your installation.").arg(path));
      connect(&dummy, &QObject::destroyed, qApp, &QCoreApplication::quit, Qt::QueuedConnection);
    }

    if (path.endsWith(QChar(Qt::Key_Slash)) || path.endsWith(QChar(Qt::Key_Backslash)))
    {
      path.chop(1);
    }

    return path;
  }();

  return path;
}

QString kal::AssetPathResolver::applicationAssetFilePath(const QString &fileName)
{
  static const QString asset = QFileInfo(applicationAssetPath()).fileName();
  return applicationFilePath(asset % QStringLiteral("/") % fileName);
}

const QStringList &kal::AssetPathResolver::imageSuffixList()
{
  static const QStringList list = QStringList{"png"};
  return list;
}

const QStringList &kal::AssetPathResolver::animatedImageSuffixList()
{
  static const QStringList list = QStringList{"webp", "apng", "gif", "png"};
  return list;
}

const QStringList &kal::AssetPathResolver::audioSuffixList()
{
  static const QStringList list = QStringList{"opus", "ogg", "mp3", "wav"};
  return list;
}

kal::AssetPathResolver::AssetPathResolver(PathResolver &pathResolver, Options &options, QObject *parent)
    : QObject(parent)
    , m_path_resolver(pathResolver)
    , options(options)
{
  connect(&m_path_resolver, &PathResolver::mountListChanged, this, &AssetPathResolver::mountListChanged);
  connect(&m_path_resolver, &PathResolver::cacheCleared, this, &AssetPathResolver::cacheCleared);

  connect(&options, &Options::mountListChanged, this, &AssetPathResolver::updateMountList);

  updateMountList();
}

kal::AssetPathResolver::~AssetPathResolver()
{}

kal::MaybePath kal::AssetPathResolver::themePath(const QString &theme)
{
  return themeFilePath(theme, QString(), NoAssetType);
}

kal::MaybePath kal::AssetPathResolver::themeFilePath(const QString &theme, const QString &fileName, AssetType type)
{
  return filePath("themes/" % theme % "/" % fileName, type);
}

kal::MaybePath kal::AssetPathResolver::currentThemePath()
{
  return themePath(options.theme());
}

kal::MaybePath kal::AssetPathResolver::currentThemeFilePath(const QString &fileName, AssetType type)
{
  return themeFilePath(options.theme(), fileName, type);
}

kal::MaybePath kal::AssetPathResolver::characterPath(const QString &character)
{
  return characterFilePath(character, QString(), NoAssetType);
}

kal::MaybePath kal::AssetPathResolver::characterFilePath(const QString &character, const QString &fileName, AssetType type)
{
  return filePath("characters/" % character % "/" % fileName, type);
}

kal::MaybePath kal::AssetPathResolver::backgroundPath(const QString &background)
{
  return backgroundFilePath(background, QString(), NoAssetType);
}

kal::MaybePath kal::AssetPathResolver::backgroundFilePath(const QString &background, const QString &fileName, AssetType type)
{
  return filePath("background/" % background % "/" % fileName, type);
}

kal::MaybePath kal::AssetPathResolver::blipPath()
{
  return blipFilePath(QString());
}

kal::MaybePath kal::AssetPathResolver::blipFilePath(const QString &blip)
{
  return filePath("sounds/blip/" % blip, AudioAssetType);
}

kal::MaybePath kal::AssetPathResolver::musicPath()
{
  return musicFilePath(QString());
}

kal::MaybePath kal::AssetPathResolver::musicFilePath(const QString &music)
{
  return filePath("sounds/music/" % music, AudioAssetType);
}

kal::MaybePath kal::AssetPathResolver::evidenceFilePath(const QString &evidence)
{
  return filePath("evidence/" % evidence, ImageAssetType);
}

kal::MaybePath kal::AssetPathResolver::effectPath(const QString &effect)
{
  return effectFilePath(effect, QString(), NoAssetType);
}

kal::MaybePath kal::AssetPathResolver::effectFilePath(const QString &effect, const QString &fileName, AssetType type)
{
  return filePath("effects/" % effect % "/" % fileName, type);
}

kal::MaybePath kal::AssetPathResolver::soundPath()
{
  return soundFilePath(QString(), NoAssetType);
}

kal::MaybePath kal::AssetPathResolver::soundFilePath(const QString &fileName, AssetType type)
{
  return filePath("sounds/general/" % fileName, type);
}

kal::MaybePath kal::AssetPathResolver::chatboxPath(const QString &chatbox)
{
  return chatboxFilePath(chatbox, QString(), NoAssetType);
}

kal::MaybePath kal::AssetPathResolver::chatboxFilePath(const QString &chatbox, const QString &fileName, AssetType type)
{
  return filePath("chatbox/" % chatbox % "/" % fileName, type);
}

kal::MaybePath kal::AssetPathResolver::shoutPath(const QString &shout)
{
  return shoutFilePath(shout, QString(), NoAssetType);
}

kal::MaybePath kal::AssetPathResolver::shoutFilePath(const QString &shout, const QString &fileName, AssetType type)
{
  return filePath("shouts/" % shout % "/" % fileName, type);
}

kal::MaybePath kal::AssetPathResolver::filePath(const QString &fileName, const QString &origin)
{
  return m_path_resolver.filePath(fileName, origin);
}

kal::MaybePath kal::AssetPathResolver::filePath(const QString &fileName, AssetType type)
{
  const QStringList *suffixes = nullptr;
  switch (type)
  {
  default:
  case NoAssetType:
    break;

  case ImageAssetType:
    suffixes = &imageSuffixList();
    break;

  case AnimatedImageAssetType:
    suffixes = &animatedImageSuffixList();
    break;

  case AudioAssetType:
    suffixes = &audioSuffixList();
    break;
  }

  if (suffixes)
  {
    for (const QString &suffix : *suffixes)
    {
      if (auto path = filePath(fileName % '.' % suffix, fileName))
      {
        return path;
      }
    }
  }

  return filePath(fileName, fileName);
}

void kal::AssetPathResolver::clearCache()
{
  m_path_resolver.clearCache();
}

void kal::AssetPathResolver::updateMountList()
{
  QStringList m_complete_mount_list = options.mountPaths();
  m_complete_mount_list.prepend(applicationAssetPath());
  m_path_resolver.setMountList(m_complete_mount_list);
}
