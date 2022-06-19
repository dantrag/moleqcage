#ifndef CAGING3D_BALLSCONNECTIVITYGRAPHVERTEX_H
#define CAGING3D_BALLSCONNECTIVITYGRAPHVERTEX_H

#include "caging3d/slice.h"

namespace caging3d {

class BallsConnectivityGraphVertex {
 public:
  BallsConnectivityGraphVertex();
  BallsConnectivityGraphVertex(int orientation_index, int connected_component_in_slice);

  void AddAdjacentVertex(std::shared_ptr<BallsConnectivityGraphVertex>);

  std::vector<std::shared_ptr<BallsConnectivityGraphVertex>> AdjacentVertices() {
    return adjacent_vertices_;
  }

  void SetComponent(int component_number) {
    connected_component_ = component_number;
  }

  int GetComponent() {
    return connected_component_;
  }

  bool IsInfinite() { return is_infinite_; }

  void SetVertexVolume(double volume);
  double GetVertexVolume();

 private:
  int orientation_index_;
  int connected_component_in_slice_;
  bool is_infinite_ = true;
  int connected_component_ = -1;
  std::vector<std::shared_ptr<BallsConnectivityGraphVertex>> adjacent_vertices_;
  double vertex_volume_ = 0;
};

} // namespace caging3d

#endif // CAGING3D_BALLSCONNECTIVITYGRAPHVERTEX_H
