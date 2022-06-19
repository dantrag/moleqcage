#ifndef UI_TABS_EXPERIMENT_TAB_H
#define UI_TABS_EXPERIMENT_TAB_H

#include <QWidget>
#include <QListWidgetItem>
#include <QStyledItemDelegate>

#include "caging3d/caging_includes.h"
#include "caging_job_single_process_thread.h"
#include "caging_job_multi_process_thread.h"

namespace Ui {
class ExperimentTab;
}

class ExperimentTab : public QWidget {
  Q_OBJECT

 public:
  explicit ExperimentTab(QWidget* parent = nullptr);
  ~ExperimentTab();

  void AddHosts(QStringList file_paths);
  void AddGuests(QStringList file_paths);

 signals:
  void CurrentMoleculeChanged(QString file_path);
  void JobCompleted(caging3d::CagingJob* job, QString name);

 private slots:
   void UpdateLists();
   void ThreadStarted(int thread_id);
   void ThreadFinished(int thread_id);
   void SubmitJobs();
   void AddGrids();
   void CurrentListWidgetItemChanged(QListWidgetItem* current, QListWidgetItem* /*previous*/);
   void ListWidgetItemClicked(QListWidgetItem* item);
   void CalculateMinimalEpsilon();
   void SetMinimalEpsilon();

 private:
   void AddJob(QString object_filename, QString obstacle_filename,
               QString grid_filename, caging3d::CagingParameters parameters);
   void SubmitThread(int job_id);
   void UpdateJobList();
   void DisplayJobDetails();
   void PrintJobToFile(int job_id);

   Ui::ExperimentTab* ui_ = nullptr;
   std::vector<std::unique_ptr<caging3d::CagingJob>> jobs_;
   CagingJobThread* job_thread_ = nullptr;
};

class JobListItemDelegate : public QStyledItemDelegate {
  void paint(QPainter* painter, const QStyleOptionViewItem& option,
             const QModelIndex& index) const override;
};

#endif // UI_TABS_EXPERIMENT_TAB_H
