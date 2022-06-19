#ifndef CAGING_JOB_MULTI_PROCESS_THREAD_H
#define CAGING_JOB_MULTI_PROCESS_THREAD_H

#include "caging_job_thread.h"

class CagingJobMultiProcessThread : public CagingJobThread {
  Q_OBJECT
 public:
  CagingJobMultiProcessThread(QObject* parent = 0);

 protected:
  void run() override;
};

#endif // CAGING_JOB_MULTI_PROCESS_THREAD_H
