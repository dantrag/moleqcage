#ifndef CAGING3D_CAGINGJOB_H
#define CAGING3D_CAGINGJOB_H

#include <chrono>

#include "caging3d/includes.h"
#include "caging3d/object3d.h"
#include "caging3d/obstacle.h"

namespace caging3d {

class CagingJob {
 public:
  CagingJob();
  CagingJob(std::string object_filename, std::string obstacle_filename,
            std::string so3_grid_filename, CagingParameters params);

  std::chrono::high_resolution_clock::time_point SubmissionTime() { return submission_time_; }
  std::string ObjectFileName() { return object_filename_; }
  std::string ObstacleFileName() { return obstacle_filename_; }
  std::string GridFileName() { return so3_grid_filename_; }
  void SetParameters(CagingParameters params) { params_ = params; }
  CagingParameters Parameters() { return params_; }

  CagingResult Run();
  CagingResult Result() { return result_; }
  void PrintToFile(std::string filename, bool detailed = false);

 private:
  void CleanUp();

  std::string object_filename_;
  std::string obstacle_filename_;
  std::string so3_grid_filename_;
  CagingParameters params_;

  std::shared_ptr<Object3D> object_;
  std::shared_ptr<Obstacle> obstacle_;

  std::chrono::high_resolution_clock::time_point submission_time_;
  int initialization_time_;
  int run_time_;
  CagingResult result_;
};

} // namespace caging3d

#endif // CAGING3D_CAGINGJOB_H
