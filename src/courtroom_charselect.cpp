#include "courtroom.h"
#include "lobby.h"

#include "debug_functions.h"
#include "file_functions.h"
#include "hardware_functions.h"
#include "network/packet/character_packets.h"

void Courtroom::buildCharacterSelect()
{
  this->setWindowFlags((this->windowFlags() | Qt::CustomizeWindowHint) & ~Qt::WindowMaximizeButtonHint);

  ui_char_select_background = new AOImage(m_resolver, this);
  ui_char_select_background->setObjectName("ui_char_select_background");

  ui_character_tree = new QTreeWidget(ui_char_select_background);
  ui_character_tree->setColumnCount(2);
  ui_character_tree->setHeaderLabels({"Name", "ID"});
  ui_character_tree->setHeaderHidden(true);
  ui_character_tree->header()->setSectionResizeMode(QHeaderView::ResizeToContents);
  ui_character_tree->hideColumn(1);
  ui_character_tree->setDropIndicatorShown(true);
  ui_character_tree->setObjectName("ui_char_list");

  ui_char_buttons = new QWidget(ui_char_select_background);
  ui_char_buttons->setObjectName("ui_char_buttons");

  ui_back_to_lobby = new AOButton(options, m_resolver, ui_char_select_background);
  ui_back_to_lobby->setObjectName("ui_back_to_lobby");

  ui_close_character_select = new AOButton(options, m_resolver, ui_char_select_background);
  ui_close_character_select->setObjectName("ui_close_character_select");

  ui_char_select_left = new AOButton(options, m_resolver, ui_char_select_background);
  ui_char_select_left->setObjectName("ui_char_select_left");
  ui_char_select_right = new AOButton(options, m_resolver, ui_char_select_background);
  ui_char_select_right->setObjectName("ui_char_select_right");

  ui_spectator = new AOButton(options, m_resolver, ui_char_select_background);
  ui_spectator->setText(tr("Spectator"));
  ui_spectator->setObjectName("ui_spectator");

  ui_char_search = new QLineEdit(ui_char_select_background);
  ui_char_search->setPlaceholderText(tr("Search"));
  ui_char_search->setObjectName("ui_char_search");

  ui_char_taken = new QCheckBox(ui_char_select_background);
  ui_char_taken->setText(tr("Taken"));
  ui_char_taken->setObjectName("ui_char_taken");

  connect(ui_character_tree, &QTreeWidget::itemDoubleClicked, this, &Courtroom::on_char_list_double_clicked);

  connect(ui_back_to_lobby, &AOButton::clicked, this, &Courtroom::on_back_to_lobby_clicked);
  connect(ui_close_character_select, &AOButton::clicked, [this]() {
    ui_char_select_background->hide();
    set_courtroom_size();
  });

  connect(ui_char_select_left, &AOButton::clicked, this, &Courtroom::previousCharacterSelectPage);
  connect(ui_char_select_right, &AOButton::clicked, this, &Courtroom::nextCharacterSelectPage);

  connect(ui_spectator, &AOButton::clicked, this, &Courtroom::requestSpectating);

  connect(ui_char_search, &QLineEdit::textEdited, this, &Courtroom::filterCharacterSelectList);
  connect(ui_char_taken, &QCheckBox::stateChanged, this, &Courtroom::filterCharacterSelectList);
}

void Courtroom::setupCharacterSelect()
{
  QString filename = "courtroom_design.ini";

  pos_size_type f_charselect = ao_app.get_element_dimensions("char_select", filename);

  if (f_charselect.width < 0 || f_charselect.height < 0)
  {
    kWarning() << "did not find char_select width or height in "
                  "courtroom_design.ini!";
    this->setFixedSize(m_courtroom_width, m_courtroom_height);
  }
  else
  {
    this->setFixedSize(f_charselect.width, f_charselect.height);
  }

  ui_char_select_background->resize(f_charselect.width, f_charselect.height);
  ui_char_select_background->setImage("charselect_background");

  set_size_and_pos(ui_char_search, "char_search");
  set_size_and_pos(ui_character_tree, "char_list");
  set_size_and_pos(ui_char_taken, "char_taken");
  set_size_and_pos(ui_char_buttons, "char_buttons");

  truncate_label_text(ui_char_taken, "char_taken");
}

void Courtroom::buildCharacterSelectList()
{
  for (AOCharButton *button : qAsConst(ui_character_list))
  {
    button->hide();
    button->deleteLater();
  }
  ui_character_list.clear();

  const float scaling = options.themeScalingFactor();

  const pos_size_type size = ao_app.get_element_dimensions("char_buttons", "courtroom_design.ini");
  const QPoint spacing = ao_app.get_button_spacing("char_button_spacing", "courtroom_design.ini") * scaling;
  const int width = button_width * scaling;
  const int height = button_height * scaling;

  m_character_grid.max_cols = qMax(1, (size.width + spacing.x()) / (width + spacing.x()));
  m_character_grid.max_rows = qMax(1, (size.height + spacing.y()) / (height + spacing.y()));
  m_character_grid.current_page = 0;

  for (int row = 0; row < m_character_grid.max_rows; ++row)
  {
    for (int col = 0; col < m_character_grid.max_cols; ++col)
    {
      AOCharButton *button = new AOCharButton(options, m_resolver, ui_char_buttons);
      button->resize(width, height);
      button->move((width + spacing.x()) * col, (height + spacing.y()) * row);
      ui_character_list.append(button);

      connect(button, &AOCharButton::characterClicked, this, &Courtroom::requestCharacter);
      connect(button, &AOCharButton::customContextMenuRequested, this, &Courtroom::on_char_button_context_menu_requested);
    }
  }
}

