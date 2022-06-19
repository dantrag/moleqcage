#include "caging_job_multi_process_thread.h"

#include <sys/stat.h>
#include <cstdio>

#include "caging3d/caging_includes.h"
#include "utils/file_utilities.h"

CagingJobMultiProcessThread::CagingJobMultiProcessThread(QObject* parent)
    : CagingJobThread(parent) {}

void CagingJobMultiProcessThread::run() {
  auto object_name = FileName(job_->ObjectFileName()).toStdString();
  auto obstacle_name = FileName(job_->ObstacleFileName()).toStdString();
  auto grid_name = FileName(job_->GridFileName()).toStdString();
  std::stringstream stream;
  stream << "./" << object_name << "_" << obstacle_name << "_" << grid_name << "_"
         << QString::number(job_->Parameters().epsilon(), 'f', 6).toStdString()
         << "_"
         << QString::number(job_->Parameters().infinity(), 'f', 6).toStdString();
  auto filename = stream.str();
  int count = 1;
  std::string count_filename = "";
  while (true) {
    struct stat buffer = {};
    std::stringstream count_stream;
    count_stream << filename << "_" << count << ".cjb";
    count_filename = count_stream.str();
    if (stat(count_filename.c_str(), &buffer) != 0) {
      break;
    }
    count++;
  };

  QString command_line = QString("./NCaging3D -g %1 -o %2 -o %3 -e %4 -i %5 -f %6")
                           .arg(QString::fromStdString(job_->GridFileName()))
                           .arg(QString::fromStdString(job_->ObjectFileName()))
                           .arg(QString::fromStdString(job_->ObstacleFileName()))
                           .arg(job_->Parameters().epsilon())
                           .arg(job_->Parameters().infinity())
                           .arg(QString::fromStdString(count_filename));
  system(command_line.toStdString().c_str());

  job_->LoadFromFile(count_filename);

  std::remove(count_filename.c_str());
}
