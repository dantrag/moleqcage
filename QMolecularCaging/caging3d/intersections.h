#ifndef CAGING3D_INTERSECTIONS_H
#define CAGING3D_INTERSECTIONS_H

#include "caging3d/includes.h"
#include "caging3d/object3d.h"

namespace caging3d {

const double kCbrt2 = 1.25992104989;
const std::vector<std::vector<int>> kBFSDirections = {{0, 0, -1}, {0, 0, 1}, {0, -1, 0}, {0, 1, 0}, {-1, 0, 0}, {1, 0, 0}};

class SimplePoint {
 public:
  SimplePoint(double x, double y, double z)
      : x_(x), y_(y), z_(z) {}

  SimplePoint(Point p)
      : x_(CGAL::to_double(p.x())),
        y_(CGAL::to_double(p.y())),
        z_(CGAL::to_double(p.z())) {}

#ifdef INCLUDE_EPECK
  SimplePoint(Point_exact p)
      : x_(CGAL::to_double(p.x())),
        y_(CGAL::to_double(p.y())),
        z_(CGAL::to_double(p.z())) {}

  Point_exact to_Point_exact() {
    return Point_exact(x_, y_, z_);
  }
#endif

  double x() { return x_; }
  double y() { return y_; }
  double z() { return z_; }

 private:
  double x_;
  double y_;
  double z_;
};

class SimpleSphere {
 public:
  SimpleSphere(SimplePoint center, double radius) : center_(center), radius_(radius) {}
  SimpleSphere(double x, double y, double z, double radius) : SimpleSphere(SimplePoint(x, y, z), radius) {}
  SimpleSphere() : SimpleSphere(0, 0, 0, 0) {}

  SimplePoint center() { return center_; }
  double radius() { return radius_; }

 private:
  SimplePoint center_;
  double radius_;
};

class BallIntersectionApproximation {
 public:
  BallIntersectionApproximation(SimpleSphere ball1, SimpleSphere ball2, double resolution)
      : ball1_(ball1), ball2_(ball2) {
    ChangeResolution(resolution);
  }
  void ChangeResolution(double resolution);

 private:
  SimpleSphere ball1_;
  SimpleSphere ball2_;
  double res_;
  std::vector<SimpleSphere> small_balls_;
};

inline double distance2(SimplePoint p1, SimplePoint p2) {
  return (p1.x() - p2.x()) * (p1.x() - p2.x()) + (p1.y() - p2.y()) * (p1.y() - p2.y()) + (p1.z() - p2.z()) * (p1.z() - p2.z());
}

inline double distance(SimplePoint p1, SimplePoint p2) {
  return sqrt(distance2(p1, p2));
}

} // namespace caging3d

#endif // CAGING3D_INTERSECTIONS_H
