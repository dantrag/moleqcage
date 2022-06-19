#include "utils/qt_3d_utilities.h"

#include <QLayout>
#include <Qt3DCore/QTransform>
#include <Qt3DInput/QInputAspect>
#include <Qt3DExtras/QOrbitCameraController>
#include <Qt3DExtras/QForwardRenderer>
#include <Qt3DRender/QCamera>
#include <Qt3DRender/QPointLight>
#include <Qt3DRender/QRenderSettings>

SceneForceUpdater::SceneForceUpdater(Qt3DCore::QEntity *scene_root_entity) {
  dummy_entity_ = new Qt3DCore::QEntity(scene_root_entity);
  dummy_mesh_ = new Qt3DExtras::QSphereMesh();
  dummy_mesh_->setRings(20);
  dummy_mesh_->setSlices(20);
  dummy_mesh_->setRadius(100);
  dummy_material_ = new Qt3DExtras::QPhongMaterial();
  dummy_material_->setDiffuse(QColor(Qt::red));
  dummy_entity_->addComponent(dummy_mesh_);
  dummy_entity_->addComponent(dummy_material_);
}

SceneForceUpdater::~SceneForceUpdater() {
  dummy_entity_->removeComponent(dummy_mesh_);
  dummy_entity_->removeComponent(dummy_material_);
  delete dummy_mesh_;
  delete dummy_material_;
  delete dummy_entity_;
}

void SetupScene(QWidget* window, QWidget*& scene_widget, QWidget* containter_widget,
                Qt3DExtras::Qt3DWindow*& scene, Qt3DCore::QEntity*& scene_root_entity) {
  scene = new Qt3DExtras::Qt3DWindow();
  scene_widget = QWidget::createWindowContainer(scene);
  containter_widget->layout()->addWidget(scene_widget);
  scene_widget->setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding));

  scene->defaultFrameGraph()->setClearColor(0xFFFFFF); // 0x00b200
  auto input = new Qt3DInput::QInputAspect;
  scene->registerAspect(input);

  scene_root_entity = new Qt3DCore::QEntity();
  auto render_settings = new Qt3DRender::QRenderSettings(scene_root_entity);
  render_settings->setRenderPolicy(Qt3DRender::QRenderSettings::OnDemand);

  render_settings->setActiveFrameGraph(scene->defaultFrameGraph());
  scene_root_entity->addComponent(render_settings);
  auto camera_entity = scene->camera();
  camera_entity->lens()->setPerspectiveProjection(45.0f, 16.0f/9.0f, 0.1f, 1000.0f);
  camera_entity->setPosition(QVector3D(0, 0, 30));
  camera_entity->setUpVector(QVector3D(0, 1, 0));
  camera_entity->setViewCenter(QVector3D(0, 0, 0));

  auto light_entity = new Qt3DCore::QEntity(scene_root_entity);
  auto light_transform = new Qt3DCore::QTransform(light_entity);
  light_transform->setTranslation(camera_entity->position());
  auto light = new Qt3DRender::QPointLight(light_entity);
  light->setColor("white");
  light->setIntensity(1);
  light_entity->addComponent(light);
  light_entity->addComponent(light_transform);

  auto camera_controller = new Qt3DExtras::QOrbitCameraController(scene_root_entity);
  camera_controller->setCamera(camera_entity);
  camera_controller->setLookSpeed(1800);
  window->connect(camera_entity, &Qt3DRender::QCamera::positionChanged,
                  light_transform, &Qt3DCore::QTransform::setTranslation);

  scene->setRootEntity(scene_root_entity);
}

Qt3DCore::QEntity* AddMoleculeToScene(Qt3DCore::QEntity* scene_root_entity,
                                      Molecule *molecule, ElementCollection *elements) {
  auto molecule_entity = new Qt3DCore::QEntity(scene_root_entity);

  auto sphere_mesh = new Qt3DExtras::QSphereMesh();
  sphere_mesh->setRings(20);
  sphere_mesh->setSlices(20);
  sphere_mesh->setRadius(1.0);

  for (auto& atom : molecule->Atoms()) {
    auto atomic_number = atom->AtomicNumber();
    double radius = 0.0;
    QColor colour;
    if (atomic_number == 0) {
      radius = atom->Radius();
      colour = QColor("gray");
    } else {
      const auto& element = (*elements)[atomic_number];
      radius = element->Radius();
      colour = element->Colour();
    }

    auto sphere_material = new Qt3DExtras::QPhongMaterial();
    sphere_material->setDiffuse(colour);

    auto sphere_transform = new Qt3DCore::QTransform();
    sphere_transform->setScale(radius);
    sphere_transform->setTranslation(QVector3D(atom->X(), atom->Y(), atom->Z()));

    auto atom_entity = new Qt3DCore::QEntity(molecule_entity);
    atom_entity->setObjectName("Atom");
    atom_entity->addComponent(sphere_mesh);
    atom_entity->addComponent(sphere_material);
    atom_entity->addComponent(sphere_transform);
  }
  return molecule_entity;
}

void ShowMoleculeOnScene(Qt3DCore::QEntity* molecule_entity, bool show) {
  molecule_entity->setEnabled(show);
}

void SetMoleculeTranslationOnScene(Qt3DCore::QEntity* molecule_entity, QVector3D translation) {
  for (auto& component : molecule_entity->components()) {
    if (auto transform = dynamic_cast<Qt3DCore::QTransform*>(component)) {
      transform->setTranslation(translation);
      return;
    }
  }
  auto transform = new Qt3DCore::QTransform();
  transform->setTranslation(translation);
}

void EnableMoleculeTranslationOnScene(Qt3DCore::QEntity* molecule_entity, bool enable) {
  for (auto& component : molecule_entity->components()) {
    if (auto transform = dynamic_cast<Qt3DCore::QTransform*>(component)) {
      transform->setEnabled(enable);
    }
  }
}

void RemoveAllAtomsFromScene(Qt3DCore::QEntity* scene_root_entity) {
  for (auto node : scene_root_entity->childNodes()) {
    auto entity = (Qt3DCore::QEntity*)node;
    if (entity && entity->objectName() == "Atom") {
      for(auto component : entity->components()) {
        entity->removeComponent(component);
        delete component;
      }
      delete node;
    }
  }
}

void RemoveMoleculeFromScene(Qt3DCore::QEntity* molecule_entity) {
  for(auto component : molecule_entity->components()) {
    molecule_entity->removeComponent(component);
    delete component;
  }
  delete molecule_entity;
}
