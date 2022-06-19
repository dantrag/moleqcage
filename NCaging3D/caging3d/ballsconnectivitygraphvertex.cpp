#include "caging3d/ballsconnectivitygraphvertex.h"

namespace caging3d {

BallsConnectivityGraphVertex::BallsConnectivityGraphVertex() {}

BallsConnectivityGraphVertex::BallsConnectivityGraphVertex(int orientation_index,
                                                           int connected_component_in_slice)
    : orientation_index_(orientation_index),
      connected_component_in_slice_(connected_component_in_slice) {
  if (connected_component_in_slice_ > 0) {
    is_infinite_ = false;
  }

  connected_component_ = -1;
}

void BallsConnectivityGraphVertex::AddAdjacentVertex(std::shared_ptr<BallsConnectivityGraphVertex> vertex) {
  adjacent_vertices_.push_back(vertex);
}

void BallsConnectivityGraphVertex::SetVertexVolume(double volume) {
  vertex_volume_ = volume;

#ifdef CAGING_DEBUG
  if (std::isnan(vertex_volume_)) {
    assert(0);
  }
#endif
}

double BallsConnectivityGraphVertex::GetVertexVolume() {
#ifdef CAGING_DEBUG
  if (std::isnan(vertex_volume_)) {
    assert(0);
  }
#endif

  return vertex_volume_;
}

} // namespace caging3d
