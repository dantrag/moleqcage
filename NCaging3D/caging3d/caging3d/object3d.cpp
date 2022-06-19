#include "caging3d/object3d.h"

namespace caging3d {

Object3D::Object3D(std::shared_ptr<BallsUnion> bu)
    : bu_(bu) {
#ifdef CAGING_DEBUG
  if (std::isnan(bu->GCY())) {
    assert(0);
  }
#endif
}

double Object3D::GetDiameter() {
  double diameter = 0;

  for (int i = 0; i < bu_->size(); i++) {
    auto p = bu_->get_nth(i);
    auto ith_diameter = p->x() * p->x() + p->y() * p->y() +  p->z() * p->z() + p->weight();
    if (ith_diameter > diameter) {
      diameter = ith_diameter;
    }
  }
  diameter = sqrt(diameter);

#ifdef CAGING_OUTPUT
  std::cout << "object's radius " << diameter << std::endl;
#endif

  return diameter;
}

std::shared_ptr<Object3D> Object3D::Rotate(Quaternion quaternion) {
  auto rotated_points = bu_->Rotate(quaternion);
#ifdef CAGING_DEBUG
  if (std::isnan(rotated_points->GCY())) {
    assert(0);
  }
#endif
  auto obj = std::make_shared<Object3D>(rotated_points);
  return obj;
}

std::shared_ptr<BallsUnion> Object3D::ComputeCollisions(const std::shared_ptr<Obstacle> obstacle) {
  auto collision_points = std::make_shared<BallsUnion>();

  for (int i = 0; i < bu_->size(); i++) {
    auto p = bu_->get_nth(i);
    auto collision_with_given_point = obstacle->ComputeCollisionWithGivenPoint(p, bu_->GCX(),
                                                                               bu_->GCY(),
                                                                               bu_->GCZ());
    collision_points->AddPoints(collision_with_given_point);
  }
  return collision_points;
}

bool Object3D::IsSingleBall() {
  return bu_->size() == 1;
}

} // namespace caging3d
