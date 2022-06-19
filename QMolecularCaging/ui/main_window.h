#ifndef UI_MAIN_WINDOW_H_
#define UI_MAIN_WINDOW_H_
/* avoid include guards collision with auto-generated ui_main_window.h */

#include <QMainWindow>
#include <QListWidgetItem>
#include <Qt3DCore/QEntity>
#include <Qt3DExtras/Qt3DWindow>

#include "chemistry/molecule_collection.h"
#include "ui/tabs/file_tab.h"
#include "ui/tabs/experiment_tab.h"
#include "ui/tabs/analysis_tab.h"
#include "ui/tabs/visualization_tab.h"
#include "utils/qt_3d_utilities.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow {
  Q_OBJECT

 public:
  MainWindow(QWidget* parent = nullptr);
  ~MainWindow();

 signals:
  void CurrentMoleculeChanged(QString file_path);

 private slots:
  void CurrentTabChanged(int index);
  void MoleculeLoaded(QString file_path);
  void MoleculeUnloaded(QString file_path);
  void MoleculeListItemChanged();
  void CurrentListWidgetItemChanged(QListWidgetItem* current, QListWidgetItem* /*previous*/);
  void UpdateMoleculeList();
  void AddMoleculesToHosts();
  void AddMoleculesToGuests();
  void AddMoleculesToScene();

  void UpdatePreviewMolecule(QString file_name);
  void UpdatePreview();

 private:
  void ClearMoleculeCheckmarks();
  QStringList GetCheckedMolecules();

  const QString file_tab_caption_ = "File";
  const QString experiment_tab_caption_ = "Experiment";
  const QString analysis_tab_caption_ = "Analysis";
  const QString visualization_tab_caption_ = "Visualization";

  Ui::MainWindow* ui_ = nullptr;
  FileTab* file_tab_ = nullptr;
  ExperimentTab* experiment_tab_ = nullptr;
  AnalysisTab* analysis_tab_ = nullptr;
  VisualizationTab* visualization_tab_ = nullptr;

  QWidget* scene_widget_ = nullptr;
  Qt3DExtras::Qt3DWindow* scene_ = nullptr;
  Qt3DCore::QEntity* scene_root_entity_ = nullptr;

  ElementCollection* elements_ = nullptr;
  MoleculeCollection* molecules_ = nullptr;
  Molecule* preview_molecule_ = nullptr;
  Qt3DCore::QEntity* preview_molecule_entity_ = nullptr;
};

#endif // UI_MAIN_WINDOW_H_
