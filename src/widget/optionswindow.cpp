#include "optionswindow.h"

#include "file_functions.h"
#include "gui/guiloader.h"
#include "options.h"

#include <bass.h>

#include <QCollator>
#include <QDesktopServices>
#include <QFileDialog>
#include <QGroupBox>
#include <QMessageBox>
#include <QResource>
#include <QUiLoader>
#include <QVBoxLayout>

OptionsWindow::OptionsWindow(Options &options, kal::AssetPathResolver &assetPathResolver, QWidget *parent)
    : QDialog(parent)
    , options(options)
    , m_resolver(assetPathResolver)
{
  setupUI();
}

void OptionsWindow::populateAudioDevices()
{
  ui_audio_device_combobox->clear();
  if (needsDefaultAudioDevice())
  {
    ui_audio_device_combobox->addItem("default", "default");
  }

  BASS_DEVICEINFO info;
  for (int a = 0; BASS_GetDeviceInfo(a, &info); a++)
  {
    ui_audio_device_combobox->addItem(info.name, info.name);
  }
}

template <>
void OptionsWindow::setWidgetData(QCheckBox *widget, const bool &value)
{
  widget->setChecked(value);
}

template <>
bool OptionsWindow::widgetData(QCheckBox *widget) const
{
  return widget->isChecked();
}

template <>
void OptionsWindow::setWidgetData(QLineEdit *widget, const QString &value)
{
  widget->setText(value);
}

template <>
QString OptionsWindow::widgetData(QLineEdit *widget) const
{
  return widget->text();
}

template <>
void OptionsWindow::setWidgetData(QLineEdit *widget, const uint16_t &value)
{
  widget->setText(QString::number(value));
}

template <>
uint16_t OptionsWindow::widgetData(QLineEdit *widget) const
{
  return widget->text().toUShort();
}

template <>
void OptionsWindow::setWidgetData(QPlainTextEdit *widget, const QStringList &value)
{
  widget->setPlainText(value.join('\n'));
}

template <>
QStringList OptionsWindow::widgetData(QPlainTextEdit *widget) const
{
  return widget->toPlainText().trimmed().split('\n');
}

template <>
void OptionsWindow::setWidgetData(QSpinBox *widget, const int &value)
{
  widget->setValue(value);
}

template <>
int OptionsWindow::widgetData(QSpinBox *widget) const
{
  return widget->value();
}

template <>
void OptionsWindow::setWidgetData(QComboBox *widget, const QString &value)
{
  for (auto i = 0; i < widget->count(); i++)
  {
    if (widget->itemData(i).toString() == value)
    {
      widget->setCurrentIndex(i);
      return;
    }
  }
  kWarning() << "value" << value << "not found for widget" << widget->objectName();
}

template <>
void OptionsWindow::setWidgetData(QComboBox *widget, const RESIZE_MODE &value)
{
  widget->setCurrentIndex(value);
}

template <>
QString OptionsWindow::widgetData(QComboBox *widget) const
{
  return widget->currentData().toString();
}

template <>
RESIZE_MODE OptionsWindow::widgetData(QComboBox *widget) const
{
  return RESIZE_MODE(widget->currentIndex());
}

template <>
void OptionsWindow::setWidgetData(QGroupBox *widget, const bool &value)
{
  widget->setChecked(value);
}

template <>
bool OptionsWindow::widgetData(QGroupBox *widget) const
{
  return widget->isChecked();
}

template <>
void OptionsWindow::setWidgetData(QListWidget *widget, const QStringList &value)
{
  widget->addItems(value);
}

template <>
QStringList OptionsWindow::widgetData(QListWidget *widget) const
{
  QStringList paths;
  for (auto i = 1; i < widget->count(); i++)
  {
    paths.append(widget->item(i)->text());
  }
  return paths;
}

template <typename T, typename V>
void OptionsWindow::registerOption(const QString &widgetName, V (Options::*getter)() const, void (Options::*setter)(V))
{
  auto *widget = findChild<T *>(widgetName);
  if (!widget)
  {
    kWarning() << "could not find widget" << widgetName;
    return;
  }

  OptionEntry entry;
  entry.load = [=, this] {
    setWidgetData<T, V>(widget, (options.*getter)());
  };
  entry.save = [=, this] {
    (options.*setter)(widgetData<T, V>(widget));
  };

  optionEntries.append(entry);
}

