#ifndef CAGING3D_OBSTACLE_H
#define CAGING3D_OBSTACLE_H

#include "caging3d/ballsunion.h"

namespace caging3d {

class Obstacle {
 public:
  Obstacle(std::shared_ptr<BallsUnion> bu);
  std::shared_ptr<BallsUnion> ComputeCollisionWithGivenPoint(std::shared_ptr<Weighted_point> p,
                                                             double reference_pointX,
                                                             double reference_pointY,
                                                             double reference_pointZ);

 private:
  std::shared_ptr<BallsUnion> bu_;
};

} // namespace caging3d

#endif // CAGING3D_OBSTACLE_H
