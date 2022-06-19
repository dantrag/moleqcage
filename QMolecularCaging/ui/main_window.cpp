#include "ui/main_window.h"

#include <Qt3DCore/QTransform>
#include <Qt3DInput/QInputAspect>
#include <Qt3DExtras/QOrbitCameraController>
#include <Qt3DExtras/QForwardRenderer>
#include <Qt3DExtras/QPhongMaterial>
#include <Qt3DExtras/QSphereMesh>
#include <Qt3DRender/QCamera>
#include <Qt3DRender/QPointLight>
#include <Qt3DRender/QRenderSettings>
#include <QVector3D>

#include "ui_main_window.h"

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent), ui_(new Ui::MainWindow) {
  ui_->setupUi(this);

  this->setWindowState(Qt::WindowMaximized);

  elements_ = new ElementCollection();
  molecules_ = new MoleculeCollection(elements_);

  connect(ui_->tab_widget, &QCustomTabWidget::currentChanged, this, &MainWindow::CurrentTabChanged);

  file_tab_ = new FileTab(molecules_, elements_, ui_->tab_widget);
  experiment_tab_ = new ExperimentTab(ui_->tab_widget);
  analysis_tab_ = new AnalysisTab(ui_->tab_widget);
  //visualization_tab_ = new VisualizationTab(ui_->tab_widget);
  ui_->tab_widget->addTab(file_tab_, file_tab_caption_);
  ui_->tab_widget->addTab(experiment_tab_, experiment_tab_caption_);
  ui_->tab_widget->addTab(analysis_tab_, analysis_tab_caption_);
  //ui_->tab_widget->addTab(visualization_tab_, visualization_tab_caption_);

  SetupScene(this, scene_widget_, ui_->molecule_view_widget, scene_, scene_root_entity_);

  connect(this, &MainWindow::CurrentMoleculeChanged, this, &MainWindow::UpdatePreviewMolecule);
  connect(experiment_tab_, &ExperimentTab::CurrentMoleculeChanged, this, &MainWindow::UpdatePreviewMolecule);
  connect(file_tab_, &FileTab::RadiusChanged, this, &MainWindow::UpdatePreview);
  connect(experiment_tab_, &ExperimentTab::JobCompleted, analysis_tab_, &AnalysisTab::AddCompletedJob);
  connect(molecules_, &MoleculeCollection::MoleculeLoaded, this, &MainWindow::MoleculeLoaded);
  connect(molecules_, &MoleculeCollection::MoleculeUnloaded, this, &MainWindow::MoleculeUnloaded);
  connect(ui_->molecule_list_widget, &QListWidget::currentItemChanged, this, &MainWindow::CurrentListWidgetItemChanged);
  connect(ui_->molecule_list_widget, &QListWidget::itemChanged, this, &MainWindow::MoleculeListItemChanged);
  connect(ui_->show_full_path_check_box, &QPushButton::clicked, this, &MainWindow::UpdateMoleculeList);
  connect(ui_->add_to_guests_button, &QPushButton::clicked, this, &MainWindow::AddMoleculesToGuests);
  connect(ui_->add_to_hosts_button, &QPushButton::clicked, this, &MainWindow::AddMoleculesToHosts);
  connect(ui_->add_to_scene_button, &QPushButton::clicked, this, &MainWindow::AddMoleculesToScene);

  ui_->main_splitter->setSizes(QList<int>({INT_MAX, INT_MAX}));
  ui_->display_splitter->setSizes(QList<int>({INT_MAX / 3, INT_MAX / 2}));
  //ui_->molecule_splitter->setSizes(QList<int>({INT_MAX, INT_MAX}));
  /*auto sp = ui_->display_widget->sizePolicy();
  sp.setHorizontalStretch(2);
  ui_->display_widget->setSizePolicy(sp);
  sp = ui_->molecule_splitter->sizePolicy();
  sp.setHorizontalStretch(1);
  ui_->molecule_splitter->setSizePolicy(sp);
  sp = ui_->molecule_stats_widget->sizePolicy();
  sp.setHorizontalStretch(1);
  ui_->molecule_stats_widget->setSizePolicy(sp);
  sp = ui_->molecule_view_widget->sizePolicy();
  sp.setHorizontalStretch(1);
  ui_->molecule_view_widget->setSizePolicy(sp);*/
}

