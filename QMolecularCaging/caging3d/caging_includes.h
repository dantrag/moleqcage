#ifndef CAGING3D_CAGING_INCLUDES_H
#define CAGING3D_CAGING_INCLUDES_H

#include <vector>

namespace caging3d {

/*
 * epsilon = 0.36 for molecules
 * epsilon = 0.7 for object-ring
 * epsilon = 1.25 to get a nice hole in the 1 exp
 * epsilon = 2.0 for object-narrow-part
 * infinity was previously 1e7, 1e10
 */

class CagingParameters {
 public:
  CagingParameters() {}

  void SetEpsilon(double epsilon) { epsilon_ = epsilon; }
  void SetInfinity(double infinity) { infinity_ = infinity; }

  double epsilon() { return epsilon_; }
  double infinity() { return infinity_; }

 private:
  double epsilon_ = 1.0;
  double infinity_ = 1e5;
};

class CagingResult {
 public:
  CagingResult() {}

  void SetTime(double time_in_ms) { time_ = time_in_ms; }
  void AddComponent(double volume) { component_volumes_.push_back(volume); }

  int TimeInMs() { return time_; }
  double TimeInS() { return time_ / 1000.0; }
  std::vector<double> components() { return component_volumes_; }

 private:
  int time_ = 0;
  std::vector<double> component_volumes_;
};

} // namespace caging3d

#endif // CAGING3D_CAGING_INCLUDES_H
