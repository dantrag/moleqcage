#include "caging_job_thread.h"

CagingJobThread::CagingJobThread(QObject* parent)
    : QThread(parent) {}

void CagingJobThread::DoRun(caging3d::CagingJob* job, int unique_id) {
  id_ = unique_id;
  connect(this, &CagingJobThread::started, [&]() {
    emit ThreadStarted(id_);
  });
  connect(this, &CagingJobThread::finished, [&]() {
    emit ThreadFinished(id_);
  });

  if (!isRunning()) {
    job_ = job;
#ifdef DEBUG_OUTPUT
    std::cout << "Starting thread with ID " << id_ << std::endl;
#endif
    start();
  }
}

void CagingJobThread::run() {
}

CagingJobThread::~CagingJobThread() {
  quit();
  wait();
}
