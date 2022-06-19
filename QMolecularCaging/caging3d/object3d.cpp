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

double Object3D::GetRadius() {
  double radius = 0.0;

  for (int i = 0; i < bu_->size(); i++) {
    auto p = bu_->get_nth(i);
    auto ith_radius = sqrt((p->x() - bu_->GCX()) * (p->x() - bu_->GCX()) +
                           (p->y() - bu_->GCY()) * (p->y() - bu_->GCY()) +
                           (p->z() - bu_->GCZ()) * (p->z() - bu_->GCZ())) +
                      p->weight();
    if (ith_radius > radius) {
      radius = ith_radius;
    }
  }

  return radius;
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
