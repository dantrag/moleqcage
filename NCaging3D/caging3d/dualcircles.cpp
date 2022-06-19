#include "caging3d/dualcircles.h"
//#include "caging3d/intersections.h"

namespace caging3d {

struct ReportComponentsOverlap {
 public:
  ReportComponentsOverlap(std::vector<std::shared_ptr<Weighted_point>>* spheres_1,
                          std::vector<std::shared_ptr<Weighted_point>>* spheres_2, bool& do_overlap)
      : spheres_1(spheres_1), spheres_2(spheres_2), do_overlap(&do_overlap) {}

  // callback functor that reports all truly intersecting triangles
  void operator()(const Box_sphere& a, const Box_sphere& b) const {
#ifdef CAGING_OUTPUT
    //std::cout << "Box " << (a.handle() - spheres_1->begin()) << " and "
    //          << (b.handle() - spheres_2->begin()) << " intersect";
#endif
    auto& A = *(a.handle());
    auto& B = *(b.handle());
    auto& Ap = A->point();
    auto& Bp = B->point();
    double dx = Ap.x() - Bp.x();
    double dy = Ap.y() - Bp.y();
    double dz = Ap.z() - Bp.z();
    double squared_distance = dx * dx + dy * dy + dz * dz;
    double weight_1 = A->weight();
    double weight_2 = B->weight();
    double rad_1 = std::sqrt(weight_1);
    double rad_2 = std::sqrt(weight_2);

    if (squared_distance <= (weight_1 + weight_2 + 2 * rad_1 * rad_2)) {
#ifdef CAGING_OUTPUT
      //std::cout << ", and the spheres intersect also";
#endif
      *do_overlap = true;
    }
  }

 private:
  std::vector<std::shared_ptr<Weighted_point>>* spheres_1;
  std::vector<std::shared_ptr<Weighted_point>>* spheres_2;
  bool* do_overlap;
};

struct ReportBallsIntersection {
 public:
  ReportBallsIntersection(std::vector<std::shared_ptr<Weighted_point>>& spheres,
                          DualCircles& dual_diagram)
      : spheres_(spheres), dual_diagram_(dual_diagram) {}

  // callback functor that reports all truly intersecting balls
  void operator()(const Box_sphere& a, const Box_sphere& b) const {
#ifdef CAGING_OUTPUT
    //std::cout << "Box " << (a.handle() - spheres->begin()) << " and "
    //          << (b.handle() - spheres->begin()) << " intersect";
#endif
    auto& A = *(a.handle());
    auto& B = *(b.handle());
    auto& Ap = A->point();
    auto& Bp = B->point();
    double dx = Ap.x() - Bp.x();
    double dy = Ap.y() - Bp.y();
    double dz = Ap.z() - Bp.z();
    double squared_distance = dx * dx + dy * dy + dz * dz;
    double weight_1 = A->weight();
    double weight_2 = B->weight();

    double rad_1 = std::sqrt(weight_1);
    double rad_2 = std::sqrt(weight_2);
    if (squared_distance <= weight_1 + weight_2 + 2 * rad_1 * rad_2) {
#ifdef CAGING_OUTPUT
      //std::cout << ", and the spheres intersect also";
#endif
      dual_diagram_.AddEdgeBetweenCircles(a.handle() - spheres_.begin(), b.handle() - spheres_.begin());
    }
  }

