#ifndef CAGING3D_OBJECT3D_H
#define CAGING3D_OBJECT3D_H

#include "caging3d/ballsunion.h"
#include "caging3d/obstacle.h"

namespace caging3d {

class Object3D {
 public:
  Object3D();
  Object3D(std::shared_ptr<BallsUnion> bu);

  double GetDiameter();
  std::shared_ptr<BallsUnion> ComputeCollisions(const std::shared_ptr<Obstacle> obstacle);
  std::shared_ptr<Object3D> Rotate(Quaternion quaternion);

  bool IsSingleBall();

 private:
  std::shared_ptr<BallsUnion> bu_;
};

} // namespace caging3d

#endif // CAGING3D_OBJECT3D_H
