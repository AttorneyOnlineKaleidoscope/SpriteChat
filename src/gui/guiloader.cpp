#include "guiloader.h"

#include <QFile>
#include <QLayout>
#include <QMainWindow>
#include <QMetaProperty>
#include <QMetaType>

kal::GuiLoader::GuiLoader(QWidget &parent)
    : m_parent(parent)
{}

void kal::GuiLoader::load(const QString &fileName)
{
  QFile file(fileName);
  if (!file.open(QIODevice::ReadOnly))
  {
    kCritical("Failed to open UI file: %s", qUtf8Printable(fileName));
    return;
  }

  QWidget *source = m_loader.load(&file);
  if (QMainWindow *window = qobject_cast<QMainWindow *>(&m_parent))
  {
    window->setCentralWidget(source);
  }
  else
  {
    QLayout *layout = m_parent.layout();
    if (!layout)
    {
      layout = new QVBoxLayout(&m_parent);
    }

    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);
    layout->addWidget(source);
  }
}
