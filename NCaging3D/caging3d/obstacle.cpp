#include "caging3d/obstacle.h"

namespace caging3d {

Obstacle::Obstacle(std::shared_ptr<BallsUnion> bu)
    : bu_(bu) {}

std::shared_ptr<BallsUnion> Obstacle::ComputeCollisionWithGivenPoint(std::shared_ptr<Weighted_point> p,
                                                                     double reference_pointX,
                                                                     double reference_pointY,
                                                                     double reference_pointZ) {
  double v_x, v_y, v_z;
  v_x = p->x() - reference_pointX;
  v_y = p->y() - reference_pointY;
  v_z = p->z() - reference_pointZ;
  std::shared_ptr<BallsUnion> collision_points = bu_->Collide(v_x, v_y, v_z, p->weight());
  return collision_points;
}

} // namespace caging3d
