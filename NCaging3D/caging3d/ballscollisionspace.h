#ifndef CAGING3D_BALLSCOLLISIONSPACE_H
#define CAGING3D_BALLSCOLLISIONSPACE_H

#include "caging3d/slice.h"
#include "caging3d/ballsconnectivitygraphvertex.h"
#include "caging3d/object3d.h"
#include "caging3d/obstacle.h"

namespace caging3d {

class BallsCollisionSpace {
 public:
  BallsCollisionSpace();
  BallsCollisionSpace(const std::shared_ptr<Object3D> object, const std::shared_ptr<Obstacle> obs,
                      std::string so3_grid_filename, CagingParameters params);
  void CreateVertices(std::shared_ptr<Slice> slice);
  void CreateEdges(const std::shared_ptr<Slice> slice_1, const std::shared_ptr<Slice> slice_2);
  void ComputeConnectedComponents();
  CagingResult Result() { return result_; }

 private:
  std::vector<std::shared_ptr<BallsConnectivityGraphVertex>> vertices_;
  std::vector<std::vector<bool>> are_connected_;
  std::vector<Quaternion> orientation_values_;
  std::vector<std::vector<int>> orientation_neighbours_;
  CagingParameters params_;
  CagingResult result_;
};

} // namespace caging3d

#endif // CAGING3D_BALLSCOLLISIONSPACE_H
