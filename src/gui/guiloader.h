#pragma once

#include <QString>
#include <QUiLoader>
#include <QWidget>

namespace kal
{
class GuiLoader
{
public:
  explicit GuiLoader(QWidget &parent);

  void load(const QString &fileName);

  template <typename T>
  void assign(T *&object, const QString &objectName);

private:
  QWidget &m_parent;
  QUiLoader m_loader;
};

template <typename T>
void GuiLoader::assign(T *&object, const QString &objectName)
{
  object = m_parent.findChild<T *>(objectName);
  if (object == nullptr)
  {
    kCritical("Failed to find object: %s", qUtf8Printable(objectName));
    return;
  }
}
} // namespace kal
