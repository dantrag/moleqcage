#include "caging3d/slice.h"

namespace caging3d {

Slice::Slice() {
#ifdef CAGING_OUTPUT
  std::cout << "slice" ;
#endif
}

Slice::Slice(std::shared_ptr<BallsUnion> bu, Quaternion quaternion, int quaternion_index,
             CagingParameters& params)
    : bu_(bu), quaternion_(quaternion), quaternion_index_(quaternion_index),
      params_(params), infinity_(params.infinity()) {}

void Slice::ConstructDualDiagram() {
  dual_diagram_ = std::make_shared<DualCircles>(params_);
  auto lwp = bu_->GetAListOfPoints();
  auto triang = std::make_shared<Triangulation_3>(lwp.begin(), lwp.end());

  //find points dual to cells of the triangulation
#ifdef CAGING_OUTPUT
  //std::cout << triang->number_of_cells() << " " << triang->number_of_facets() << " " << triang->number_of_edges() << std::endl;
  //std::cout << "get Voronoi vertices " << std::endl;
#endif
  for (auto cell_it = triang->finite_cells_begin(); cell_it != triang->finite_cells_end(); ++cell_it) {
    auto tetr = std::make_shared<Tetrahedron>(cell_it->vertex(0)->point(),
                                              cell_it->vertex(1)->point(),
                                              cell_it->vertex(2)->point(),
                                              cell_it->vertex(3)->point());
#ifdef CAGING_DEBUG
    if (tetr->volume() == 0) {
# ifdef CAGING_OUTPUT
      std::cout << "ZERO VOLUME --------------------------";
      std::cout << cell_it->vertex(0)->point().x() << " " << cell_it->vertex(0)->point().y() <<  " " << cell_it->vertex(0)->point().z() << std::endl;
      std::cout << cell_it->vertex(1)->point().x() << " " << cell_it->vertex(1)->point().y() <<  " " << cell_it->vertex(1)->point().z() << std::endl;
      std::cout << cell_it->vertex(2)->point().x() << " " << cell_it->vertex(2)->point().y() <<  " " << cell_it->vertex(2)->point().z() << std::endl;
      std::cout << cell_it->vertex(3)->point().x() << " " << cell_it->vertex(3)->point().y() <<  " " << cell_it->vertex(3)->point().z() << std::endl;
# endif
    }
#endif
    Bare_point point = triang->dual(cell_it);
#ifdef CAGING_OUTPUT
    //std::cout << point.x() << " " << point.y() << " " << point.z() << std::endl;
#endif
    //compute radius
    double squared_distance = (cell_it->vertex(0)->point().x() - point.x()) * (cell_it->vertex(0)->point().x() - point.x()) +
                              (cell_it->vertex(0)->point().y() - point.y()) * (cell_it->vertex(0)->point().y() - point.y()) +
                              (cell_it->vertex(0)->point().z() - point.z()) * (cell_it->vertex(0)->point().z() - point.z());
    double squared_radius = squared_distance - cell_it->vertex(0)->point().weight();
#ifdef CAGING_DEBUG
    if (std::isnan(squared_radius)) {
      assert(0);
    }
#endif
    if (squared_radius > 0) {
      dual_diagram_->AddCircle(point.x(), point.y(), point.z(), squared_radius);
    }
  }

  //find edges dual to faces of the triangulation
#ifdef CAGING_OUTPUT
  //std::cout << "look at faces" << std::endl;
#endif
  for (auto face_it = triang->finite_facets_begin(); face_it != triang->finite_facets_end(); ++face_it) {
    Triangle triangle = triang->triangle(*face_it);
    if (triangle.squared_area() < 1e-30) {
#ifdef CAGING_OUTPUT
      std::cout << triangle.squared_area();
#endif
      assert(0);
    }
    Triangulation_3::Object res = triang->dual(*face_it);
    const Segment* s = CGAL::object_cast<Segment>(&res);
    if (!s) {
      //if the edge is a ray and not a segment
      const Ray* ray = CGAL::object_cast<Ray>(&res);
      if (!ray) {
        assert(0);
      }
      Bare_point point (ray->point(0).x(), ray->point(0).y(), ray->point(0).z());
      double dx = ray->direction().dx();
      double dy = ray->direction().dy();
      double dz = ray->direction().dz();
      double norm = std::sqrt(dx * dx + dy * dy + dz * dz);
      if (norm < 1e-30) continue;
      dx = dx / norm;
      dy = dy / norm;
      dz = dz / norm;
      double centre_x = point.x() + dx * infinity_;
      double centre_y = point.y() + dy * infinity_;
      double centre_z = point.z() + dz * infinity_;
      double squared_radius = (dx * dx + dy * dy + dz * dz) * infinity_ * infinity_;
      dual_diagram_->AddCircle(centre_x, centre_y, centre_z, squared_radius);
#ifdef CAGING_OUTPUT
      // std::cout << squared_radius << std::endl;
#endif
    }
  }

  triang.reset();
  dual_diagram_->ComputeAdjacencyList();
#ifdef CAGING_DEBUG
# ifdef CAGING_OUTPUT
  std::cout << "check duplicates" << std::endl;
# endif
  dual_diagram_->checkDuplicateBalls();
#endif

  dual_diagram_->ComputeConnectedComponents();
  dual_diagram_->CreateBoxTrees();

  //printCollisionSpaceBallsToFile("collisionSpace.txt");
  //dual_diagram_->printToFile("spheres.txt");
}

void Slice::PrintCollisionSpaceBallsToFile(std::string input) {
  std::ofstream output_file(input);
  std::stringstream sph;
  auto lwp = bu_->GetAListOfPoints();
  for (int i = 0; i < lwp.size(); ++i) {
    auto& sphere = lwp[i];
    sph << sphere.point().x() << " " << sphere.point().y() << " " << sphere.point().z() << " " << sphere.weight() << std::endl;
  }
  output_file << sph.str();
  output_file.close();
}

double Slice::ComponentVolume(int component) {
  return dual_diagram_->ComponentVolume(component);
}

} // namespace caging3d
