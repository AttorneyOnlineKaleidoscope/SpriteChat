#pragma once

#include "assetpathresolver.h"

#include <QLabel>

class AOImage : public QLabel
{
  Q_OBJECT

public:
  AOImage(kal::AssetPathResolver &assetPathResolver, QWidget *parent = nullptr);

  QString image();

  bool setImage(QString fileName, QString miscellaneous);
  bool setImage(QString fileName);

private:
  kal::AssetPathResolver &m_resolver;

  QString m_file_name;
};