 private:
  std::vector<std::shared_ptr<Weighted_point>>& spheres_;
  DualCircles& dual_diagram_;
};

DualCircles::DualCircles(CagingParameters params)
    : infinity_(params.infinity()) {}

void DualCircles::ComputeAdjacencyList() {
  for (int i = 0; i < spheres_.size(); i++) {
    balls_adjacency_list_.push_back({});
  }

  // Create the corresponding vector of bounding boxes
  std::vector<Box_sphere> boxes;
  for (Sphere_iterator i = spheres_.begin(); i != spheres_.end(); ++i) {
    auto& sphere = *i;
    auto& center = sphere->point();
    double radius = std::sqrt(sphere->weight());
    double x_min = center.x() - radius;
    double y_min = center.y() - radius;
    double z_min = center.z() - radius;
    double x_max = center.x() + radius;
    double y_max = center.y() + radius;
    double z_max = center.z() + radius;
    boxes.push_back(Box_sphere(CGAL::Bbox_3(x_min, y_min, z_min, x_max, y_max, z_max), i));
  }

  // Run the self intersection algorithm with all defaults
  CGAL::box_self_intersection_d(boxes.begin(), boxes.end(), ReportBallsIntersection(spheres_, *this));
}

void DualCircles::ComputeConnectedComponents() {
  int current_component = 0;
  int first_infinite_ball = -1;
  bool done = false;
  balls_connected_components_.push_back({});
  for (int i = 0; i < spheres_.size(); i++) {
    component_index_.push_back(-1);
    if (spheres_[i]->weight() > infinity_ * infinity_ / 10) {
      if (first_infinite_ball == -1) {
        first_infinite_ball = i;
      }
      component_index_[i] = current_component;
      balls_connected_components_[current_component].push_back(i);
    }
  }
  int seed_ball = first_infinite_ball;
  this->BallsDFS(seed_ball, current_component);
  int next_seed_ball = 0;
  while (!done) {
#ifdef CAGING_OUTPUT
    //std::cout << component_index_[next_seed_ball] << std::endl;
#endif
    if (component_index_[next_seed_ball] < 0) {
      current_component++;
      balls_connected_components_.push_back({});
#ifdef CAGING_OUTPUT
      //std::cout << "weight " << spheres_[next_seed_ball]->weight() << std::endl;
      //std::cout << next_seed_ball << std::endl;
#endif
      this->BallsDFS(next_seed_ball, current_component);
    }
    next_seed_ball++;
    if (next_seed_ball == spheres_.size()) {
      done = true;
    }
  }

#ifdef CAGING_OUTPUT
  //cout << current_component + 1 << " components in the slice" << endl;
#endif
  /*
  if (current_component > 0) {
    if (ComponentVolume(1) > 1000) {
      printToFile(string("1_comp_slice.txt"), [&](int i)->bool { return component_index_[i] == 1; });
    }
#ifdef CAGING_OUTPUT
    std::cout << "volume " << ComponentVolume(1) << std::endl;
#endif
  }
  */
}

void DualCircles::BallsDFS(int current_ball, int current_component) {
  component_index_[current_ball] = current_component;
#ifdef CAGING_OUTPUT
  /*
  cout << "DFS " << current_ball << " " << current_component << endl;
  cout << "DFS " << current_ball << " " << spheres_[current_ball]->point().x() << " " <<
    spheres_[current_ball]->point().y() <<  " " << spheres_[current_ball]->weight() << std::endl;
  */
#endif
  balls_connected_components_[current_component].push_back(current_ball);
#ifdef CAGING_OUTPUT
  //cout << balls_connected_components_[current_component].size() << endl;
#endif
  for (int i = 0; i < balls_adjacency_list_[current_ball].size(); i++) {
    int neighbor = balls_adjacency_list_[current_ball][i];
    if (component_index_[neighbor] < 0) {
      this->BallsDFS(neighbor, current_component);
    }
  }
}

void DualCircles::CreateBoxTrees() {
  for (int i = 0; i < balls_connected_components_.size(); i++) {
    box_trees_.push_back({});
  }

  for (Sphere_iterator i = spheres_.begin(); i != spheres_.end(); ++i) {
    auto& sphere = *i;
    auto& center = sphere->point();
    double radius = std::sqrt(sphere->weight());
    double x_min = center.x() - radius;
    double y_min = center.y() - radius;
    double z_min = center.z() - radius;
    double x_max = center.x() + radius;
    double y_max = center.y() + radius;
    double z_max = center.z() + radius;

    int component = component_index_[i - spheres_.begin()];
    //infinite balls are not taken into account
    if (radius < infinity_ / 10) {
      box_trees_[component].push_back(Box_sphere(CGAL::Bbox_3(x_min, y_min, z_min,
                                                              x_max, y_max, z_max), i));
    }
  }
}

bool DualCircles::DoComponentsOverlap(std::shared_ptr<DualCircles> dual_diagram_2,
                                    int components_slice_1, int components_slice_2) {
  bool do_overlap = false;
  std::vector<Box_sphere> box_tree_1 = box_trees_[components_slice_1];
  std::vector<Box_sphere> box_tree_2 = dual_diagram_2->BoxTree(components_slice_2);
  auto spheres_2 = dual_diagram_2->Spheres();

  // Run the self intersection algorithm with all defaults
  CGAL::box_intersection_d(box_tree_1.begin(), box_tree_1.end(),
                           box_tree_2.begin(), box_tree_2.end(),
                           ReportComponentsOverlap(&spheres_, spheres_2, do_overlap));
#ifdef CAGING_OUTPUT
  //std::cout << "do_overlap " << do_overlap << std::endl;
#endif
  return do_overlap;
}

void DualCircles::PrintToFile(std::string input) {
  PrintToFile(input, [](int)->bool { return true; });
}

void DualCircles::PrintToFile(std::string input, std::function<bool(int)> include_sphere) {
  std::ofstream output_file(input);
  std::stringstream sph;
  for (int i = 0; i < spheres_.size(); ++i) {
    if (include_sphere(i)) {
      auto& sphere = spheres_[i];
      sph << sphere->point().x() << " " << sphere->point().y() << " " <<
             sphere->point().z() << " " << sphere->weight() << std::endl;
    }
  }

  output_file << sph.str();
  output_file.close();
}

double DualCircles::ComponentVolume(int component) {
  double volume = 0;
  for (int i = 0; i < spheres_.size(); i ++) {
    if (component_index_[i] == component) {
      volume = volume + std::sqrt(spheres_[i]->weight());
    }
  }
  return volume;
}

} // namespace caging3d
