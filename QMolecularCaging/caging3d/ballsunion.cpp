#include "caging3d/ballsunion.h"

namespace caging3d {

BallsUnion::BallsUnion() {
  points_ = {};
}

BallsUnion::BallsUnion(std::string input_, CagingParameters params) {
  epsilon_ = params.epsilon();

  std::ifstream file;
  double x, y, z, w;
  int number;
  points_ = {};
  file.open(input_);
  file >> number;
  while (file >> x >> y >> z >> w) {
#ifdef CAGING_DEBUG
    if (std::isnan(x)) {
      assert(0);
    }
    if (std::isnan(y)) {
      assert(0);
    }
    if (std::isnan(z)) {
      assert(0);
    }
    if (std::isnan(w)) {
      assert(0);
    }
#endif
    points_.push_back(std::make_shared<Weighted_point>(Bare_point(x, y, z), w));
  }
  file.close();
  gcX_ = 0;
  gcY_ = 0;
  gcZ_ = 0;
  for (auto p : points_) {
    gcX_ += p->x();
    gcY_ += p->y();
    gcZ_ += p->z();
  }
  gcX_ = gcX_ / points_.size();
  gcY_ = gcY_ / points_.size();
  gcZ_ = gcZ_ / points_.size();
#ifdef CAGING_DEBUG
  if (std::isnan(gcX_)) {
    assert(0);
  }
  if (std::isnan(gcY_)) {
    assert(0);
  }
  if (std::isnan(gcZ_)) {
    assert(0);
  }
#endif
}

std::vector<Weighted_point> BallsUnion::GetAListOfPoints() {
  std::vector<Weighted_point> list;
#ifdef CAGING_OUTPUT
  // std::cout << points_.size() << std::endl;
#endif
  int perturbation_numerator = 100000;
  double perturbation_denominator;
  //for ring-like object
  //perturbation_denominator = 0.00000001;
  //for both objects max resolution:
  perturbation_denominator = 0.000000001;

  for (auto p : points_) {
    double x = p->point().x();
    double y = p->point().y();
    double z = p->point().z();
    x = x + perturbation_denominator * (rand() % perturbation_numerator);
    y = y + perturbation_denominator * (rand() % perturbation_numerator);
    z = z + perturbation_denominator * (rand() % perturbation_numerator);
#ifdef CAGING_DEBUG
    if (std::isnan(x)) {
      assert(0);
    }
    if (std::isnan(y)) {
      assert(0);
    }
    if (std::isnan(z)) {
      assert(0);
    }
    if (std::isnan(p->weight())) {
      assert(0);
    }
#endif
    list.push_back(Weighted_point(Bare_point(x, y, z), p->weight()));
#ifdef CAGING_OUTPUT
    //std::cout << p->point() << std::endl;
#endif
  }
  return list;
}

std::shared_ptr<BallsUnion> BallsUnion::Collide(double v_x, double v_y, double v_z, double weight) {
  auto translated_points = std::make_shared<BallsUnion>();
  double x, y, z;
  for (auto p : points_) {
    x = p->x() + v_x;
    y = p->y() + v_y;
    z = p->z() + v_z;
#ifdef CAGING_DEBUG
    if (std::isnan(x)) {
      assert(0);
    }
    if (std::isnan(y)) {
      assert(0);
    }
    if (std::isnan(z)) {
      assert(0);
    }
#endif
    translated_points->AddPoint(x, y, z, (p->weight() + weight - epsilon_)*(p->weight() + weight - epsilon_));
#ifdef CAGING_OUTPUT
    //std::cout << "weight " <<
    //  (p->weight() + weight - epsilon_) * (p->weight() + weight - epsilon_) << std::endl;
#endif
  }
  translated_points->ComputeGeometricCenter();

  return translated_points;
}

std::shared_ptr<BallsUnion> BallsUnion::Rotate(Quaternion quaternion) const {
  auto rotated_points = std::make_shared<BallsUnion>();
  //apply rotation
  double q_x, q_y, q_z, s;
  q_x = quaternion.getX();
  q_y = quaternion.getY();
  q_z = quaternion.getZ();
  s = quaternion.getW();

  for (auto p : points_) {
    double x, y, z;
    //vector u = (q_x, q_y, q_z)
    //vector v = (p.x, p.y, p.z)
    double uv_dot = q_x * p->x() + q_y * p->y() + q_z * p->z();
    double uu_dot = q_x * q_x + q_y * q_y + q_z * q_z;
    double uv_cross_x = q_y * p->z() - q_z * p->y();
    double uv_cross_y = q_z * p->x() - q_x * p->z();
    double uv_cross_z = q_x * p->y() - q_y * p->x();

    x = 2 * uv_dot * q_x + (s * s - uu_dot) * p->x() + 2 * s * uv_cross_x;
    y = 2 * uv_dot * q_y + (s * s - uu_dot) * p->y() + 2 * s * uv_cross_y;
    z = 2 * uv_dot * q_z + (s * s - uu_dot) * p->z() + 2 * s * uv_cross_z;

#ifdef CAGING_OUTPUT
    //std::cout << "rotated " << std::endl;
    //std::cout << x << " " << y << " " << z << std::endl;
#endif
    rotated_points->AddPoint(x, y, z, p->weight());

#ifdef CAGING_OUTPUT
    //std::cout << "rotated points " << x << " " << y << " " << z << std::endl;
    //std::cout << "distance " << x * x + y * y + z * z << endl;
    // Do the math
    /*
    vprime = 2.0f * dot(u, v) * u
          + (s * s - dot(u, u)) * v
          + 2.0f * s * cross(u, v);
    */
#endif
  }
#ifdef CAGING_OUTPUT
  //std::cout << "quaternion " << q_x << " " << q_y << " " << q_z << " " << s << std::endl;
#endif

  rotated_points->ComputeGeometricCenter();

#ifdef CAGING_DEBUG
  if (points_.empty()) {
    assert(0);
  }
#endif

  return rotated_points;
}

void BallsUnion::AddPoints(std::shared_ptr<BallsUnion> new_points) {
  for (int i = 0; i < new_points->size(); i++) {
    points_.push_back(new_points->get_nth(i));
  }
}
void BallsUnion::ComputeGeometricCenter() {
  gcX_ = 0;
  gcY_ = 0;
  gcZ_ = 0;
  for (auto p : points_) {
    gcX_ += p->x();
    gcY_ += p->y();
    gcZ_ += p->z();
  }
  gcX_ = gcX_ / points_.size();
  gcY_ = gcY_ / points_.size();
  gcZ_ = gcZ_ / points_.size();
}

void BallsUnion::AddPoint(double x, double y, double z, double w) {
  points_.push_back(std::make_shared<Weighted_point>(Bare_point(x, y, z), w));
}

int BallsUnion::size() {
  return points_.size();
}

std::shared_ptr<Weighted_point> BallsUnion::get_nth(int n) {
  return points_[n];
}

double BallsUnion::GCX() {
  return gcX_;
}

double BallsUnion::GCY() {
  return gcY_;
}

double BallsUnion::GCZ() {
  return gcZ_;
}

} // namespace caging3d