void OptionsWindow::updateValues()
{
  QSet<QString> themes;
  QStringList bases = options.mountPaths();
  bases.push_front(kal::AssetPathResolver::applicationAssetPath());

  for (const QString &base : bases)
  {
    QStringList l_themes = QDir(base + "/themes").entryList(QDir::Dirs | QDir::NoDotAndDotDot);

    // Resorts list to match numeric sorting found in Windows.
    QCollator l_sorting;
    l_sorting.setNumericMode(true);
    std::sort(l_themes.begin(), l_themes.end(), l_sorting);

    for (const QString &l_theme : qAsConst(l_themes))
    {
      if (!themes.contains(l_theme))
      {
        ui_theme_combobox->addItem(l_theme, l_theme);
        themes.insert(l_theme);
      }
    }
  }

  for (const OptionEntry &entry : qAsConst(optionEntries))
  {
    entry.load();
  }
}

void OptionsWindow::savePressed()
{
  bool l_reload_theme_required = (ui_theme_combobox->currentText() != options.theme()) || (ui_theme_scaling_factor_sb->value() != options.themeScalingFactor());
  for (const OptionEntry &entry : qAsConst(optionEntries))
  {
    entry.save();
  }

  if (l_reload_theme_required)
  {
    Q_EMIT reloadThemeRequest();
  }
  close();
}

void OptionsWindow::discardPressed()
{
  close();
}

void OptionsWindow::buttonClicked(QAbstractButton *button)
{
  if (ui_settings_buttons->buttonRole(button) == QDialogButtonBox::ResetRole)
  {
    if (QMessageBox::question(this, "", "Restore default settings?\nThis can't be undone!", QMessageBox::Yes | QMessageBox::No) == QMessageBox::Yes)
    {
      // Destructive operation.
      options.clearConfig();
      updateValues();
    }
  }
}

void OptionsWindow::onReloadThemeClicked()
{
  options.setTheme(ui_theme_combobox->currentText());
  options.setAnimatedThemeEnabled(ui_animated_theme_cb->isChecked());
  optionEntries.clear();
}

void OptionsWindow::themeChanged(int i)
{
  { // unload previous
    QString previous_theme = options.theme();
    if (auto path = m_resolver.filePath("themes/" % previous_theme % ".rcc"))
    {
      if (QResource::unregisterResource(path.value()))
      {
        kInfo() << "Unloaded theme" << previous_theme;
      }
    }
  }

  QString theme = ui_theme_combobox->currentText();
  if (auto path = m_resolver.filePath("themes/" % theme % ".rcc"))
  {
    if (QResource::registerResource(path.value()))
    {
      kInfo() << "Loaded theme" << theme;
    }
  }
}

