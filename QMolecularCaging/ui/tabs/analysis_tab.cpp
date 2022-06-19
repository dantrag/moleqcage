#include "ui/tabs/analysis_tab.h"

#include <fstream>

#include <QFileDialog>

#include "ui_analysis_tab.h"
#include "utils/caging_utilities.h"
#include "utils/file_utilities.h"

AnalysisTab::AnalysisTab(QWidget* parent)
    : QWidget(parent), ui_(new Ui::AnalysisTab) {
  ui_->setupUi(this);

  for (auto& variable_item : variable_items_) {
    ui_->column_variable_combo_box->addItem(variable_item.first);
    ui_->row_variable_combo_box->addItem(variable_item.first);
  }
  for (auto& result_item : result_items_) {
    ui_->result_variable_combo_box->addItem(result_item.first);
  }

  ui_->column_variable_combo_box->setCurrentIndex(0);
  ui_->row_variable_combo_box->setCurrentIndex(1);
  ui_->result_variable_combo_box->setCurrentIndex(0);

  ui_->time_mode_button_group->setId(ui_->time_ms_radio_button,
                                        static_cast<int>(SummaryTableModel::TimeMode::kTimeMs));
  ui_->time_mode_button_group->setId(ui_->time_s_radio_button,
                                        static_cast<int>(SummaryTableModel::TimeMode::kTimeS));
  ui_->time_mode_button_group->setId(ui_->time_min_radio_button,
                                        static_cast<int>(SummaryTableModel::TimeMode::kTimeMin));
  ui_->time_mode_button_group->setId(ui_->time_h_radio_button,
                                        static_cast<int>(SummaryTableModel::TimeMode::kTimeH));

  connect(ui_->load_jobs_from_files_button, &QPushButton::clicked, this, &AnalysisTab::LoadJobsFromFiles);
  connect(ui_->add_jobs_to_summary_button, &QPushButton::clicked, this, &AnalysisTab::CreateSummary);
  connect(ui_->export_summary_button, &QPushButton::clicked, this, &AnalysisTab::Export);
  connect(ui_->completed_jobs_list_widget, &QListWidget::itemChanged,
          this, &AnalysisTab::JobListItemChanged);
  connect(ui_->completed_jobs_list_widget, &CustomListWidget::currentItemChanged,
          this, &AnalysisTab::DisplayJobDetails);

  ui_->summary_table_view->resizeColumnsToContents();
}

void AnalysisTab::AddCompletedJob(caging3d::CagingJob* job, QString name) {
  ui_->completed_jobs_list_widget->AddCheckablePointerItem(name, job);
}

void AnalysisTab::JobListItemChanged() {
  for (int row = 0; row < ui_->completed_jobs_list_widget->count(); ++row) {
    if (ui_->completed_jobs_list_widget->item(row)->checkState() == Qt::Checked) {
      ui_->add_jobs_to_summary_button->setEnabled(true);
      return;
    }
  }
  ui_->add_jobs_to_summary_button->setEnabled(false);
}

void AnalysisTab::LoadJobsFromFiles() {
  auto files_to_load = QFileDialog::getOpenFileNames(this, "Select files with caging job results", QString(),
                                                    "CJB (*.cjb);;All files (*)");
  if (!files_to_load.empty()) {
    for (auto filename : files_to_load) {
      loaded_jobs_.push_back(std::unique_ptr<caging3d::CagingJob>(
                               new caging3d::CagingJob(filename.toStdString())));
      ui_->completed_jobs_list_widget->AddCheckablePointerItem(QString("Loaded job #%1").arg(loaded_jobs_.size()),
                                                               loaded_jobs_.back().get());
    }
  }
}

void AnalysisTab::DisplayJobDetails() {
  ui_->job_details_edit->clear();
  const auto& list_item = ui_->completed_jobs_list_widget->currentItem();
  if (list_item != nullptr) {
    auto job = static_cast<caging3d::CagingJob*>(list_item->data(Qt::UserRole).value<void*>());
    ui_->job_details_edit->insertHtml(JobDescription(job, list_item->text()));
  }
}

