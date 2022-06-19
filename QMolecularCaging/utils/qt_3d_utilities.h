#ifndef UTILS_QT_3D_UTILITIES_H
#define UTILS_QT_3D_UTILITIES_H

#include <QWidget>
#include <QVector3D>
#include <Qt3DCore/QEntity>
#include <Qt3DExtras/Qt3DWindow>
#include <Qt3DExtras/QPhongMaterial>
#include <Qt3DExtras/QSphereMesh>

#include <chemistry/molecule.h>
#include <chemistry/element_collection.h>

class SceneForceUpdater {
 public:
  SceneForceUpdater(Qt3DCore::QEntity* scene_root_entity);
  ~SceneForceUpdater();

 private:
  Qt3DCore::QEntity* dummy_entity_ = nullptr;
  Qt3DExtras::QSphereMesh* dummy_mesh_ = nullptr;
  Qt3DExtras::QPhongMaterial* dummy_material_ = nullptr;
};

void SetupScene(QWidget* window, QWidget*& scene_widget, QWidget* containter_widget,
                Qt3DExtras::Qt3DWindow*& scene, Qt3DCore::QEntity*& scene_root_entity);

Qt3DCore::QEntity* AddMoleculeToScene(Qt3DCore::QEntity* scene_root_entity,
                                      Molecule* molecule, ElementCollection* elements);

void ShowMoleculeOnScene(Qt3DCore::QEntity* molecule_entity, bool show = true);

void SetMoleculeTranslationOnScene(Qt3DCore::QEntity* molecule_entity, QVector3D translation);
void EnableMoleculeTranslationOnScene(Qt3DCore::QEntity* molecule_entity, bool enable);

void RemoveAllAtomsFromScene(Qt3DCore::QEntity* scene_root_entity);

void RemoveMoleculeFromScene(Qt3DCore::QEntity* molecule_entity);

#endif // UTILS_QT_3D_UTILITIES_H
