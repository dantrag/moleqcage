#include "ui/tabs/experiment_tab.h"

#include <sys/stat.h>
#include <ctime>
#include <set>

#include <QFileDialog>
#include <QPainter>
#include <QPainterPath>

#include "ui_experiment_tab.h"
#include "utils/file_utilities.h"
#include "utils/caging_utilities.h"

ExperimentTab::ExperimentTab(QWidget *parent)
    : QWidget(parent), ui_(new Ui::ExperimentTab) {
  ui_->setupUi(this);
  ui_->jobs_list_widget->setItemDelegate(new JobListItemDelegate());

  connect(ui_->show_full_path_check_box, &QCheckBox::clicked, this, &ExperimentTab::UpdateLists);
  connect(ui_->add_jobs_button, &QPushButton::clicked, this, &ExperimentTab::SubmitJobs);
  connect(ui_->add_grids_button, &QPushButton::clicked, this, &ExperimentTab::AddGrids);
  connect(ui_->hosts_list_widget, &CustomListWidget::currentItemChanged, this, &ExperimentTab::CurrentListWidgetItemChanged);
  connect(ui_->hosts_list_widget, &CustomListWidget::itemClicked, this, &ExperimentTab::ListWidgetItemClicked);
  connect(ui_->guests_list_widget, &CustomListWidget::currentItemChanged, this, &ExperimentTab::CurrentListWidgetItemChanged);
  connect(ui_->guests_list_widget, &CustomListWidget::itemClicked, this, &ExperimentTab::ListWidgetItemClicked);
  connect(ui_->jobs_list_widget, &CustomListWidget::currentItemChanged, this, &ExperimentTab::DisplayJobDetails);

  connect(ui_->guests_combo_box, static_cast<void(QComboBox::*)(int)>(&QComboBox::currentIndexChanged),
          this, &ExperimentTab::CalculateMinimalEpsilon);
  connect(ui_->grid_resolution_spin_box, static_cast<void(QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged),
          this, &ExperimentTab::CalculateMinimalEpsilon);
  connect(ui_->set_minimal_epsilon_spin_box, &QPushButton::clicked, this, &ExperimentTab::SetMinimalEpsilon);

  job_thread_ = new CagingJobSingleProcessThread();
}

void ExperimentTab::AddHosts(QStringList file_paths) {
  for (auto file_path : file_paths) {
    ui_->hosts_list_widget->AddCheckableFileItem(file_path);
  }
  UpdateLists();
}

void ExperimentTab::AddGuests(QStringList file_paths) {
  for (auto file_path : file_paths) {
    ui_->guests_list_widget->AddCheckableFileItem(file_path);
    ui_->guests_combo_box->addItem(FileName(file_path),
                                   ui_->guests_list_widget->item(ui_->guests_list_widget->count() - 1)->data(Qt::UserRole));
  }
  UpdateLists();
}

void ExperimentTab::UpdateLists() {
  bool show_full_path = ui_->show_full_path_check_box->isChecked();
  ui_->hosts_list_widget->SetDisplayMode(show_full_path);
  ui_->guests_list_widget->SetDisplayMode(show_full_path);
  ui_->grids_list_widget->SetDisplayMode(show_full_path);
}

