#pragma once

#include "options.h"
#include "pathresolver.h"

#include <QHash>
#include <QObject>
#include <QString>

namespace kal
{
enum AssetType
{
  NoAssetType,
  ImageAssetType,
  AnimatedImageAssetType,
  AudioAssetType,
};

class AssetPathResolver : public QObject
{
  Q_OBJECT

public:
  static QString applicationPath();
  static QString applicationFilePath(const QString &fileName);
  static QString applicationAssetPath();
  static QString applicationAssetFilePath(const QString &fileName);

  static const QStringList &imageSuffixList();
  static const QStringList &animatedImageSuffixList();
  static const QStringList &audioSuffixList();

  explicit AssetPathResolver(PathResolver &pathResolver, Options &options, QObject *parent = nullptr);
  virtual ~AssetPathResolver();

  MaybePath themePath(const QString &theme);
  MaybePath themeFilePath(const QString &theme, const QString &fileName, AssetType type = NoAssetType);

  MaybePath currentThemePath();
  MaybePath currentThemeFilePath(const QString &fileName, AssetType type = NoAssetType);

  MaybePath characterPath(const QString &character);
  MaybePath characterFilePath(const QString &character, const QString &fileName, AssetType type = NoAssetType);

  MaybePath backgroundPath(const QString &background);
  MaybePath backgroundFilePath(const QString &background, const QString &fileName, AssetType type = NoAssetType);

  MaybePath blipPath();
  MaybePath blipFilePath(const QString &blip);

  MaybePath musicPath();
  MaybePath musicFilePath(const QString &music);

  MaybePath evidenceFilePath(const QString &evidence);

  MaybePath effectPath(const QString &effect);
  MaybePath effectFilePath(const QString &effect, const QString &fileName, AssetType type = NoAssetType);

  MaybePath soundPath();
  MaybePath soundFilePath(const QString &fileName, AssetType type = NoAssetType);

  MaybePath chatboxPath(const QString &chatbox);
  MaybePath chatboxFilePath(const QString &chatbox, const QString &fileName, AssetType type = NoAssetType);

  MaybePath shoutPath(const QString &shout);
  MaybePath shoutFilePath(const QString &shout, const QString &fileName, AssetType type = NoAssetType);

  MaybePath filePath(const QString &fileName, const QString &origin);
  MaybePath filePath(const QString &fileName, AssetType type = NoAssetType);

public Q_SLOTS:
  void clearCache();

Q_SIGNALS:
  void mountListChanged();
  void cacheCleared();

private:
  PathResolver &m_path_resolver;
  Options &options;

private Q_SLOTS:
  void updateMountList();
};
} // namespace kal
