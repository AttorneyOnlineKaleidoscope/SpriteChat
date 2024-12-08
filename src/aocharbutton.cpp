#include "aocharbutton.h"

#include "file_functions.h"

AOCharButton::AOCharButton(Options &options, kal::AssetPathResolver &assetPathResolver, QWidget *parent)
    : QPushButton(parent)
    , options(options)
    , m_resolver(assetPathResolver)
{
  int size = 60 * options.themeScalingFactor();
  int selector_size = 62 * options.themeScalingFactor();

  resize(size, size);

  ui_taken = new AOImage(assetPathResolver, this);
  ui_taken->setAttribute(Qt::WA_TransparentForMouseEvents);
  ui_taken->resize(size, size);
  ui_taken->setImage("char_taken");
  ui_taken->hide();

  ui_selector = new AOImage(assetPathResolver, parent);
  ui_selector->setAttribute(Qt::WA_TransparentForMouseEvents);
  ui_selector->resize(selector_size, selector_size);
  ui_selector->setImage("char_selector");
  ui_selector->hide();

  connect(this, &AOCharButton::clicked, this, &AOCharButton::notifyClick);
}

void AOCharButton::setCharacterId(kal::CharacterId id)
{
  m_id = id;
}

void AOCharButton::setTaken(bool enabled)
{
  if (enabled)
  {
    ui_taken->move(0, 0);
    ui_taken->show();
  }
  else
  {
    ui_taken->hide();
  }
}

void AOCharButton::setCharacter(QString character)
{
  setText(QString());

  auto image_path = m_resolver.characterFilePath(character, "char_icon", kal::ImageAssetType);
  if (image_path)
  {
    setStyleSheet("QPushButton { border-image: url(\"" + image_path.value() +
                  "\") 0 0 0 0 stretch stretch; }"
                  "QToolTip { background-image: url(); color: #000000; "
                  "background-color: #ffffff; border: 0px; }");
  }
  else
  {
    setStyleSheet("QPushButton { border-image: url(); }"
                  "QToolTip { background-image: url(); color: #000000; "
                  "background-color: #ffffff; border: 0px; }");
    setText(character);
  }
}

void AOCharButton::enterEvent(QEnterEvent *event)
{
  int offset = options.themeScalingFactor();
  ui_selector->move(x() - offset, y() - offset);
  ui_selector->raise();
  ui_selector->show();

  setFlat(false);

  QPushButton::enterEvent(event);
}

void AOCharButton::leaveEvent(QEvent *event)
{
  ui_selector->hide();

  QPushButton::leaveEvent(event);
}

void AOCharButton::notifyClick()
{
  Q_EMIT characterClicked(m_id);
}