void Courtroom::filterCharacterSelectList()
{
  for (int i = 0; i < m_character_list.length(); ++i)
  {
    ui_character_map[i]->setHidden(true);
  }

  ui_filtered_character_list.clear();
  const QString search = ui_char_search->text();
  for (int i = 0; i < m_character_list.length(); ++i)
  {
    const CharacterSlot &slot = m_character_list.at(i);
    if (!ui_char_taken->isChecked() && slot.taken)
    {
      continue;
    }

    if (!slot.name.contains(search, Qt::CaseInsensitive))
    {
      continue;
    }

    ui_filtered_character_list.append(i);
    ui_character_map[i]->setHidden(false);
  }

  m_character_grid.count = ui_filtered_character_list.size();
  m_character_grid.current_page = 0;
  displayCurrentCharacterSelectPage();
}

void Courtroom::displayCurrentCharacterSelectPage()
{
  ui_char_select_left->setHidden(m_character_grid.current_page < 1);
  ui_char_select_right->setHidden(m_character_grid.current_page == m_character_grid.maxPageIndex());
  for (AOCharButton *button : qAsConst(ui_character_list))
  {
    button->hide();
  }

  const QList<int> indexes = ui_filtered_character_list.mid(m_character_grid.currentPageOffset(), m_character_grid.itemsPerPage());
  for (int i = 0; i < indexes.length(); ++i)
  {
    const kal::CharacterId target_id = indexes.at(i);
    const CharacterSlot &slot = m_character_list.at(target_id);
    AOCharButton *button = ui_character_list.at(i);
    button->setCharacterId(target_id);
    button->setCharacter(slot.name);
    button->setTaken(slot.taken);
    button->show();
  }
}

void Courtroom::previousCharacterSelectPage()
{
  m_character_grid.previousPage();
  displayCurrentCharacterSelectPage();
}

void Courtroom::nextCharacterSelectPage()
{
  m_character_grid.nextPage();
  displayCurrentCharacterSelectPage();
}

void Courtroom::requestCharacter(kal::CharacterId id)
{
  if (id == m_character_id)
  {
    update_character(id);
    set_courtroom_size();
    return;
  }

  Q_ASSERT_X(id >= 0 && id < m_character_list.size(), Q_FUNC_INFO, "Character ID out of bounds");

  QString char_name = m_character_list.at(id).name;
  kal::MaybePath char_ini_path = m_resolver.characterFilePath(char_name, kal::CHARACTER_DATA_FILE);
  if (!char_ini_path)
  {
    call_error(tr("Could not find character (char.ini) for %1").arg(char_name));
    return;
  }

  ao_app.createAndShipPacket<kal::SelectCharacterPacket>(id);
}

void Courtroom::requestSpectating()
{
  ao_app.createAndShipPacket<kal::SelectCharacterPacket>(kal::NoCharacterId);
}

void Courtroom::on_char_list_double_clicked(QTreeWidgetItem *p_item, int column)
{
  Q_UNUSED(column);
  int cid = p_item->text(1).toInt();
  if (cid == -1 && !p_item->isExpanded())
  {
    p_item->setExpanded(true);
    return;
  }
  else if (cid == -1)
  {
    p_item->setExpanded(false);
    return;
  }

  requestCharacter(cid);
}

void Courtroom::on_char_button_context_menu_requested(const QPoint &pos)
{
  AOCharButton *button = qobject_cast<AOCharButton *>(sender());
  int id = ui_character_list.indexOf(button);
  if (id == -1)
  {
    return;
  }

  QString char_name = m_character_list.at(id).name;
  kal::MaybePath char_ini_path = m_resolver.characterFilePath(char_name, kal::CHARACTER_DATA_FILE);
  if (!char_ini_path)
  {
    call_error(tr("Could not find character (char.ini) for %1").arg(char_name));
    return;
  }

  QMenu *menu = new QMenu(this);
  menu->addAction(QStringLiteral("Edit char.ini"), this, [char_ini_path] { QDesktopServices::openUrl(QUrl::fromLocalFile(char_ini_path.value())); });
  menu->addSeparator();
  menu->addAction(QStringLiteral("Open character folder"), this, [=, this] {
    kal::MaybePath p_path = m_resolver.characterPath(char_name);
    if (p_path)
    {
      QDesktopServices::openUrl(QUrl::fromLocalFile(p_path.value()));
    }
  });
  menu->popup(button->mapToGlobal(pos));
}

void Courtroom::buildCharacterSelectTree()
{
  ui_character_map.clear();
  ui_character_tree->clear();
  QList<QTreeWidgetItem *> orphans;
  QHash<QString, QTreeWidgetItem *> parents;
  for (int i = 0; i < m_character_list.length(); ++i)
  {
    const CharacterSlot &slot = m_character_list.at(i);

    QTreeWidgetItem *item = new QTreeWidgetItem;
    item->setText(0, slot.name);
    item->setIcon(0, QIcon(m_resolver.characterFilePath(slot.name, "char_icon", kal::ImageAssetType).value_or(QString())));
    ui_character_map.insert(i, item);

    const QString category = ao_app.get_category(slot.name);
    if (category.isEmpty())
    {
      orphans.append(item);
    }
    else
    {
      QTreeWidgetItem *parent;
      if (parents.contains(category))
      {
        parent = parents.value(category);
      }
      else
      {
        parent = new QTreeWidgetItem;
        parent->setText(0, category);
        parent->setChildIndicatorPolicy(QTreeWidgetItem::DontShowIndicatorWhenChildless);
        ui_character_tree->addTopLevelItem(parent);
        parents.insert(category, parent);
      }
      parent->addChild(item);
    }
  }
  ui_character_tree->addTopLevelItems(orphans);
  ui_character_tree->expandAll();
}
