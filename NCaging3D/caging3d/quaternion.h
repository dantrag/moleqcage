#ifndef CAGING3D_QUATERNION_H
#define CAGING3D_QUATERNION_H

namespace caging3d {

class Quaternion {
 public:
  Quaternion();
  Quaternion(double q1, double q2, double q3, double q4);
  double getX() { return q1_; }
  double getY() { return q2_; }
  double getZ() { return q3_; }
  double getW() { return q4_; }

 private:
  double q1_, q2_, q3_, q4_;
};

} // namespace caging3d

#endif // CAGING3D_QUATERNION_H