void AnalysisTab::CreateSummary() {
  /* Delete previous model, if any */
  if (model_ != nullptr) {
    ui_->summary_table_view->setModel(nullptr);
    delete model_;
  }
  model_ = new SummaryTableModel();

  /* Collect and add results to the model */
  for (int row = 0; row < ui_->completed_jobs_list_widget->count(); ++row) {
    if (ui_->completed_jobs_list_widget->item(row)->checkState() == Qt::Checked) {
      auto list_item_user_data = ui_->completed_jobs_list_widget->item(row)->data(Qt::UserRole);
      auto job = static_cast<caging3d::CagingJob*>(list_item_user_data.value<void*>());
      if (job != nullptr) {
        SummaryTableModel::SingleResult result = {};
        result.object_filename = QString::fromStdString(job->ObjectFileName());
        result.obstacle_filename = QString::fromStdString(job->ObstacleFileName());
        result.grid_filename = QString::fromStdString(job->GridFileName());
        result.epsilon = job->Parameters().epsilon();
        result.infinity = job->Parameters().infinity();
        result.components = job->Result().components().size();
        result.cage = (job->Result().components().size() > 1);
        result.time = job->Result().TimeInMs();
        model_->AddResult(result);
      }
    }
  }
  model_->Sort();

  /* Connect Column/Row/Display controls */
  connect(model_, &SummaryTableModel::ColumnVariableChanged, [&](SummaryTableModel::Variable variable) {
            ui_->column_variable_combo_box->setCurrentIndex(GridVariableToIndex(variable));
          });
  connect(model_, &SummaryTableModel::RowVariableChanged, [&](SummaryTableModel::Variable variable) {
            ui_->row_variable_combo_box->setCurrentIndex(GridVariableToIndex(variable));
          });
  connect(model_, &SummaryTableModel::DisplayVariableChanged, [&](SummaryTableModel::Variable variable) {
            ui_->result_variable_combo_box->setCurrentIndex(ResultVariableToIndex(variable));
          });

  connect(ui_->column_variable_combo_box, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged),
          this, [&](int index) {
            model_->SetColumnVariable(variable_items_[index].second);
            ui_->column_variable_combo_box->setCurrentIndex(GridVariableToIndex(model_->ColumnVariable()));
          });
  connect(ui_->row_variable_combo_box, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged),
          this, [&](int index) {
            model_->SetRowVariable(variable_items_[index].second);
            ui_->row_variable_combo_box->setCurrentIndex(GridVariableToIndex(model_->RowVariable()));
          });
  connect(ui_->result_variable_combo_box, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged),
          this, [&](int index) {
            model_->SetDisplayVariable(result_items_[index].second);
            ui_->result_variable_combo_box->setCurrentIndex(ResultVariableToIndex(model_->DisplayVariable()));
          });
  ui_->column_variable_combo_box->currentIndexChanged(0);
  ui_->row_variable_combo_box->currentIndexChanged(1);
  ui_->result_variable_combo_box->currentIndexChanged(0);

  connect(ui_->time_mode_button_group,
          static_cast<void (QButtonGroup::*)(int)>(&QButtonGroup::buttonClicked),
          model_,
          static_cast<void (SummaryTableModel::*)(int)>(&SummaryTableModel::SetTimeMode));
  connect(ui_->colours_check_box, &QCheckBox::toggled, model_, &SummaryTableModel::EnableColours);

  /* Populate and connect Constant controls */
  struct ConstantControl {
    ConstantControl(SummaryTableModel::Variable variable_, QComboBox* control_)
        : variable(variable_), control(control_) {}

    SummaryTableModel::Variable variable = SummaryTableModel::Variable::kNone;
    QComboBox* control = nullptr;
  };

  std::vector<ConstantControl> constants = {
    { SummaryTableModel::Variable::kObject, ui_->guest_constant_combo_box },
    { SummaryTableModel::Variable::kObstacle, ui_->host_constant_combo_box },
    { SummaryTableModel::Variable::kGrid, ui_->grid_constant_combo_box },
    { SummaryTableModel::Variable::kEpsilon, ui_->epsilon_constant_combo_box },
    { SummaryTableModel::Variable::kInfinity, ui_->infinity_constant_combo_box }
  };

  for (auto& constant : constants) {
    constant.control->clear();
    auto levels = model_->CalculateLevels(constant.variable);
    for (auto level : levels) {
      if (IsFileVariable(constant.variable)) {
        constant.control->addItem(FileName(level.toString()));
      } else {
        constant.control->addItem(level.toString());
      }
      constant.control->setItemData(constant.control->count() - 1, level);
    }
    connect(constant.control, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged),
            [=](int index) {
              model_->SetConstantValue(constant.variable, constant.control->itemData(index));
            });
    constant.control->setCurrentIndex(0);
    constant.control->currentIndexChanged(0);
  }

  ui_->summary_table_view->setModel(model_);

  ui_->summary_table_view->resizeColumnsToContents();
}

