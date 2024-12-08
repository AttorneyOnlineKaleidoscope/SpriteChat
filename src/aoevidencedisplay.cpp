#include "aoevidencedisplay.h"

#include "datatypes.h"

AOEvidenceDisplay::AOEvidenceDisplay(AOApplication &p_ao_app, kal::AssetPathResolver &assetPathResolver, QWidget *p_parent)
    : QLabel(p_parent)
    , ao_app(p_ao_app)
    , m_resolver(assetPathResolver)
{
  ui_prompt_details = new QPushButton(this);
  ui_prompt_details->hide();

  m_sfx_player = new AOSfxPlayer(ao_app, m_resolver);

  m_evidence_movie = new kal::InterfaceAnimationLayer(ao_app, m_resolver, this);

  connect(m_evidence_movie, &kal::InterfaceAnimationLayer::finishedPlayback, this, &AOEvidenceDisplay::show_done);
  connect(ui_prompt_details, &QPushButton::clicked, this, &AOEvidenceDisplay::icon_clicked);
}

void AOEvidenceDisplay::show_evidence(int p_index, QString p_evidence_image, bool is_left_side, int p_volume)
{
  QString gif_name;
  QString icon_identifier;

  m_sfx_player->setVolume(p_volume);

  if (is_left_side)
  {
    icon_identifier = "left_evidence_icon";
    gif_name = "evidence_appear_left";
  }
  else
  {
    icon_identifier = "right_evidence_icon";
    gif_name = "evidence_appear_right";
  }

  QPixmap pixmap;
  if (auto path = m_resolver.evidenceFilePath(p_evidence_image))
  {
    pixmap.load(path.value());
  }

  pos_size_type icon_dimensions = ao_app.get_element_dimensions(icon_identifier, "courtroom_design.ini");
  pixmap = pixmap.scaled(icon_dimensions.width, icon_dimensions.height);

  ui_prompt_details->setIcon(QIcon(pixmap));
  ui_prompt_details->setIconSize(pixmap.size());
  ui_prompt_details->resize(pixmap.size());
  ui_prompt_details->move(icon_dimensions.x, icon_dimensions.y);

  if (m_last_evidence_index != p_index)
  {
    this->reset();
    m_last_evidence_index = p_index;

    m_evidence_movie->setPlayOnce(true);
    m_evidence_movie->loadAndPlayAnimation(gif_name, "");
    m_sfx_player->findAndPlaySfx(ao_app.get_court_sfx("evidence_present"));
  }
  else
  {
    ui_prompt_details->show();
  }
}

void AOEvidenceDisplay::reset()
{
  m_sfx_player->stop();
  m_evidence_movie->stopPlayback();
  ui_prompt_details->hide();
  this->clear();
}

void AOEvidenceDisplay::show_done()
{
  ui_prompt_details->show();
}

void AOEvidenceDisplay::icon_clicked()
{
  if (m_last_evidence_index != -1)
  {
    Q_EMIT show_evidence_details(m_last_evidence_index - 1); // i dont know why i have to subtract 1 here
  }
}

void AOEvidenceDisplay::combo_resize(int w, int h)
{
  QSize f_size(w, h);
  this->resize(f_size);
  m_evidence_movie->resize(w, h);
}

void AOEvidenceDisplay::setLastEvidenceIndex(int f_index)
{
  m_last_evidence_index = f_index;
}
