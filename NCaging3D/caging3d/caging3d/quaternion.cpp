#include "caging3d/quaternion.h"

namespace caging3d {

Quaternion::Quaternion() {}

Quaternion::Quaternion(double q1, double q2, double q3, double q4)
    : q1_(q1), q2_(q2), q3_(q3), q4_(q4) {}

} // namespace caging3d
