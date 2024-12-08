#pragma once

#include <QString>
#include <QVersionNumber>

namespace kal
{
class SpriteChat
{
public:
  static QString softwareName();
  static QVersionNumber softwareVersion();
  static QString softwareVersionString();
  static QString softwareAgent();
};
} // namespace kal
