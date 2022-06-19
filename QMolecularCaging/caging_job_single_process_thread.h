#ifndef CAGING_JOB_SINGLE_PROCESS_THREAD_H
#define CAGING_JOB_SINGLE_PROCESS_THREAD_H

#include "caging_job_thread.h"

class CagingJobSingleProcessThread : public CagingJobThread {
  Q_OBJECT
 public:
  CagingJobSingleProcessThread(QObject* parent = 0);

 protected:
  void run() override;
};

#endif // CAGING_JOB_SINGLE_PROCESS_THREAD_H