void OptionsWindow::setupUI()
{
  kal::GuiLoader loader(*this);
  loader.load(":/resource/ui/options_dialog.ui");

  // General dialog element.
  loader.assign(ui_settings_buttons, "settings_buttons");

  connect(ui_settings_buttons, &QDialogButtonBox::accepted, this, &OptionsWindow::savePressed);
  connect(ui_settings_buttons, &QDialogButtonBox::rejected, this, &OptionsWindow::discardPressed);
  connect(ui_settings_buttons, &QDialogButtonBox::clicked, this, &OptionsWindow::buttonClicked);

  // Gameplay Tab
  loader.assign(ui_theme_combobox, "theme_combobox");
  connect(ui_theme_combobox, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &OptionsWindow::themeChanged);

  registerOption<QComboBox, QString>("theme_combobox", &Options::theme, &Options::setTheme);

  loader.assign(ui_theme_reload_button, "theme_reload_button");
  connect(ui_theme_reload_button, &QPushButton::clicked, this, &::OptionsWindow::onReloadThemeClicked);

  loader.assign(ui_theme_folder_button, "theme_folder_button");
  connect(ui_theme_folder_button, &QPushButton::clicked, this, [=, this] {
    kal::MaybePath path = m_resolver.currentThemeFilePath(ui_theme_combobox->itemText(ui_theme_combobox->currentIndex()));
    if (path)
    {
      QDesktopServices::openUrl(QUrl::fromLocalFile(path.value()));
    }
  });

  loader.assign(ui_theme_scaling_factor_sb, "theme_scaling_factor_sb");
  loader.assign(ui_animated_theme_cb, "animated_theme_cb");
  loader.assign(ui_stay_time_spinbox, "stay_time_spinbox");
  loader.assign(ui_instant_objection_cb, "instant_objection_cb");
  loader.assign(ui_text_crawl_spinbox, "text_crawl_spinbox");
  loader.assign(ui_chat_ratelimit_spinbox, "chat_ratelimit_spinbox");
  loader.assign(ui_username_textbox, "username_textbox");
  loader.assign(ui_showname_cb, "showname_cb");
  loader.assign(ui_default_showname_textbox, "default_showname_textbox");
  loader.assign(ui_ms_textbox, "ms_textbox");
  loader.assign(ui_discord_cb, "discord_cb");
  loader.assign(ui_language_combobox, "language_combobox");
  loader.assign(ui_resize_combobox, "resize_combobox");
  loader.assign(ui_shake_cb, "shake_cb");
  loader.assign(ui_effects_cb, "effects_cb");
  loader.assign(ui_framenetwork_cb, "framenetwork_cb");
  loader.assign(ui_colorlog_cb, "colorlog_cb");
  loader.assign(ui_stickysounds_cb, "stickysounds_cb");
  loader.assign(ui_stickyeffects_cb, "stickyeffects_cb");
  loader.assign(ui_stickypres_cb, "stickypres_cb");
  loader.assign(ui_customchat_cb, "customchat_cb");
  loader.assign(ui_continuous_cb, "continuous_cb");
  loader.assign(ui_category_stop_cb, "category_stop_cb");
  loader.assign(ui_sfx_on_idle_cb, "sfx_on_idle_cb");
  loader.assign(ui_evidence_double_click_cb, "evidence_double_click_cb");
  loader.assign(ui_slides_cb, "slides_cb");
  loader.assign(ui_restoreposition_cb, "restoreposition_cb");
  loader.assign(ui_playerlist_format_edit, "playerlist_format_edit");

  registerOption<QSpinBox, int>("theme_scaling_factor_sb", &Options::themeScalingFactor, &Options::setThemeScalingFactor);
  registerOption<QCheckBox, bool>("animated_theme_cb", &Options::animatedThemeEnabled, &Options::setAnimatedThemeEnabled);
  registerOption<QSpinBox, int>("stay_time_spinbox", &Options::textStayTime, &Options::setTextStayTime);
  registerOption<QCheckBox, bool>("instant_objection_cb", &Options::objectionSkipQueueEnabled, &Options::setObjectionSkipQueueEnabled);
  registerOption<QSpinBox, int>("text_crawl_spinbox", &Options::textCrawlSpeed, &Options::setTextCrawlSpeed);
  registerOption<QSpinBox, int>("chat_ratelimit_spinbox", &Options::chatRateLimit, &Options::setChatRateLimit);
  registerOption<QLineEdit, QString>("username_textbox", &Options::username, &Options::setUsername);
  registerOption<QCheckBox, bool>("showname_cb", &Options::customShownameEnabled, &Options::setCustomShownameEnabled);
  registerOption<QLineEdit, QString>("default_showname_textbox", &Options::shownameOnJoin, &Options::setShownameOnJoin);
  registerOption<QLineEdit, QString>("ms_textbox", &Options::alternativeMasterserver, &Options::setAlternativeMasterserver);
  registerOption<QCheckBox, bool>("discord_cb", &Options::discordEnabled, &Options::setDiscordEnabled);
  registerOption<QComboBox, QString>("language_combobox", &Options::language, &Options::setLanguage);

  ui_language_combobox->addItem("English", "en");
  ui_language_combobox->addItem("Deutsch", "de");
  ui_language_combobox->addItem("Español", "es");
  ui_language_combobox->addItem("Português", "pt");
  ui_language_combobox->addItem("Polski", "pl");
  ui_language_combobox->addItem("日本語", "jp");
  ui_language_combobox->addItem("Русский", "ru");

  registerOption<QComboBox, RESIZE_MODE>("resize_combobox", &Options::resizeMode, &Options::setResizeMode);
  registerOption<QCheckBox, bool>("shake_cb", &Options::shakeEnabled, &Options::setShakeEnabled);
  registerOption<QCheckBox, bool>("effects_cb", &Options::effectsEnabled, &Options::setEffectsEnabled);
  registerOption<QCheckBox, bool>("framenetwork_cb", &Options::networkedFrameSfxEnabled, &Options::setNetworkedFrameSfxEnabled);
  registerOption<QCheckBox, bool>("colorlog_cb", &Options::colorLogEnabled, &Options::setColorLogEnabled);
  registerOption<QCheckBox, bool>("stickysounds_cb", &Options::clearSoundsDropdownOnPlayEnabled, &Options::setClearSoundsDropdownOnPlayEnabled);
  registerOption<QCheckBox, bool>("stickyeffects_cb", &Options::clearEffectsDropdownOnPlayEnabled, &Options::setClearEffectsDropdownOnPlayEnabled);
  registerOption<QCheckBox, bool>("stickypres_cb", &Options::clearPreOnPlayEnabled, &Options::setClearPreOnPlayEnabled);
  registerOption<QCheckBox, bool>("customchat_cb", &Options::customChatboxEnabled, &Options::setCustomChatboxEnabled);
  registerOption<QCheckBox, bool>("continuous_cb", &Options::continuousPlaybackEnabled, &Options::setContinuousPlaybackEnabled);
  registerOption<QCheckBox, bool>("category_stop_cb", &Options::stopMusicOnCategoryEnabled, &Options::setStopMusicOnCategoryEnabled);
  registerOption<QCheckBox, bool>("sfx_on_idle_cb", &Options::playSelectedSFXOnIdle, &Options::setPlaySelectedSFXOnIdle);
  registerOption<QCheckBox, bool>("evidence_double_click_cb", &Options::evidenceDoubleClickEdit, &Options::setEvidenceDoubleClickEdit);
  registerOption<QCheckBox, bool>("slides_cb", &Options::slidesEnabled, &Options::setSlidesEnabled);
  registerOption<QCheckBox, bool>("restoreposition_cb", &Options::restoreWindowPositionEnabled, &Options::setRestoreWindowPositionEnabled);
  registerOption<QLineEdit, QString>("playerlist_format_edit", &Options::playerlistFormatString, &Options::setPlayerlistFormatString);

  // Callwords tab. This could just be a QLineEdit, but no, we decided to allow
  // people to put a billion entries in.
  loader.assign(ui_callwords_textbox, "callwords_textbox");
  registerOption<QPlainTextEdit, QStringList>("callwords_textbox", &Options::callwords, &Options::setCallwords);

  // Audio tab.
  loader.assign(ui_audio_device_combobox, "audio_device_combobox");
  populateAudioDevices();
  registerOption<QComboBox, QString>("audio_device_combobox", &Options::audioOutputDevice, &Options::setAudioOutputDevice);

  loader.assign(ui_suppress_audio_spinbox, "suppress_audio_spinbox");
  loader.assign(ui_bliprate_spinbox, "bliprate_spinbox");
  loader.assign(ui_blank_blips_cb, "blank_blips_cb");
  loader.assign(ui_loopsfx_cb, "loopsfx_cb");
  loader.assign(ui_objectmusic_cb, "objectmusic_cb");
  loader.assign(ui_disablestreams_cb, "disablestreams_cb");

  registerOption<QSpinBox, int>("suppress_audio_spinbox", &::Options::defaultSuppressAudio, &Options::setDefaultSupressedAudio);
  registerOption<QSpinBox, int>("bliprate_spinbox", &::Options::blipRate, &Options::setBlipRate);
  registerOption<QCheckBox, bool>("blank_blips_cb", &Options::blankBlip, &Options::setBlankBlip);
  registerOption<QCheckBox, bool>("loopsfx_cb", &Options::loopingSfx, &Options::setLoopingSfx);
  registerOption<QCheckBox, bool>("objectmusic_cb", &Options::objectionStopMusic, &Options::setObjectionStopMusic);
  registerOption<QCheckBox, bool>("disablestreams_cb", &Options::streamingEnabled, &Options::setStreamingEnabled);

  // Asset tab
  loader.assign(ui_mount_list, "mount_list");
  auto *defaultMount = new QListWidgetItem(tr("%1 (default)").arg(kal::AssetPathResolver::applicationAssetPath()));
  defaultMount->setFlags(Qt::ItemFlag::NoItemFlags);
  ui_mount_list->addItem(defaultMount);
  registerOption<QListWidget, QStringList>("mount_list", &Options::mountPaths, &Options::setMountPaths);

  loader.assign(ui_mount_add, "mount_add");
  connect(ui_mount_add, &QPushButton::clicked, this, [this] {
    QString path = QFileDialog::getExistingDirectory(this, tr("Select a base folder"), kal::AssetPathResolver::applicationPath(), QFileDialog::ShowDirsOnly);
    if (path.isEmpty())
    {
      return;
    }
    QDir dir(kal::AssetPathResolver::applicationPath());
    QString relative = dir.relativeFilePath(path);
    if (!relative.contains("../"))
    {
      path = relative;
    }
    QListWidgetItem *dir_item = new QListWidgetItem(path);
    ui_mount_list->addItem(dir_item);
    ui_mount_list->setCurrentItem(dir_item);

    // quick hack to update buttons
    Q_EMIT ui_mount_list->itemSelectionChanged();
  });

  loader.assign(ui_mount_remove, "mount_remove");
  connect(ui_mount_remove, &QPushButton::clicked, this, [this] {
    auto selected = ui_mount_list->selectedItems();
    if (selected.isEmpty())
    {
      return;
    }
    delete selected[0];
    Q_EMIT ui_mount_list->itemSelectionChanged();
    asset_cache_dirty = true;
  });

  loader.assign(ui_mount_up, "mount_up");
  connect(ui_mount_up, &QPushButton::clicked, this, [this] {
    auto selected = ui_mount_list->selectedItems();
    if (selected.isEmpty())
    {
      return;
    }
    auto *item = selected[0];
    int row = ui_mount_list->row(item);
    ui_mount_list->takeItem(row);
    int new_row = qMax(1, row - 1);
    ui_mount_list->insertItem(new_row, item);
    ui_mount_list->setCurrentRow(new_row);
    asset_cache_dirty = true;
  });

  loader.assign(ui_mount_down, "mount_down");
  connect(ui_mount_down, &QPushButton::clicked, this, [this] {
    auto selected = ui_mount_list->selectedItems();
    if (selected.isEmpty())
    {
      return;
    }
    auto *item = selected[0];
    int row = ui_mount_list->row(item);
    ui_mount_list->takeItem(row);
    int new_row = qMin(ui_mount_list->count() + 1, row + 1);
    ui_mount_list->insertItem(new_row, item);
    ui_mount_list->setCurrentRow(new_row);
    asset_cache_dirty = true;
  });

  loader.assign(ui_mount_clear_cache, "mount_clear_cache");
  connect(ui_mount_clear_cache, &QPushButton::clicked, this, [this] {
    asset_cache_dirty = true;
    ui_mount_clear_cache->setEnabled(false);
  });

  connect(ui_mount_list, &QListWidget::itemSelectionChanged, this, [this] {
    auto selected_items = ui_mount_list->selectedItems();
    bool row_selected = !ui_mount_list->selectedItems().isEmpty();
    ui_mount_remove->setEnabled(row_selected);
    ui_mount_up->setEnabled(row_selected);
    ui_mount_down->setEnabled(row_selected);

    if (!row_selected)
    {
      return;
    }

    int row = ui_mount_list->row(selected_items[0]);
    if (row <= 1)
    {
      ui_mount_up->setEnabled(false);
    }
    if (row >= ui_mount_list->count() - 1)
    {
      ui_mount_down->setEnabled(false);
    }
  });

  // Logging tab
  loader.assign(ui_downwards_cb, "downwards_cb");
  loader.assign(ui_length_spinbox, "length_spinbox");
  loader.assign(ui_log_newline_cb, "log_newline_cb");
  loader.assign(ui_log_margin_spinbox, "log_margin_spinbox");
  loader.assign(ui_log_timestamp_format_lbl, "log_timestamp_format_lbl");
  loader.assign(ui_log_timestamp_format_combobox, "log_timestamp_format_combobox");

  registerOption<QCheckBox, bool>("downwards_cb", &Options::logDirectionDownwards, &Options::setLogDirectionDownwards);
  registerOption<QSpinBox, int>("length_spinbox", &Options::maxLogSize, &Options::setMaxLogSize);
  registerOption<QCheckBox, bool>("log_newline_cb", &Options::logNewline, &Options::setLogNewline);
  registerOption<QSpinBox, int>("log_margin_spinbox", &Options::logMargin, &Options::setLogMargin);

  loader.assign(ui_log_timestamp_cb, "log_timestamp_cb");
  registerOption<QCheckBox, bool>("log_timestamp_cb", &Options::logTimestampEnabled, &Options::setLogTimestampEnabled);
  connect(ui_log_timestamp_cb, &QCheckBox::stateChanged, this, &::OptionsWindow::timestampCbChanged);
  ui_log_timestamp_format_lbl->setText(tr("Log timestamp format:\n") + QDateTime::currentDateTime().toString(options.logTimestampFormat()));

  loader.assign(ui_log_timestamp_format_combobox, "log_timestamp_format_combobox");
  registerOption<QComboBox, QString>("log_timestamp_format_combobox", &Options::logTimestampFormat, &Options::setLogTimestampFormat);
  connect(ui_log_timestamp_format_combobox, &QComboBox::currentTextChanged, this, &::OptionsWindow::onTimestampFormatEdited);

  QString l_current_format = options.logTimestampFormat();

  ui_log_timestamp_format_combobox->setCurrentText(l_current_format);

  ui_log_timestamp_format_combobox->addItem(l_current_format, l_current_format);
  ui_log_timestamp_format_combobox->addItem("h:mm:ss AP", "h:mm:ss AP");
  ui_log_timestamp_format_combobox->addItem("hh:mm:ss", "hh:mm:ss");
  ui_log_timestamp_format_combobox->addItem("h:mm AP", "h:mm AP");
  ui_log_timestamp_format_combobox->addItem("hh:mm", "hh:mm");

  if (!options.logTimestampEnabled())
  {
    ui_log_timestamp_format_combobox->setDisabled(true);
  }

  loader.assign(ui_log_ic_actions_cb, "log_ic_actions_cb");
  loader.assign(ui_desync_logs_cb, "desync_logs_cb");
  loader.assign(ui_log_text_cb, "log_text_cb");

  registerOption<QCheckBox, bool>("log_ic_actions_cb", &Options::logIcActions, &Options::setLogIcActions);
  registerOption<QCheckBox, bool>("desync_logs_cb", &Options::desynchronisedLogsEnabled, &Options::setDesynchronisedLogsEnabled);
  registerOption<QCheckBox, bool>("log_text_cb", &Options::logToTextFileEnabled, &Options::setLogToTextFileEnabled);

  // DSGVO/Privacy tab

  loader.assign(ui_privacy_policy, "privacy_policy");
  ui_privacy_policy->setPlainText(tr("Getting privacy policy..."));
  loader.assign(ui_privacy_optout_cb, "privacy_optout_cb");
  registerOption<QCheckBox, bool>("privacy_optout", &Options::playerCountOptout, &Options::setPlayerCountOptout);

  updateValues();
}

void OptionsWindow::onTimestampFormatEdited()
{
  const QString format = ui_log_timestamp_format_combobox->currentText();
  const int index = ui_log_timestamp_format_combobox->currentIndex();

  ui_log_timestamp_format_combobox->setItemText(index, format);
  ui_log_timestamp_format_combobox->setItemData(index, format);
  ui_log_timestamp_format_lbl->setText(tr("Log timestamp format:\n") + QDateTime::currentDateTime().toString(format));
}

void OptionsWindow::timestampCbChanged(int state)
{
  ui_log_timestamp_format_combobox->setDisabled(state == 0);
}

#if (defined(_WIN32) || defined(_WIN64))
bool OptionsWindow::needsDefaultAudioDevice()
{
  return true;
}
#elif (defined(LINUX) || defined(__linux__))
bool OptionsWindow::needsDefaultAudioDevice()
{
  return false;
}
#elif defined __APPLE__
bool OptionsWindow::needsDefaultAudioDevice()
{
  return true;
}
#else
#error This operating system is not supported.
#endif
