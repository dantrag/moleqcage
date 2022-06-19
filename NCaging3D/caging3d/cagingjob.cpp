#include "cagingjob.h"

#include "ballsunion.h"
#include "ballscollisionspace.h"

namespace caging3d {

CagingJob::CagingJob() {}

CagingJob::CagingJob(std::string object_filename,
                     std::string obstacle_filename,
                     std::string so3_grid_filename,
                     CagingParameters params)
    : object_filename_(object_filename), obstacle_filename_(obstacle_filename),
      so3_grid_filename_(so3_grid_filename), params_(params) {
  submission_time_ = std::chrono::high_resolution_clock::now();
  std::shared_ptr<BallsUnion> object_balls, obstacle_balls;
  obstacle_balls.reset(new BallsUnion(obstacle_filename, params_));
  object_balls.reset(new BallsUnion(object_filename, params_));

  object_.reset(new Object3D(object_balls));
  obstacle_.reset(new Obstacle(obstacle_balls));

  initialization_time_ = std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::high_resolution_clock::now() - submission_time_).count();
}

CagingResult CagingJob::Run() {
  auto t1 = std::chrono::high_resolution_clock::now();
  auto collision_space = std::make_shared<BallsCollisionSpace>(object_, obstacle_,
                                                               so3_grid_filename_,
                                                               params_);
  auto t2 = std::chrono::high_resolution_clock::now();
  run_time_ = std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1).count();

  result_ = collision_space->Result();
  result_.SetTime(initialization_time_ + run_time_);

  CleanUp();

  return result_;
}

void CagingJob::CleanUp() {
  object_.reset();
  obstacle_.reset();
}

void CagingJob::PrintToFile(std::string filename, bool detailed) {
  std::ofstream file(filename);
  std::stringstream stm;
  auto components = result_.components();
  if (detailed) stm << "Number of connected components: ";
  stm << components.size() << std::endl;
  for (int i = 0; i < components.size(); ++i) {
    if (detailed) stm << " component" << i << " volume: ";
    stm << components[i] << std::endl;
  }
  stm << std::endl;
  if (detailed) stm << "The job was running ";
  stm << result_.TimeInMs() << std::endl;
  if (detailed) stm << " ms";
  stm << std::endl;
  stm << "Setup info" << std::endl;
  stm << " object: " << object_filename_ << std::endl;
  stm << " obstacle: " << obstacle_filename_ << std::endl;
  stm << " grid: " << so3_grid_filename_ << std::endl;
  auto time = std::chrono::high_resolution_clock::to_time_t(submission_time_);
  stm << " submitted: " << std::ctime(&time) << std::endl;
  stm << " epsilon: " << params_.epsilon() << std::endl;
  stm << " infinity: " << params_.infinity() << std::endl;
  file << stm.str();
  file.close();
}

} // namespace caging3d
