#include "caging3d/intersections.h"

#include <queue>

namespace caging3d {

void BallIntersectionApproximation::ChangeResolution(double resolution) {
  res_ = resolution;
  small_balls_.clear();

  auto d2 = distance2(ball1_.center(), ball2_.center());
  double d = sqrt(d2);
  if (d2 == 0.0) {
    small_balls_.push_back(SimpleSphere(ball1_.center(), res_));
  } else if (d < ball1_.radius()) {
    small_balls_.push_back(SimpleSphere(ball2_.center(), res_));
  } else if (d < ball2_.radius()) {
    small_balls_.push_back(SimpleSphere(ball1_.center(), res_));
  } else {
    double lambda = (ball1_.radius() * ball1_.radius() - ball2_.radius() * ball2_.radius() + d2) / d2 / 2.0;

    small_balls_.push_back(SimpleSphere(ball1_.center().x() + lambda * (ball2_.center().x() - ball1_.center().x()),
                           ball1_.center().y() + lambda * (ball2_.center().y() - ball1_.center().y()),
                           ball1_.center().z() + lambda * (ball2_.center().z() - ball1_.center().z()),
                           res_));
  }

  double grid = res_ / kCbrt2;
  std::queue<int> border_balls;
  std::map<int, std::map<int, std::map<int, int>>> grid_check;
  std::vector<std::vector<int>> grid_coords;
  border_balls.push(0);
  grid_check[0][0][0] = 1; // number of ball + 1
  grid_coords.push_back({0, 0, 0});
  double x0 = small_balls_[0].center().x();
  double y0 = small_balls_[0].center().y();
  double z0 = small_balls_[0].center().z();
#ifdef CAGING_DEBUG
# ifdef CAGING_OUTPUT
  cout << "Center 1: " << ball1_.center().x() << " " << ball1_.center().y() << " " << ball1_.center().z() << std::endl;
  cout << "Radius 1: " << ball1_.radius() << std::endl;
  cout << "Center 2: " << ball2_.center().x() << " " << ball2_.center().y() << " " << ball2_.center().z() << std::endl;
  cout << "Radius 2: " << ball2_.radius() << std::endl;
  cout << "Center start: " << x0 << " " << y0 << " " << z0 << std::endl;
# endif
#endif
  while (!border_balls.empty()) {
    int cur_ball = border_balls.front();
    border_balls.pop();
    if ((distance(small_balls_[cur_ball].center(), ball1_.center()) < ball1_.radius()) &&
      (distance(small_balls_[cur_ball].center(), ball2_.center()) < ball2_.radius())) {
      // non-internal balls do not propagate further
      for (int i = 0; i < kBFSDirections.size(); ++i) {
        int grid_x = grid_coords[cur_ball][0] + kBFSDirections[i][0];
        int grid_y = grid_coords[cur_ball][1] + kBFSDirections[i][1];
        int grid_z = grid_coords[cur_ball][2] + kBFSDirections[i][2];
        if (grid_check[grid_x][grid_y][grid_z] == 0) {
          small_balls_.push_back(SimpleSphere(x0 + grid_x * grid,
                                 y0 + grid_y * grid,
                                 z0 + grid_z * grid,
                                 res_));
          grid_coords.push_back({grid_x, grid_y, grid_z});
          grid_check[grid_x][grid_y][grid_z] = small_balls_.size();
          border_balls.push(small_balls_.size() - 1);
        }
      }
    }
  }
#ifdef CAGING_OUTPUT
  std::cout << "Generated " << small_balls_.size() << " small balls" << std::endl;
#endif
}

} // namespace caging3d
