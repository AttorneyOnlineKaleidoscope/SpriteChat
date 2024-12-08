#include "animationlayer.h"

#include "aoapplication.h"
#include "options.h"
#include "spritechatcommon.h"

#include <QRectF>
#include <QThreadPool>

static QThreadPool *thread_pool;

namespace kal
{
AnimationLayer::AnimationLayer(QWidget *parent)
    : QLabel(parent)
{
  setAlignment(Qt::AlignCenter);

  m_ticker = new QTimer(this);
  m_ticker->setSingleShot(true);
  m_ticker->setTimerType(Qt::PreciseTimer);

  connect(m_ticker, &QTimer::timeout, this, &AnimationLayer::frameTicker);

  if (!thread_pool)
  {
    thread_pool = new QThreadPool(qApp);
    thread_pool->setMaxThreadCount(8);
  }

  createLoader();
}

AnimationLayer::~AnimationLayer()
{
  deleteLoader();
}

QString AnimationLayer::fileName()
{
  return m_file_name;
}

void AnimationLayer::setFileName(QString fileName)
{
  stopPlayback();
  m_file_name = fileName;
  if (m_file_name.trimmed().isEmpty())
  {
#ifdef DEBUG_MOVIE
    kWarning() << "AnimationLayer::setFileName called with empty string";
#endif
    m_file_name = QObject::tr("Invalid File");
  }
  resetData();
}

void AnimationLayer::startPlayback()
{
  if (m_processing)
  {
#ifdef DEBUG_MOVIE
    kWarning() << "AnimationLayer::startPlayback called while already processing";
#endif
    return;
  }
  resetData();
  m_processing = true;
  setVisible(true);
  Q_EMIT startedPlayback();
  frameTicker();
}

void AnimationLayer::stopPlayback()
{
  if (m_ticker->isActive())
  {
    m_ticker->stop();
  }
  m_processing = false;
  if (m_reset_cache_when_stopped)
  {
    createLoader();
  }
  Q_EMIT stoppedPlayback();
}

void AnimationLayer::restartPlayback()
{
  stopPlayback();
  startPlayback();
}

void AnimationLayer::pausePlayback(bool enabled)
{
  if (m_pause == enabled)
  {
#ifdef DEBUG_MOVIE
    kWarning() << "AnimationLayer::pausePlayback called with identical state";
#endif
    return;
  }
  m_pause = enabled;
}

QSize AnimationLayer::frameSize()
{
  return m_frame_size;
}

int AnimationLayer::frameCount()
{
  return m_frame_count;
}

int AnimationLayer::currentFrameNumber()
{
  return m_frame_number;
}

/**
 * @brief AnimationLayer::jumpToFrame
 * @param number The frame number to jump to. Must be in valid range. If the number is out of range, the method does nothing.
 * @details If frame number is valid and playback is processing, the frame will immediately be displayed.
 */
void AnimationLayer::jumpToFrame(int number)
{
  if (number < 0 || number >= m_frame_count)
  {
#ifdef DEBUG_MOVIE
    kWarning() << "AnimationLayer::jumpToFrame failed to jump to frame" << number << "(file:" << m_file_name << ", frame count:" << m_frame_count << ")";
#endif
    return;
  }

  bool is_processing = m_processing;
  if (m_ticker->isActive())
  {
    m_ticker->stop();
  }
  m_target_frame_number = number;
  if (is_processing)
  {
    frameTicker();
  }
}

bool AnimationLayer::isPlayOnce()
{
  return m_play_once;
}

void AnimationLayer::setPlayOnce(bool enabled)
{
  m_play_once = enabled;
}

void AnimationLayer::setStretchToFit(bool enabled)
{
  m_stretch_to_fit = enabled;
}

void AnimationLayer::setResetCacheWhenStopped(bool enabled)
{
  m_reset_cache_when_stopped = enabled;
}

void AnimationLayer::setFlipped(bool enabled)
{
  m_flipped = enabled;
}

void AnimationLayer::setResizeMode(RESIZE_MODE mode)
{
  m_resize_mode = mode;
}

void AnimationLayer::setMinimumDurationPerFrame(int duration)
{
  m_minimum_duration = duration;
}

void AnimationLayer::setMaximumDurationPerFrame(int duration)
{
  m_maximum_duration = duration;
}

void AnimationLayer::setMaskingRect(QRect rect)
{
  m_mask_rect_hint = rect;
  calculateFrameGeometry();
}

void AnimationLayer::resizeEvent(QResizeEvent *event)
{
  QLabel::resizeEvent(event);
  calculateFrameGeometry();
}

void AnimationLayer::createLoader()
{
  deleteLoader();
  m_loader = new AnimationLoader(thread_pool);
}

void AnimationLayer::deleteLoader()
{
  if (m_loader)
  {
    delete m_loader;
    m_loader = nullptr;
  }
}

void AnimationLayer::resetData()
{
  m_first_frame = true;
  m_frame_number = 0;
  if (m_file_name != m_loader->loadedFileName())
  {
    m_loader->load(m_file_name);
  }
  m_frame_count = m_loader->frameCount();
  m_frame_size = m_loader->size();
  m_frame_rect = QRect(QPoint(0, 0), m_frame_size);
  m_ticker->stop();
  calculateFrameGeometry();
}

void AnimationLayer::calculateFrameGeometry()
{
  m_mask_rect = QRect();
  m_scaled_frame_size = QSize();
  m_transformation_mode = Qt::SmoothTransformation;

  QSize widget_size = size();
  if (!widget_size.isValid() || !m_frame_size.isValid())
  {
    return;
  }

  if (m_stretch_to_fit)
  {
    m_scaled_frame_size = widget_size;
  }
  else
  {
    m_scaled_frame_size = m_frame_size;
    if (m_frame_rect.contains(m_mask_rect_hint))
    {
      m_mask_rect = m_mask_rect_hint;
      m_scaled_frame_size = m_mask_rect_hint.size();
    }

    double scale = double(widget_size.height()) / double(m_scaled_frame_size.height());
    m_scaled_frame_size *= scale;

    if (m_frame_size.height() < widget_size.height())
    {
      m_transformation_mode = Qt::FastTransformation;
    }
  }

  if (m_resize_mode == PIXEL_RESIZE_MODE)
  {
    m_transformation_mode = Qt::FastTransformation;
  }
  else if (m_resize_mode == SMOOTH_RESIZE_MODE)
  {
    m_transformation_mode = Qt::SmoothTransformation;
  }

  displayCurrentFrame();
}

void AnimationLayer::finishPlayback()
{
  stopPlayback();
  Q_EMIT finishedPlayback();
}

void AnimationLayer::prepareNextTick()
{
  int duration = qMax(m_minimum_duration, m_current_frame.duration);
  duration = (m_maximum_duration > 0) ? qMin(m_maximum_duration, duration) : duration;
  m_ticker->start(duration);
}

void AnimationLayer::displayCurrentFrame()
{
  QPixmap image = m_current_frame.texture;

  if (m_frame_size.isValid())
  {
    if (m_mask_rect.isValid())
    {
      image = image.copy(m_mask_rect);
    }

    if (!image.isNull())
    {
      image = image.scaled(m_scaled_frame_size, Qt::IgnoreAspectRatio, m_transformation_mode);

      if (m_flipped)
      {
        image = image.transformed(QTransform().scale(-1.0, 1.0));
      }
    }
  }
  else
  {
    image = QPixmap(1, 1);
    image.fill(Qt::transparent);
  }

  setPixmap(image);
}

void AnimationLayer::frameTicker()
{
  if (!m_processing)
  {
    return;
  }

  if (m_frame_count < 1)
  {
    if (m_play_once)
    {
      finishPlayback();
      return;
    }

    stopPlayback();
    return;
  }

  if (m_pause && !m_first_frame)
  {
    return;
  }

  if (m_frame_number == m_frame_count)
  {
    if (m_play_once)
    {
      finishPlayback();
      return;
    }

    if (m_frame_count > 1)
    {
      m_frame_number = 0;
    }
    else
    {
      return;
    }
  }

  m_first_frame = false;
  if (m_target_frame_number != -1)
  {
    m_frame_number = m_target_frame_number;
    m_target_frame_number = -1;
  }
  m_current_frame = m_loader->frame(m_frame_number);
  displayCurrentFrame();
  Q_EMIT frameNumberChanged(m_frame_number);
  ++m_frame_number;

  if (!m_pause)
  {
    prepareNextTick();
  }
}

CharacterAnimationLayer::CharacterAnimationLayer(AOApplication &ao_app, AssetPathResolver &assetPathResolver, QWidget *parent)
    : AnimationLayer(parent)
    , ao_app(ao_app)
    , m_resolver(assetPathResolver)
{
  m_duration_timer = new QTimer(this);
  m_duration_timer->setSingleShot(true);
  connect(m_duration_timer, &QTimer::timeout, this, &CharacterAnimationLayer::onDurationLimitReached);

  connect(this, &CharacterAnimationLayer::stoppedPlayback, this, &CharacterAnimationLayer::onPlaybackStopped);
  connect(this, &CharacterAnimationLayer::frameNumberChanged, this, &CharacterAnimationLayer::notifyFrameEffect);
  connect(this, &CharacterAnimationLayer::finishedPlayback, this, &CharacterAnimationLayer::notifyEmotePlaybackFinished);
}

void CharacterAnimationLayer::loadCharacterEmote(QString character, QString fileName, kal::EmoteType emoteType, int durationLimit)
{
  auto is_dialog_emote = [](kal::EmoteType emoteType) {
    return emoteType == kal::IdleEmote || emoteType == kal::TalkEmote;
  };

  bool synchronize_frame = false;
  const int previous_frame_count = frameCount();
  const int previous_frame_number = currentFrameNumber();
  if (m_character == character && m_emote == fileName && is_dialog_emote(m_emote_type) && is_dialog_emote(emoteType))
  {
    synchronize_frame = true;
  }

  m_character = character;
  m_emote = fileName;
  m_resolved_emote = fileName;
  m_emote_type = emoteType;

  QStringList prefixes;
  bool placeholder_fallback = false;
  bool play_once = false;
  switch (emoteType)
  {
  default:
    break;

  case kal::PreEmote:
    play_once = true;
    break;

  case kal::IdleEmote:
    prefixes << QStringLiteral("(a)") << QStringLiteral("(a)/");
    placeholder_fallback = true;
    break;

  case kal::TalkEmote:
    prefixes << QStringLiteral("(b)") << QStringLiteral("(b)/");
    placeholder_fallback = true;
    break;

  case kal::PostEmote:
    prefixes << QStringLiteral("(c)") << QStringLiteral("(c)/");
    break;
  }

  int index = -1;
  kal::MaybePath path = [&]() -> kal::MaybePath {
    for (int i = 0; i < prefixes.size(); ++i)
    {
      if (kal::MaybePath path = m_resolver.characterFilePath(character, prefixes.at(i) % fileName, kal::AnimatedImageAssetType))
      {
        index = i;
        return path;
      }
    }

    if (kal::MaybePath path = m_resolver.characterFilePath(character, fileName, kal::AnimatedImageAssetType))
    {
      return path;
    }

    if (placeholder_fallback)
    {
      return m_resolver.currentThemeFilePath(QStringLiteral("placeholder"), kal::ImageAssetType);
    }

    return kal::MaybePath();
  }();

  if (index != -1)
  {
    m_resolved_emote = prefixes.at(index) % m_emote;
  }

  setFileName(path.value_or(QString()));
  setPlayOnce(play_once);
  setResizeMode(ao_app.get_scaling(ao_app.get_emote_property(character, fileName, "scaling")));
  setStretchToFit(ao_app.get_emote_property(character, fileName, "stretch").startsWith("true"));
  if (synchronize_frame && previous_frame_count == frameCount())
  {
    jumpToFrame(previous_frame_number);
  }
  m_duration = durationLimit;
}

void CharacterAnimationLayer::setFrameEffects(QStringList data)
{
  m_effects.clear();

  static const QList<EffectType> EFFECT_TYPE_LIST{ShakeEffect, FlashEffect, SfxEffect};
  for (int i = 0; i < data.length(); ++i)
  {
    const EffectType effect_type = EFFECT_TYPE_LIST.at(i);

    QStringList emotes = data.at(i).split("^");
    for (const QString &emote : qAsConst(emotes))
    {
      QStringList emote_effects = emote.split("|");

      const QString emote_name = emote_effects.takeFirst();

      for (const QString &raw_effect : qAsConst(emote_effects))
      {
        QStringList frame_data = raw_effect.split("=");

        const int frame_number = frame_data.at(0).toInt();

        FrameEffect effect;
        effect.emote_name = emote_name;
        effect.type = effect_type;
        if (effect_type == EffectType::SfxEffect)
        {
          effect.file_name = frame_data.at(1);
        }

        m_effects[frame_number].append(effect);
      }
    }
  }
}

void CharacterAnimationLayer::startTimeLimit()
{
  if (m_duration > 0)
  {
    m_duration_timer->start(m_duration);
  }
}

void CharacterAnimationLayer::onPlaybackStopped()
{
  if (m_duration_timer->isActive())
  {
    m_duration_timer->stop();
  }
}

void CharacterAnimationLayer::notifyEmotePlaybackFinished()
{
  if (m_emote_type == kal::PreEmote || m_emote_type == kal::PostEmote)
  {
    Q_EMIT finishedPreOrPostEmotePlayback();
  }
}

void CharacterAnimationLayer::onPlaybackFinished()
{
  if (m_emote_type == kal::PreEmote || m_emote_type == kal::PostEmote)
  {
    if (m_duration_timer->isActive())
    {
      m_duration_timer->stop();
    }

    notifyEmotePlaybackFinished();
  }
}

void CharacterAnimationLayer::onDurationLimitReached()
{
  stopPlayback();
  notifyEmotePlaybackFinished();
}

void CharacterAnimationLayer::notifyFrameEffect(int frameNumber)
{
  auto it = m_effects.constFind(frameNumber);
  if (it != m_effects.constEnd())
  {
    for (const FrameEffect &effect : qAsConst(*it))
    {
      if (effect.emote_name == m_resolved_emote)
      {
        switch (effect.type)
        {
        default:
          break;

        case EffectType::SfxEffect:
          Q_EMIT soundEffect(effect.file_name);
          break;

        case EffectType::ShakeEffect:
          Q_EMIT shakeEffect();
          break;

        case EffectType::FlashEffect:
          Q_EMIT flashEffect();
          break;
        }
      }
    }
  }
}

BackgroundAnimationLayer::BackgroundAnimationLayer(AOApplication &ao_app, AssetPathResolver &assetPathResolver, QWidget *parent)
    : AnimationLayer(parent)
    , ao_app(ao_app)
    , m_resolver(assetPathResolver)
{}

void BackgroundAnimationLayer::loadAndPlayAnimation(const QString &background, QString fileName)
{
  kal::MaybePath file_path = m_resolver.backgroundFilePath(background, fileName, kal::AnimatedImageAssetType);
  if (file_path)
  {
    if (file_path == this->fileName())
    {
      return;
    }

    kInfo() << "[BackgroundLayer] Loading background" << file_path.value();
  }
  else
  {
    kWarning() << "[BackgroundLayer] Failed to load background" << background << fileName;
  }

  QString design_path;
  if (auto path = m_resolver.backgroundFilePath(background, "design.ini"))
  {
    design_path = path.value();
  }
  else
  {
    kWarning() << "[BackgroundLayer] Failed to load design.ini for background" << background;
  }
  setResizeMode(ao_app.get_scaling(ao_app.read_design_ini("scaling", design_path)));
  setStretchToFit(ao_app.read_design_ini("stretch", design_path).startsWith("true"));
  startPlayback();
}

SplashAnimationLayer::SplashAnimationLayer(AOApplication &ao_app, kal::AssetPathResolver &assetPathResolver, QWidget *parent)
    : AnimationLayer(parent)
    , ao_app(ao_app)
    , m_resolver(assetPathResolver)
{
  connect(this, &SplashAnimationLayer::startedPlayback, this, &SplashAnimationLayer::show);
  connect(this, &SplashAnimationLayer::stoppedPlayback, this, &SplashAnimationLayer::hide);
}

void SplashAnimationLayer::loadAndPlayAnimation(QString p_filename, QString p_charname, QString p_miscname)
{
  setFileName(m_resolver.currentThemeFilePath(p_filename, kal::AnimatedImageAssetType).value_or(QString()));
  setResizeMode(ao_app.get_chatbox_scaling(p_miscname));
  startPlayback();
}

EffectAnimationLayer::EffectAnimationLayer(AOApplication &ao_app, QWidget *parent)
    : AnimationLayer(parent)
    , ao_app(ao_app)
{
  connect(this, &EffectAnimationLayer::startedPlayback, this, &EffectAnimationLayer::show);
  connect(this, &EffectAnimationLayer::stoppedPlayback, this, &EffectAnimationLayer::maybeHide);
}

void EffectAnimationLayer::loadAndPlayAnimation(QString p_filename, bool repeat)
{
  setFileName(p_filename);
  setPlayOnce(!repeat);
  startPlayback();
}

void EffectAnimationLayer::setHideWhenStopped(bool enabled)
{
  m_hide_when_stopped = enabled;
}

void EffectAnimationLayer::maybeHide()
{
  if (m_hide_when_stopped && isPlayOnce())
  {
    hide();
  }
}

InterfaceAnimationLayer::InterfaceAnimationLayer(AOApplication &ao_app, kal::AssetPathResolver &assetPathResolver, QWidget *parent)
    : AnimationLayer(parent)
    , ao_app(ao_app)
    , m_resolver(assetPathResolver)
{
  setStretchToFit(true);

  connect(this, &InterfaceAnimationLayer::startedPlayback, this, &InterfaceAnimationLayer::show);
  connect(this, &InterfaceAnimationLayer::stoppedPlayback, this, &InterfaceAnimationLayer::hide);
}

void InterfaceAnimationLayer::loadAndPlayAnimation(QString fileName, QString miscName)
{
  setFileName(m_resolver.currentThemeFilePath(fileName, kal::AnimatedImageAssetType).value_or(QString()));
  startPlayback();
}
} // namespace kal
