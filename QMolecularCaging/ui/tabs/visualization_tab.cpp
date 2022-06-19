#include "ui/tabs/visualization_tab.h"

#include <QFileDialog>
#include <Qt3DRender/QCamera>
#include <Qt3DRender/QRenderSettings>
#include <Qt3DExtras/QPhongMaterial>
#include <Qt3DExtras/QSphereMesh>
#include <QtGui/QVector3D>

#include "ui_visualization_tab.h"

VisualizationTab::VisualizationTab(QWidget *parent)
    : QWidget(parent), ui_(new Ui::VisualizationTab) {
  ui_->setupUi(this);

  SetupScene(this, scene_widget_, ui_->scene_container, scene_, scene_root_entity_);

  scene_render_capture_ = new Qt3DRender::QRenderCapture;
  scene_->activeFrameGraph()->setParent(scene_render_capture_);
  scene_->setActiveFrameGraph(scene_render_capture_);

  UpdateCameraPosition();

  connect(ui_->display_list_widget, &QListWidget::itemChanged, this, &VisualizationTab::DisplayListItemChanged);
  connect(ui_->align_mass_center_check_box, &QCheckBox::toggled, this, &VisualizationTab::UpdateTranslations);
  connect(ui_->save_image_button, &QPushButton::clicked, this, &VisualizationTab::SaveImage);
  connect(ui_->camera_distance_slider, &QSlider::valueChanged, this, &VisualizationTab::UpdateCameraDistanceSpinBox);
  connect(ui_->camera_distance_spin_box, static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged),
          this, &VisualizationTab::UpdateCameraDistanceSlider);
}

VisualizationTab::~VisualizationTab() {
  delete ui_;
}

void VisualizationTab::AddMolecules(QStringList file_paths,
                                    MoleculeCollection* molecules, ElementCollection* elements) {
  for (auto file_path : file_paths) {
    molecule_list_.push_back(file_path);
    molecule_entities_[file_path] = AddMoleculeToScene(scene_root_entity_, (*molecules)[file_path], elements);
    if (ui_->align_mass_center_check_box->isChecked()) {
      auto mass_centre = (*molecules)[file_path]->MassCentre();
      auto translation = QVector3D(std::get<0>(mass_centre),
                                   std::get<1>(mass_centre),
                                   std::get<2>(mass_centre));
      SetMoleculeTranslationOnScene(molecule_entities_[file_path], -translation);
    }
    ui_->display_list_widget->AddCheckableFileItem(file_path);
  }
}

void VisualizationTab::DisplayListItemChanged() {
  for (int row = 0; row < ui_->display_list_widget->count(); ++row) {
    auto item = ui_->display_list_widget->item(row);
    auto filename = item->data(Qt::UserRole).toString();
    ShowMoleculeOnScene(molecule_entities_[filename], item->checkState() == Qt::Checked);
  }
}

void VisualizationTab::UpdateTranslations() {
  for (auto molecule_entity : molecule_entities_) {
    EnableMoleculeTranslationOnScene(molecule_entity.second, ui_->align_mass_center_check_box->isChecked());
  }
}

void VisualizationTab::SaveImage() {
  image_filename = QFileDialog::getSaveFileName(this, "Save preview image to file", "molecule.png",
                                               "PNG (*.png);;JPEG (*.jpg *.jpeg);;BMP (*.bmp);;GIF (*.gif)",
                                               new QString("PNG (*.png)"));
  if (!image_filename.isEmpty()) {
    ui_->save_image_button->setEnabled(false);
    scene_render_capture_reply_ = scene_render_capture_->requestCapture();
    image_rendering_connection =
        QObject::connect(scene_render_capture_reply_, &Qt3DRender::QRenderCaptureReply::completed,
                         this, &VisualizationTab::ImageRenderingCompleted);
    scene_force_updater_ = new SceneForceUpdater(scene_root_entity_);
  }
}

void VisualizationTab::ImageRenderingCompleted() {
  QObject::disconnect(image_rendering_connection);

  scene_render_capture_reply_->saveImage(image_filename);
  image_filename = "";
  ui_->save_image_button->setEnabled(true);

  delete scene_force_updater_;
  delete scene_render_capture_reply_;
  scene_render_capture_reply_ = nullptr;
}

void VisualizationTab::UpdateCameraDistanceSlider() {
  if (ui_->camera_distance_slider->value() != ui_->camera_distance_spin_box->value()) {
    ui_->camera_distance_slider->setValue(ui_->camera_distance_spin_box->value());
    UpdateCameraPosition();
  }
}

void VisualizationTab::UpdateCameraDistanceSpinBox() {
  if (ui_->camera_distance_slider->value() != ui_->camera_distance_spin_box->value()) {
    ui_->camera_distance_spin_box->setValue(ui_->camera_distance_slider->value());
    UpdateCameraPosition();
  }
}

void VisualizationTab::UpdateCameraPosition() {
  auto position = scene_->camera()->position();
  position.normalize();
  scene_->camera()->setPosition(position * ui_->camera_distance_spin_box->value());
}