void ExperimentTab::SubmitJobs() {
  caging3d::CagingParameters parameters;

  parameters.SetEpsilon(ui_->epsilon_spin_box->value());
  parameters.SetInfinity(ui_->infinity_spin_box->value());

  int first_job_to_submit = jobs_.size() + 1;

  for (int object_row = 0; object_row < ui_->guests_list_widget->count(); ++object_row) {
    if (ui_->guests_list_widget->item(object_row)->checkState() == Qt::Checked) {
      auto object_filename = ui_->guests_list_widget->item(object_row)->data(Qt::UserRole).toString();

      for (int obstacle_row = 0; obstacle_row < ui_->hosts_list_widget->count(); ++obstacle_row) {
        if (ui_->hosts_list_widget->item(obstacle_row)->checkState() == Qt::Checked) {
          auto obstacle_filename = ui_->hosts_list_widget->item(obstacle_row)->data(Qt::UserRole).toString();

          for (int grid_row = 0; grid_row < ui_->grids_list_widget->count(); ++grid_row) {
            if (ui_->grids_list_widget->item(grid_row)->checkState() == Qt::Checked) {
              auto grid_filename = ui_->grids_list_widget->item(grid_row)->data(Qt::UserRole).toString();

              if (ui_->epsilon_screening_group_box->isChecked()) {
                // instead of using a single value for parameters, perform screening
                if (ui_->binary_search_radio_button->isChecked()) {
                  // TODO
                  // perform binary search
                } else {
                  // do plain iterations
                  auto epsilon_step = ui_->epsilon_step_spin_box->value();
                  auto epsilon_maximum_value = ui_->maximum_epsilon_spin_box->value();
                  for (auto epsilon = ui_->minimum_epsilon_spin_box->value();
                       epsilon <= epsilon_maximum_value; epsilon += epsilon_step) {
                    parameters.SetEpsilon(epsilon);
                    AddJob(ReplaceExtension(object_filename, "xyzr"),
                           ReplaceExtension(obstacle_filename, "xyzr"),
                           grid_filename, parameters);
                  }
                }
              } else {
                AddJob(ReplaceExtension(object_filename, "xyzr"),
                       ReplaceExtension(obstacle_filename, "xyzr"),
                       grid_filename, parameters);
              }
            }
          }
        }
      }
    }
  }
  if (!job_thread_->isRunning()) {
    SubmitThread(first_job_to_submit);
  }
}

void ExperimentTab::AddJob(QString object_filename, QString obstacle_filename,
                           QString grid_filename, caging3d::CagingParameters parameters) {
  jobs_.push_back(std::unique_ptr<caging3d::CagingJob>(
                    new caging3d::CagingJob(object_filename.toStdString(),
                                            obstacle_filename.toStdString(),
                                            grid_filename.toStdString(),
                                            parameters)));

  auto list_item = new QListWidgetItem(QString("Job #%1").arg(jobs_.size()));
  list_item->setSizeHint(QSize(0, 50));
  list_item->setData(Qt::UserRole, QVariant::fromValue<int>(jobs_.size()));
  ui_->jobs_list_widget->addItem(list_item);
}

void ExperimentTab::SubmitThread(int job_id) {
  while (job_thread_->isRunning()) {
    usleep(10000);
  }

  if (ui_->single_process_radio_button->isChecked()) {
    job_thread_ = new CagingJobSingleProcessThread();
  } else {
    job_thread_ = new CagingJobMultiProcessThread();
  }

  connect(job_thread_, &CagingJobThread::ThreadStarted, this, &ExperimentTab::ThreadStarted);
  connect(job_thread_, &CagingJobThread::ThreadFinished, this, &ExperimentTab::ThreadFinished);

  job_thread_->DoRun(jobs_[job_id - 1].get(), job_id);
}

void ExperimentTab::ThreadStarted(int thread_id) {
  ui_->jobs_list_widget->item(thread_id - 1)->setBackgroundColor(QColor(0xffcd4d));
  UpdateJobList();
}

void ExperimentTab::ThreadFinished(int thread_id) {
  job_thread_->disconnect();
  ui_->jobs_list_widget->item(thread_id - 1)->setBackgroundColor(QColor(0x66cc66));
  while (job_thread_->isRunning()) {
    usleep(10000);
  }

  if (ui_->jobs_list_widget->currentRow() == thread_id - 1) {
    DisplayJobDetails();
  }
  if (ui_->save_to_file_check_box->isChecked()) {
    PrintJobToFile(thread_id - 1);
  }
  UpdateJobList();

  emit JobCompleted(jobs_[thread_id - 1].get(), QString("Job #%1").arg(thread_id));

  if (ui_->single_process_radio_button->isChecked()) {
    job_thread_ = new CagingJobSingleProcessThread();
  } else {
    job_thread_ = new CagingJobMultiProcessThread();
  }
  if (thread_id < jobs_.size()) {
    SubmitThread(thread_id + 1);
  }
}

void ExperimentTab::UpdateJobList() {
  auto list_item = new QListWidgetItem();
  ui_->jobs_list_widget->addItem(list_item);
  ui_->jobs_list_widget->removeItemWidget(list_item);
  delete list_item;
}

