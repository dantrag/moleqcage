#ifndef UI_TABS_ANALYSIS_TAB_H
#define UI_TABS_ANALYSIS_TAB_H

#include <map>

#include <QWidget>

#include "caging3d/cagingjob.h"
#include "ui/models/summary_table_model.h"

namespace Ui {
class AnalysisTab;
}

class AnalysisTab : public QWidget {
  Q_OBJECT

 public:
  explicit AnalysisTab(QWidget* parent = nullptr);
  ~AnalysisTab();

 public slots:
  void AddCompletedJob(caging3d::CagingJob* job, QString name);
  void JobListItemChanged();

 private slots:
  void LoadJobsFromFiles();
  void DisplayJobDetails();
  void CreateSummary();
  void Export();

 private:
  int GridVariableToIndex(SummaryTableModel::Variable variable);
  int ResultVariableToIndex(SummaryTableModel::Variable variable);

  Ui::AnalysisTab* ui_ = nullptr;
  SummaryTableModel* model_ = nullptr;
  std::vector<std::unique_ptr<caging3d::CagingJob>> loaded_jobs_;

  const std::vector<std::pair<QString, SummaryTableModel::Variable>> variable_items_ = {
    { "Guest", SummaryTableModel::Variable::kObject },
    { "Host", SummaryTableModel::Variable::kObstacle },
    { "SO3 grid", SummaryTableModel::Variable::kGrid },
    { "Epsilon", SummaryTableModel::Variable::kEpsilon },
    { "Infinity", SummaryTableModel::Variable::kInfinity }
  };
  const std::vector<std::pair<QString, SummaryTableModel::Variable>> result_items_ = {
    { "Cage result (yes/no)", SummaryTableModel::Variable::kCage },
    { "Number of components", SummaryTableModel::Variable::kComponents },
    { "Time", SummaryTableModel::Variable::kTime }
  };
};

#endif // UI_TABS_ANALYSIS_TAB_H
