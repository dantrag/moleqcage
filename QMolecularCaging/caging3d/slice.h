#ifndef CAGING3D_SLICE_H
#define CAGING3D_SLICE_H

#include "caging3d/ballsunion.h"
#include "caging3d/dualcircles.h"

namespace caging3d {

class Slice {
 public:
  Slice();
  Slice(std::shared_ptr<BallsUnion> bu, Quaternion quaternion, int quaternion_index,
        CagingParameters& params);

  int QuaternionIndex() { return quaternion_index_; }
  void ConstructDualDiagram();
  std::shared_ptr<DualCircles> DualDiagram() { return dual_diagram_; }

  void AddVertex(int v) {
    vertex_indices_.push_back(v);
  }
  std::vector<int> VertexIndices() { return vertex_indices_; }
  int VertexIndex(int i) { return vertex_indices_[i]; }

  void PrintCollisionSpaceBallsToFile(std::string input);
  double ComponentVolume(int component);

 private:
  std::shared_ptr<BallsUnion> bu_;
  Quaternion quaternion_;
  int quaternion_index_;
  std::shared_ptr<DualCircles> dual_diagram_;
  std::vector<int> vertex_indices_;

  CagingParameters params_;
  // retrieved from params
  double infinity_;
};

} // namespace caging3d

#endif // CAGING3D_SLICE_H