void ExperimentTab::DisplayJobDetails() {
  ui_->job_details_edit->clear();
  const auto& list_item = ui_->jobs_list_widget->currentItem();
  if (list_item != nullptr) {
    auto job = jobs_[list_item->data(Qt::UserRole).toInt() - 1].get();
    ui_->job_details_edit->insertHtml(JobDescription(job, list_item->text()));
  }
}

void ExperimentTab::PrintJobToFile(int job_id) {
  auto& job = jobs_[job_id];
  auto object_name = FileName(job->ObjectFileName()).toStdString();
  auto obstacle_name = FileName(job->ObstacleFileName()).toStdString();
  auto grid_name = FileName(job->GridFileName()).toStdString();
  std::stringstream stream;
  stream << "./" << object_name << "_" << obstacle_name << "_" << grid_name << "_"
         << QString::number(job->Parameters().epsilon(), 'f', ui_->epsilon_spin_box->decimals()).toStdString()
         << "_"
         << QString::number(job->Parameters().infinity(), 'f', ui_->infinity_spin_box->decimals()).toStdString();
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
  job->PrintToFile(count_filename);
}

void ExperimentTab::AddGrids() {
  auto files_to_add = QFileDialog::getOpenFileNames(this, "Select files with SO3 grids", QString(),
                                                    "All files (*)");
  if (!files_to_add.empty()) {
    std::set<QString> already_present_grids;
    for (int row = 0; row < ui_->grids_list_widget->count(); ++row) {
      already_present_grids.insert(ui_->grids_list_widget->item(row)->data(Qt::UserRole).toString());
    }
    for (auto file : files_to_add) {
      if (already_present_grids.find(file) == already_present_grids.end()) {
        ui_->grids_list_widget->AddCheckableFileItem(file);
      }
    }
  }
  UpdateLists();
}

void ExperimentTab::CurrentListWidgetItemChanged(QListWidgetItem* current, QListWidgetItem* /*previous*/) {
  emit CurrentMoleculeChanged(current->data(Qt::UserRole).toString());
}

void ExperimentTab::ListWidgetItemClicked(QListWidgetItem* item) {
  if (item->isSelected()) {
    emit CurrentMoleculeChanged(item->data(Qt::UserRole).toString());
  }
}

void ExperimentTab::CalculateMinimalEpsilon() {
  if (ui_->guests_combo_box->currentIndex() != -1) {
    std::shared_ptr<caging3d::BallsUnion> guest_balls_union;
    auto filename = ReplaceExtension(ui_->guests_combo_box->currentData().toString(), "xyzr").toStdString();
    guest_balls_union.reset(new caging3d::BallsUnion(filename, caging3d::CagingParameters()));
    caging3d::Object3D guest_object(guest_balls_union);
    double epsilon = std::sin(ui_->grid_resolution_spin_box->value()) * 2.0 * guest_object.GetRadius();
    ui_->minimal_epsilon_spin_box->setValue(epsilon);
  }
}

void ExperimentTab::SetMinimalEpsilon() {
  ui_->epsilon_spin_box->setValue(ui_->minimal_epsilon_spin_box->value());
}

ExperimentTab::~ExperimentTab() {
  delete ui_;
  delete job_thread_;
}

void JobListItemDelegate::paint(QPainter* painter, const QStyleOptionViewItem& option,
                                const QModelIndex& index) const {
  auto background = index.model()->data(index, Qt::BackgroundRole);
  if (background.isNull()) {
    background = QVariant(QColor(Qt::white));
  }
  auto bg_color = background.value<QColor>();
  painter->setRenderHint(QPainter::Antialiasing);
  QPainterPath path;
  QRectF rect(option.rect);
  rect.adjust(0.5, 0.5, -0.5, -0.5);
  path.addRoundedRect(rect, 10, 10);
  auto pen_color = painter->pen().color();
  painter->setPen(bg_color.darker(150));
  painter->fillPath(path, bg_color);
  painter->drawPath(path);
  painter->setPen(pen_color);
  painter->drawText(option.rect, index.model()->data(index, Qt::DisplayRole).toString(),
                    QTextOption(Qt::AlignCenter));

  if (option.state & QStyle::State_Selected) {
    painter->fillPath(path, QColor(0, 0, 255, 32));
  }
}