void AnalysisTab::Export() {
  auto filename = QFileDialog::getSaveFileName(this, "Export table as image or text file", QString(),
                                               "PNG (*.png);;JPEG (*.jpg *.jpeg);;BMP (*.bmp);;GIF (*.gif);;CSV (*.csv)",
                                               new QString("PNG (*.png)"));
  if (!filename.isEmpty()) {
    if (Extension(filename) == "csv") {
      std::ofstream file(filename.toStdString());
      constexpr char delimiter = ',';
      file << ui_->row_variable_combo_box->currentText().toStdString();
      for (int column = 0; column < ui_->summary_table_view->model()->columnCount(); ++column) {
        file << delimiter << ui_->summary_table_view->model()->headerData(column, Qt::Horizontal).toString().toStdString();
      }
      for (int row = 0; row < ui_->summary_table_view->model()->rowCount(); ++row) {
        file << std::endl;
        file << ui_->summary_table_view->model()->headerData(row, Qt::Vertical).toString().toStdString();
        for (int column = 0; column < ui_->summary_table_view->model()->columnCount(); ++column) {
          auto index = ui_->summary_table_view->model()->index(row, column);
          file << delimiter << ui_->summary_table_view->model()->data(index).toString().toStdString();
        }
      }
      file.close();
    } else {
      auto table_geometry = ui_->summary_table_view->geometry();
      int width = ui_->summary_table_view->verticalHeader()->width();
      for (int column = 0; column < ui_->summary_table_view->model()->columnCount(); ++column) {
        width += ui_->summary_table_view->columnWidth(column) + 2;
      }
      int height = ui_->summary_table_view->horizontalHeader()->height();
      for (int row = 0; row < ui_->summary_table_view->model()->rowCount(); ++row) {
        height += ui_->summary_table_view->rowHeight(row) + 2;
      }

      ui_->summary_table_view->setGeometry(0, 0, width, height);
      QPixmap pixmap(QSize(width, height));
      ui_->summary_table_view->render(&pixmap);
      pixmap.save(filename);
      ui_->summary_table_view->setGeometry(table_geometry);
    }
  }
}

AnalysisTab::~AnalysisTab() {
  delete ui_;
}

int AnalysisTab::GridVariableToIndex(SummaryTableModel::Variable variable) {
  for (int index = 0; index < variable_items_.size(); ++index) {
    if (variable_items_[index].second == variable) {
      return index;
    }
  }
  return 0;
}

int AnalysisTab::ResultVariableToIndex(SummaryTableModel::Variable variable) {
  for (int index = 0; index < result_items_.size(); ++index) {
    if (result_items_[index].second == variable) {
      return index;
    }
  }
  return 0;
}
