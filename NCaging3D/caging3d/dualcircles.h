#ifndef CAGING3D_DUALCIRCLES_H
#define CAGING3D_DUALCIRCLES_H

#include "caging3d/includes.h"
#include "caging3d/ballsunion.h"

namespace caging3d {

typedef std::vector<std::shared_ptr<Weighted_point>>::iterator Sphere_iterator;
typedef CGAL::Box_intersection_d::Box_with_handle_d<double, 3, Sphere_iterator> Box_sphere;

class DualCircles {
 public:
  DualCircles(CagingParameters params);

  void AddCircle(double x, double y, double z, double w) {
#ifdef CAGING_DEBUG
    if (std::isnan(w)) {
      assert(0);
    }
#endif
    spheres_.push_back(std::make_shared<Weighted_point>(Bare_point(x, y, z), w));
  }

  void ComputeAdjacencyList();

  void ComputeConnectedComponents();

  void AddEdgeBetweenCircles(int a, int b) {
    balls_adjacency_list_[a].push_back(b);
    balls_adjacency_list_[b].push_back(a);
  }

  void BallsDFS(int current_ball, int current_component);

  int ComponentsNumber() { return balls_connected_components_.size(); }

  bool DoComponentsOverlap(std::shared_ptr<DualCircles> dual_diagram_2,
                           int components_slice_1, int components_slice_2);

  void CreateBoxTrees();

  std::vector<Box_sphere> BoxTree(int i) { return box_trees_[i]; }

  std::vector<std::shared_ptr<Weighted_point>>* Spheres() { return &spheres_; }

  void PrintToFile(std::string input);
  void PrintToFile(std::string input, std::function<bool(int)> include_sphere);

  void CheckDuplicateBalls() {
    for (int i = 0; i < spheres_.size(); i ++){
      for (int j = 0; j < spheres_.size(); j ++){
        if ((spheres_[i]->point().x() == spheres_[j]->point().x()) &&
            (spheres_[i]->point().y() == spheres_[j]->point().y()) &&
            (spheres_[i]->point().z() == spheres_[j]->point().z()) &&
            (i != j)) {
#ifdef CAGING_OUTPUT
          std::cout << "DUPLICATE " << i << " " << j << std::endl;
#endif
          this->AddEdgeBetweenCircles(i, j);
        }
      }
    }
  }

  void ComputeAdjacencyListNsquare() {
#ifdef CAGING_OUTPUT
    std::cout << "compute edges stupid way" << std::endl;
    std::cout << spheres_.size() << std::endl;
#endif
    for (int i = 0; i < spheres_.size(); i++) {
      balls_adjacency_list_.push_back({});
    }
    for (int i = 0; i < spheres_.size(); i++) {
      for (int j = 0; j < spheres_.size(); j++) {
        if (i != j) {
          double dx = spheres_[i]->point().x() - spheres_[j]->point().x();
          double dy = spheres_[i]->point().y() - spheres_[j]->point().y();
          double dz = spheres_[i]->point().z() - spheres_[j]->point().z();
          double squared_distance = dx * dx + dy * dy + dz * dz;
          double rad_1 = std::sqrt(spheres_[i]->weight());
          double rad_2 = std::sqrt(spheres_[j]->weight());
          if (squared_distance <= spheres_[i]->weight() + spheres_[j]->weight() + 2 * rad_1 * rad_2) {
#ifdef CAGING_OUTPUT
            //std::cout << ", and the spheres intersect also";
#endif
            this->AddEdgeBetweenCircles(i, j);
          }
        }
      }
    }
  }

  double ComponentVolume(int component);

 private:
  std::vector<std::vector<int>> balls_adjacency_list_;
  std::vector<std::shared_ptr<Weighted_point>> spheres_;
  std::vector<int> component_index_;
  std::vector<std::vector<int>> balls_connected_components_;
  std::vector<std::vector<Box_sphere>> box_trees_;

  // retrieved from params
  double infinity_;
};

} // namespace caging3d

#endif // CAGING3D_DUALCIRCLES_H
