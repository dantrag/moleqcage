#include "caging_job_single_process_thread.h"

CagingJobSingleProcessThread::CagingJobSingleProcessThread(QObject* parent)
    : CagingJobThread(parent) {}

void CagingJobSingleProcessThread::run() {
  job_->Run();
}
