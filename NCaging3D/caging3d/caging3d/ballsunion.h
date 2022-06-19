#ifndef CAGING3D_BALLSUNION_H
#define CAGING3D_BALLSUNION_H

#include "caging3d/includes.h"
#include "caging3d/quaternion.h"

namespace caging3d {

class BallsUnion {
 public:
  BallsUnion();
  BallsUnion(std::string input, CagingParameters params);
  ~BallsUnion() {
    points_.clear();
  }

  std::shared_ptr<BallsUnion> Collide(double v_x, double v_y, double v_z, double weight);
  std::shared_ptr<BallsUnion> Rotate(Quaternion quaternion) const;

  void AddPoint(double x, double y, double z, double w);
  void AddPoints(std::shared_ptr<BallsUnion> new_points);
  std::vector<Weighted_point> GetAListOfPoints();
  std::shared_ptr<Weighted_point> get_nth(int n);
  int size();

  void ComputeGeometricCenter();
  double GCX();
  double GCY();
  double GCZ();

 private:
  double gcX_;
  double gcY_;
  double gcZ_;
  std::vector<std::shared_ptr<Weighted_point>> points_;

  // retrieved from params
  double epsilon_;
};

} // namespace caging3d

#endif // CAGING3D_BALLSUNION_H
