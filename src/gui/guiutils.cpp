#include "guiutils.h"

#include <QGuiApplication>
#include <QScreen>

void kal::center_widget(QWidget *widget)
{
  QRect geometry;
  if (QWidget *parent = widget->parentWidget())
  {
    geometry = parent->geometry();
  }
  else
  {
    geometry = QGuiApplication::screenAt(widget->pos())->geometry();
    if (!geometry.isValid())
    {
      geometry = QGuiApplication::primaryScreen()->geometry();
    }
  }

  int x = geometry.x() + (geometry.width() - widget->width()) / 2;
  int y = geometry.y() + (geometry.height() - widget->height()) / 2;
  widget->move(x, y);
}
