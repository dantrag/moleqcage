#include "utils/caging_utilities.h"

QString JobDescription(caging3d::CagingJob* job, QString name) {
  auto result = job->Result();
  auto components = result.components();
  std::stringstream stream;
  std::string bold = "<span style="" font-weight:600;"">";
  std::string red = "<span style="" color:#cc0000;"">";
  std::string green = "<span style="" color:#008800;"">";
  std::string gray = "<span style="" color:#cccccc;"">";
  stream << name.toStdString() << std::endl;
  if (components.empty()) {
    stream << gray;
  }
  stream << "<p>Time: " << bold << result.TimeInMs() << " ms</span></p>";
  std::string color = gray;
  if (components.size() > 1) {
    color = green;
  }
  if (components.size() == 1) {
    color = red;
  }
  stream << "<p>Components: " << bold << color << components.size() << "</span></span></p>";
  stream << "<p>Volumes:</p>";
  for (int i = 0; i < components.size(); ++i) {
    stream << "<p> #" << i << ":   " << components[i] << "</p>";
  }
  if (components.empty()) {
    stream << "</span>";
  }

  stream << "<p></p><p>Setup info</p>";
  stream << "<p> object: " << job->ObjectFileName() << "</p>";
  stream << "<p> obstacle: " << job->ObstacleFileName() << "</p>";
  stream << "<p> grid: " << job->GridFileName() << "</p>";
  auto time = std::chrono::high_resolution_clock::to_time_t(job->SubmissionTime());
  stream << "<p> submitted: " << std::ctime(&time) << "</p>";
  stream << "<p> epsilon: " << bold << job->Parameters().epsilon() << "</span></p>";
  stream << "<p> infinity: " << job->Parameters().infinity() << "</p>";

  return QString::fromStdString(stream.str());
}
