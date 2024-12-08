#include "aoapplication.h"
#include "assetpathresolver.h"
#include "courtroom.h"
#include "debug_functions.h"
#include "file_functions.h"
#include "lobby.h"
#include "pathresolver.h"
#include "spritechatversion.h"

#include <QDebug>
#include <QDirIterator>
#include <QImageReader>
#include <QLibraryInfo>
#include <QResource>
#include <QTranslator>

int main(int argc, char *argv[])
{
  qSetMessagePattern("%{type}: %{if-category}%{category}: %{endif}%{message}");

#ifdef QT_DEBUG
  qSetMessagePattern("file://%{file}:%{line}\n[%{time h:mm:ss.zzz}][%{if-debug}D%{endif}%{if-info}I%{endif}%{if-warning}W%{endif}%{if-critical}C%{endif}%{if-fatal}F%{endif}]%{function}: %{message}");
#endif

  QApplication app(argc, argv);
  QApplication::setApplicationVersion(kal::SpriteChat::softwareVersionString());
  QApplication::setApplicationDisplayName(QStringLiteral("%1 (%2)").arg(kal::SpriteChat::softwareName(), kal::SpriteChat::softwareVersionString()));
#ifdef QT_DEBUG
  QApplication::setWindowIcon(QIcon(":/logo.png"));
#endif

  int exit_code = 0;
  {
    Options options;

    QFont scaled_font = QApplication::font();
    scaled_font.setPointSize(scaled_font.pointSize() * options.themeScalingFactor());
    QApplication::setFont(scaled_font);

    QStringList font_paths;
    font_paths.append(kal::AssetPathResolver::applicationAssetPath());
    font_paths.append(options.mountPaths());

    for (const QString &path : font_paths)
    {
      QDir dir(path);
      dir.setPath(dir.absoluteFilePath("fonts"));
      for (const QString &file : dir.entryList(QDir::Files | QDir::NoDotAndDotDot))
      {
        kInfo() << "Loading font" << file;
        QFontDatabase::addApplicationFont(dir.absoluteFilePath(file));
      }
    }

    QStringList expected_formats = kal::AssetPathResolver::animatedImageSuffixList();
    for (const QByteArray &i_format : QImageReader::supportedImageFormats())
    {
      if (expected_formats.contains(i_format, Qt::CaseInsensitive))
      {
        expected_formats.removeAll(i_format.toLower());
      }
    }

    if (!expected_formats.isEmpty())
    {
      call_error("Missing image formats: <b>" + expected_formats.join(", ") + "</b>.<br /><br /> Please make sure you have installed the application properly.");
    }

    QString p_language = options.language();
    if (p_language.trimmed().isEmpty())
    {
      p_language = QLocale::system().name();
    }

    QTranslator qtTranslator;
    if (!qtTranslator.load("qt_" + p_language, QLibraryInfo::path(QLibraryInfo::TranslationsPath)))
    {
      kDebug() << "Failed to load translation qt_" + p_language;
    }
    else
    {
      QApplication::installTranslator(&qtTranslator);
    }

    QTranslator appTranslator;
    if (appTranslator.load("ao_" + p_language, ":/resource/translations/"))
    {
      QApplication::installTranslator(&appTranslator);
      kInfo() << "Loaded translation ao_" + p_language;
    }
    else
    {
      kWarning() << "Failed to load translation ao_" + p_language;
    }

    kal::PathResolver path_resolver;
    kal::AssetPathResolver asset_path_resolver(path_resolver, options);
    AOApplication main_app(options, asset_path_resolver);
    main_app.default_font = scaled_font;

    if (auto path = asset_path_resolver.currentThemeFilePath("themes/" % options.theme() % ".rcc"))
    {
      QResource::registerResource(path.value());
    }

    exit_code = app.exec();
  }

  return exit_code;
}
