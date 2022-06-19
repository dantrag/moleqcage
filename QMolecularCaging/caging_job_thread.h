#ifndef CAGING_JOB_THREAD_H
#define CAGING_JOB_THREAD_H

#include <QThread>

#include "caging3d/cagingjob.h"

class CagingJobThread : public QThread {
  Q_OBJECT
 public:
  CagingJobThread(QObject* parent = 0);
  ~CagingJobThread();

  void DoRun(caging3d::CagingJob* job, int unique_id = -1);

 signals:
  void ThreadStarted(int id);
  void ThreadFinished(int id);

 protected:
  void run() override;

  caging3d::CagingJob* job_;
  int id_;
};

#endif // CAGING_JOB_THREAD_H