void MainWindow::CurrentTabChanged(int index) {
  auto caption = ui_->tab_widget->tabText(index);
  if (caption == visualization_tab_caption_) {
    //
  }

  bool show_experiment_controls = (caption == experiment_tab_caption_);
  bool show_scene_controls = (caption == visualization_tab_caption_);
  ui_->add_to_guests_button->setVisible(show_experiment_controls);
  ui_->add_to_hosts_button->setVisible(show_experiment_controls);
  ui_->add_to_scene_button->setVisible(show_scene_controls);
}

void MainWindow::MoleculeLoaded(QString file_path) {
  ui_->molecule_list_widget->AddCheckableFileItem(file_path, false);

  UpdateMoleculeList();
}

void MainWindow::MoleculeUnloaded(QString file_path) {
  ui_->molecule_list_widget->RemoveItem(file_path);
}

void MainWindow::UpdateMoleculeList() {
  ui_->molecule_list_widget->SetDisplayMode(ui_->show_full_path_check_box->isChecked());
}

void MainWindow::MoleculeListItemChanged() {
  for (int row = 0; row < ui_->molecule_list_widget->count(); ++row) {
    if (ui_->molecule_list_widget->item(row)->checkState() == Qt::Checked) {
      ui_->add_to_guests_button->setEnabled(true);
      ui_->add_to_hosts_button->setEnabled(true);
      ui_->add_to_scene_button->setEnabled(true);
      return;
    }
  }
  ui_->add_to_guests_button->setEnabled(false);
  ui_->add_to_hosts_button->setEnabled(false);
  ui_->add_to_scene_button->setEnabled(false);
}

void MainWindow::CurrentListWidgetItemChanged(QListWidgetItem* current, QListWidgetItem* /*previous*/) {
  emit CurrentMoleculeChanged(current->data(Qt::UserRole).toString());
}

void MainWindow::AddMoleculesToHosts() {
  experiment_tab_->AddHosts(GetCheckedMolecules());
  ClearMoleculeCheckmarks();
}

void MainWindow::AddMoleculesToGuests() {
  experiment_tab_->AddGuests(GetCheckedMolecules());
  ClearMoleculeCheckmarks();
}

void MainWindow::AddMoleculesToScene() {
  visualization_tab_->AddMolecules(GetCheckedMolecules(), molecules_, elements_);
  ClearMoleculeCheckmarks();
}

void MainWindow::ClearMoleculeCheckmarks() {
  for (int row = 0; row < ui_->molecule_list_widget->count(); ++row) {
    ui_->molecule_list_widget->item(row)->setCheckState(Qt::Unchecked);
  }
  ui_->molecule_list_widget->setFocus();
}

QStringList MainWindow::GetCheckedMolecules() {
  QStringList file_paths;
  for (int row = 0; row < ui_->molecule_list_widget->count(); ++row) {
    if (ui_->molecule_list_widget->item(row)->checkState() == Qt::Checked) {
      file_paths.push_back(ui_->molecule_list_widget->item(row)->data(Qt::UserRole).toString());
    }
  }
  return file_paths;
}

void MainWindow::UpdatePreview() {
  UpdatePreviewMolecule("");
}

void MainWindow::UpdatePreviewMolecule(QString file_name) {
  if (file_name.isEmpty() && preview_molecule_ == nullptr) {
    return;
  }
  if (!file_name.isEmpty()) {
    preview_molecule_ = (*molecules_)[file_name];
  }

  if (preview_molecule_entity_ != nullptr) {
    RemoveMoleculeFromScene(preview_molecule_entity_);
  }

  preview_molecule_entity_ = AddMoleculeToScene(scene_root_entity_, preview_molecule_, elements_);

  auto mass_centre = preview_molecule_->MassCentre();
  scene_->camera()->setViewCenter(QVector3D(std::get<0>(mass_centre),
                                            std::get<1>(mass_centre),
                                            std::get<2>(mass_centre)));
}

MainWindow::~MainWindow() {
  delete file_tab_;
  delete scene_;
  delete ui_;
  delete molecules_;
  delete elements_;
}
