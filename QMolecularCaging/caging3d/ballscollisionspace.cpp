#include "caging3d/ballscollisionspace.h"

#include <queue>

namespace caging3d {

BallsCollisionSpace::BallsCollisionSpace() {}

BallsCollisionSpace::BallsCollisionSpace(const std::shared_ptr<Object3D> object,
                                         const std::shared_ptr<Obstacle> obs,
                                         std::string so3_grid_filename, CagingParameters params) {
  params_ = params;

  std::ifstream file;
  double q1, q2, q3, q4;
  int count = 0;
  int orientation_number;
  file.open(so3_grid_filename);
  file >> orientation_number;
#ifdef CAGING_OUTPUT
  std::cout << orientation_number;
#endif
  while (count < orientation_number) {
    file >> q1 >> q2 >> q3 >> q4;
    orientation_values_.push_back(Quaternion(q1, q2, q3, q4));
    count++;
  }
  count = 0;
#ifdef CAGING_OUTPUT
  // std::cout << orientation_values_.size() << std::endl;
#endif
  if (object->IsSingleBall()) {
    orientation_number = 1;
    orientation_values_.resize(1);
    orientation_neighbours_.push_back({});
  } else {
    while (count < orientation_number) {
      std::string str;
      std::getline(file, str);
      if (!str.empty()) {
        orientation_neighbours_.push_back({});
        std::stringstream stream(str);
        double n;
        while (stream >> n) {
          orientation_neighbours_[count].push_back(n);
        }
        count++;
      }
    }
  }
  file.close();

  std::shared_ptr<BallsUnion> collision_points;
  std::vector<std::shared_ptr<Slice>> slice_pointers(orientation_number, nullptr);
  std::queue<int> orientation_queue;
  std::shared_ptr<Object3D> rotated_object;
  std::vector<bool> marked_orientations(orientation_number, false);

  int start_slice = 0;
  orientation_queue.push(0);
  auto quaternion = orientation_values_[start_slice];
  marked_orientations[start_slice] = true;
  rotated_object = object->Rotate(quaternion);
  collision_points = rotated_object->ComputeCollisions(obs);
  slice_pointers[0].reset(new Slice(collision_points, quaternion, start_slice, params_));
  slice_pointers[0]->ConstructDualDiagram();
  CreateVertices(slice_pointers[0]);

  int number_of_it = 0;
  int max_size = 0;

  while (!orientation_queue.empty()) {
    number_of_it++;
#ifdef CAGING_OUTPUT
    //std::cout << "-------------------------------------------count " << number_of_it << std::endl;
#endif
    int quaternion_number = orientation_queue.front();
#ifdef CAGING_OUTPUT
    //std::cout << "queue size " << orientation_queue.size() << std::endl;
#endif
    for (int neighbour = 0; neighbour < orientation_neighbours_[quaternion_number].size(); neighbour++) {
      int neighbour_index = orientation_neighbours_[quaternion_number][neighbour];

      //if the slice is not yet in the queue
      if (!marked_orientations[neighbour_index]) {
        Quaternion quaternion = orientation_values_[neighbour_index];
        marked_orientations[neighbour_index] = true;
        rotated_object = object->Rotate(quaternion);
        collision_points = rotated_object->ComputeCollisions(obs);
        slice_pointers[neighbour_index].reset(new Slice(collision_points, quaternion,
                                                        neighbour_index, params_));
#ifdef CAGING_OUTPUT
        // std::cout << "neighbour_index " << neighbour_index;
#endif
        slice_pointers[neighbour_index]->ConstructDualDiagram();

        CreateVertices(slice_pointers[neighbour_index]);
#ifdef CAGING_OUTPUT
        //std::cout << "added neighbour " << neighbour_index << std::endl;
#endif
        orientation_queue.push(neighbour_index);
      }

      //if the slice is not deleted yet
      if (slice_pointers[neighbour_index] != nullptr) {
        //std::cout << "or 1 " << neighbour_index << "or 2 " << quaternion_number << std::endl;
        CreateEdges(slice_pointers[neighbour_index], slice_pointers[quaternion_number]);
      }
    }

    if (max_size < orientation_queue.size()) {
      max_size = orientation_queue.size();
    }

    orientation_queue.pop();

    slice_pointers[quaternion_number].reset();
  }

  ComputeConnectedComponents();
}


void BallsCollisionSpace::CreateVertices(std::shared_ptr<Slice> slice) {
#ifdef CAGING_OUTPUT
  //std::cout << "create vertices" << std::endl;
#endif
  auto dual_diagram = slice->DualDiagram();
  for (int i = 0; i < dual_diagram->ComponentsNumber(); i++) {
    auto new_vertex = std::make_shared<BallsConnectivityGraphVertex>(slice->QuaternionIndex(), i);
    new_vertex->SetVertexVolume(slice->ComponentVolume(i));
#ifdef CAGING_OUTPUT
    //std::cout << new_vertex->GetVertexVolume() << std::endl;
#endif
    vertices_.push_back(new_vertex);
    slice->AddVertex(vertices_.size() - 1);
  }
#ifdef CAGING_OUTPUT
  //std::cout << "total number of vertices: " << vertices_.size() << std::endl;
#endif
}

void BallsCollisionSpace::CreateEdges(const std::shared_ptr<Slice> slice_1,
                                      const std::shared_ptr<Slice> slice_2) {
#ifdef CAGING_OUTPUT
  //std::cout << "create edges" << std::endl;
#endif

  //resize the vectors
  if (are_connected_.size() < vertices_.size()) {
    are_connected_.resize(vertices_.size(), {});
    for (auto& row : are_connected_) {
      row.resize(vertices_.size(), false);
    }
  }
  auto dual_diagram_1 = slice_1->DualDiagram();
  auto dual_diagram_2 = slice_2->DualDiagram();
#ifdef CAGING_OUTPUT
  //std::cout << "test intersections" << std::endl;
#endif

  //test intersections
  for (int components_slice_1 = 0; components_slice_1 < dual_diagram_1->ComponentsNumber(); components_slice_1++) {
    for (int components_slice_2 = 0; components_slice_2 < dual_diagram_2->ComponentsNumber(); components_slice_2++) {
      int vertex_id_1 = slice_1->VertexIndex(components_slice_1);
      int vertex_id_2 = slice_2->VertexIndex(components_slice_2);
#ifdef CAGING_OUTPUT
      //std::cout << vertex_id_1 << " " << vertex_id_2 << std::endl;
#endif
      if (components_slice_1 + components_slice_2 == 0) {
        are_connected_[vertex_id_1][vertex_id_2] = true;
        are_connected_[vertex_id_2][vertex_id_1] = true;
        vertices_[vertex_id_1]->AddAdjacentVertex(vertices_[vertex_id_2]);
        vertices_[vertex_id_2]->AddAdjacentVertex(vertices_[vertex_id_1]);
        continue;
      }

      bool do_overlap = dual_diagram_1->DoComponentsOverlap(dual_diagram_2, components_slice_1,
                                                            components_slice_2);
      are_connected_[vertex_id_1][vertex_id_2] = do_overlap;
      are_connected_[vertex_id_2][vertex_id_1] = do_overlap;
      if (do_overlap) {
        vertices_[vertex_id_1]->AddAdjacentVertex(vertices_[vertex_id_2]);
        vertices_[vertex_id_2]->AddAdjacentVertex(vertices_[vertex_id_1]);
      }
#ifdef CAGING_DEBUG
      if (do_overlap && (components_slice_1 * components_slice_2 == 0)) {
#ifdef CAGING_OUTPUT
        //std::cout << "different components overlap!"<< std::endl;
#endif
        //assert(0);
        /*
        if (components_slice_1 == 1) {
          dual_diagram_1->printToFile("overlap.txt");
        } else {
          dual_diagram_2->printToFile("overlap.txt");
        }
        */
      }
#endif
    }
  }
}

void BallsCollisionSpace::ComputeConnectedComponents() {
#ifdef CAGING_OUTPUT
  std::cout << "connecting components" << std::endl;
#endif

  result_ = CagingResult();
  int component_number = -1;
  double compVolume;
#ifdef CAGING_OUTPUT
  std::cout << "vertices " << vertices_.size() << std::endl;
#endif
  for (int i = 0; i < vertices_.size(); i++) {
    if (vertices_[i]->GetComponent() < 0) {
      component_number++;
#ifdef CAGING_OUTPUT
      std::cout << "component " << component_number << std::endl;
#endif
      compVolume = 0;
      auto seed_vertex = vertices_[i];
      std::vector<std::shared_ptr<BallsConnectivityGraphVertex>> current_component = {};
      std::list<std::shared_ptr<BallsConnectivityGraphVertex>> queue = {};
      current_component.push_back(seed_vertex);
      queue.push_back(seed_vertex);
      seed_vertex->SetComponent(component_number);
      compVolume = compVolume + seed_vertex->GetVertexVolume();
      while (!queue.empty()) {
        auto current_vertex = queue.front();
        queue.pop_front();
#ifdef CAGING_OUTPUT
        //std::cout << "adjacent " << current_vertex->AdjacentVertices().size() << std::endl;
#endif
        for (auto neighbor : current_vertex->AdjacentVertices()) {
          if (neighbor->GetComponent() < 0) {
            queue.push_back(neighbor);
            neighbor->SetComponent(component_number);
            compVolume = compVolume + neighbor->GetVertexVolume();
#ifdef CAGING_DEBUG
            if (std::isnan(compVolume)) {
              assert(0);
            }
#endif
          }
        }
      }
      result_.AddComponent(compVolume);

#ifdef CAGING_OUTPUT
        std::cout << "component " << component_number << " volume " << compVolume << std::endl;
#endif
    }
  }
#ifdef CAGING_OUTPUT
    std::cout << component_number + 1 << " components" << std::endl;
    //std::cout << bad_edges << std::endl;
#endif
}

} // namespace caging3d
