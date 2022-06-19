#ifndef UI_TABS_VISUALIZATION_TAB_H
#define UI_TABS_VISUALIZATION_TAB_H

#include <map>

#include <QWidget>
#include <Qt3DCore/QEntity>
#include <Qt3DExtras/Qt3DWindow>
#include <Qt3DRender/QRenderCapture>

#include "chemistry/element_collection.h"
#include "chemistry/molecule_collection.h"
#include "utils/qt_3d_utilities.h"

namespace Ui {
class VisualizationTab;
}

class VisualizationTab : public QWidget
{
  Q_OBJECT

public:
  explicit VisualizationTab(QWidget *parent = 0);
  ~VisualizationTab();

public slots:
  void AddMolecules(QStringList file_paths,
                    MoleculeCollection* molecules, ElementCollection* elements);

private slots:
  void DisplayListItemChanged();
  void UpdateTranslations();
  void SaveImage();
  void ImageRenderingCompleted();
  void UpdateCameraDistanceSlider();
  void UpdateCameraDistanceSpinBox();
  void UpdateCameraPosition();

private:
  Ui::VisualizationTab* ui_ = nullptr;

  QWidget* scene_widget_ = nullptr;
  Qt3DExtras::Qt3DWindow* scene_ = nullptr;
  Qt3DCore::QEntity* scene_root_entity_ = nullptr;
  Qt3DRender::QRenderCapture* scene_render_capture_ = nullptr;
  Qt3DRender::QRenderCaptureReply* scene_render_capture_reply_ = nullptr;
  QMetaObject::Connection image_rendering_connection;
  QString image_filename;
  SceneForceUpdater* scene_force_updater_ = nullptr;

  QStringList molecule_list_;
  std::map<QString, Qt3DCore::QEntity*> molecule_entities_;
};

#endif // UI_TABS_VISUALIZATION_TAB_H
