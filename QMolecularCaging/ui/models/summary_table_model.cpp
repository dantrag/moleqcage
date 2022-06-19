#include "ui/models/summary_table_model.h"

#include <QColor>

#include <cmath>

#include "utils/file_utilities.h"

SummaryTableModel::SummaryTableModel(QObject* parent)
    : QAbstractTableModel(parent) {}

int SummaryTableModel::rowCount(const QModelIndex&) const {
  return row_variable_levels_.size();// + stats_headers_.size();
}

int SummaryTableModel::columnCount(const QModelIndex&) const {
  return column_variable_levels_.size();// + stats_headers_.size();
}

template<typename T>
QColor GetHeatColour(int min_value, int max_value, T value, bool green_to_red = true) {
  if (green_to_red) {
    if (value > (min_value + max_value) / 2.0) {
      double fraction = (max_value - value) / double(min_value + max_value) * 2;
      return QColor(255, std::round(std::sqrt(fraction) * 255), 0, 20);
    } else {
      double fraction = (value - min_value) / double(min_value + max_value) * 2;
      return QColor(std::round(std::sqrt(fraction) * 255), 255, 0, 20);
    }
  } else {
    if (value > (min_value + max_value) / 2.0) {
      double fraction = (max_value - value) / double(min_value + max_value) * 2;
      return QColor(std::round(std::sqrt(fraction) * 255), 255, 0, 20);
    } else {
      double fraction = (value - min_value) / double(min_value + max_value) * 2;
      return QColor(255, std::round(std::sqrt(fraction) * 255), 0, 20);
    }
  }
}

QVariant SummaryTableModel::data(const QModelIndex& index, int role) const {
  if (role == Qt::BackgroundColorRole && !enable_colours_) {
    return QVariant();
  }
  if (role == Qt::TextAlignmentRole) {
    if (display_variable_ == Variable::kTime) {
      return static_cast<int>(Qt::AlignRight | Qt::AlignVCenter);
    }
    return Qt::AlignCenter;
  }

  auto row = index.row();
  auto column = index.column();
  auto column_level = column_variable_levels_[column];
  auto row_level = row_variable_levels_[row];

  for (auto result : results_) {
    if (result.Value(column_variable_) == column_level && result.Value(row_variable_) == row_level) {
      bool hit = true;
      for (Variable variable = Variable::kFirst; variable < Variable::kFirstResult; ++variable) {
        if (variable != column_variable_ && variable != row_variable_ && variable != display_variable_) {
          if (result.Value(variable) != constants_.Value(variable)) {
            hit = false;
            break;
          }
        }
      }
      if (hit) {
        switch (display_variable_) {
          case Variable::kCage: {
            if (role == Qt::UserRole) return result.cage;
            if (role == Qt::DisplayRole) return result.cage ? "+" : "-";
            if (role == Qt::BackgroundColorRole) return (result.cage ? QColor(0xf0fff0)
                                                                     : QColor(0xfff0f0));
            return QVariant();
          }
          case Variable::kComponents: {
            if (role == Qt::UserRole) return result.components;
            if (role == Qt::DisplayRole) return result.components;
            if (role == Qt::BackgroundColorRole) return (result.cage ? QColor(0xf0fff0)
                                                                     : QColor(0xfff0f0));
            return QVariant();
          }
          case Variable::kTime: {
            double factor = 1.0;
            switch (time_mode_) {
              case TimeMode::kTimeH: factor *= 60;
              case TimeMode::kTimeMin: factor *= 60;
              case TimeMode::kTimeS: factor *= 1000;
              case TimeMode::kTimeMs: factor *= 1;
            }

            if (role == Qt::UserRole) return result.time / factor;
            if (role == Qt::DisplayRole) return result.time / factor;
            if (role == Qt::BackgroundColorRole) {
              return GetHeatColour(min_time_, max_time_, result.time);
            }
            return QVariant();
          }
          default: {
            Q_ASSERT(0);
          }
        }
        break;
      }
    }
  }
/*
  if ((row == 0) || (column == 0)) {
    if (role == Qt::DisplayRole) return (row == 0) ? QString::fromStdString(object) :
                                        (column == 0) ? QString::fromStdString(obstacle) : "";
    if (role == Qt::BackgroundColorRole) return QColor(0xf0f0f0);
    if (role == Qt::TextAlignmentRole) return Qt::AlignCenter;
  } else {
    if (row == static_cast<int>(row_headers_.size()) - 2) { // Mean by rows
      if (column == static_cast<int>(column_headers_.size()) - 1) return QVariant();

      double total = 0.0;
      int count = 0;
      for (int r = 1; r < row; ++r) {
        auto user_data = data(this->index(r, column), Qt::UserRole);
        if (!user_data.isNull()) {
          total += user_data.toDouble();
          count++;
        }
      }
      if ((role == Qt::UserRole) || (role == Qt::DisplayRole)) return total / double(count);
      if (role == Qt::BackgroundColorRole && mode_ == Mode::kCage) {
        return GetHeatColour(0, 1, total / double(count), false);
      }

      return QVariant();
    }

    if (column == static_cast<int>(column_headers_.size()) - 2) { // Mean by columns
      if (row == static_cast<int>(row_headers_.size()) - 1) return QVariant();

      double total = 0.0;
      int count = 0;
      for (int c = 1; c < column; ++c) {
        auto user_data = data(this->index(row, c), Qt::UserRole);
        if (!user_data.isNull()) {
          total += user_data.toDouble();
          count++;
        }
      }
      if ((role == Qt::UserRole) || (role == Qt::DisplayRole)) return total / double(count);
      if (role == Qt::BackgroundColorRole && mode_ == Mode::kCage) {
        return GetHeatColour(0, 1, total / double(count), false);
      }

      return QVariant();
    }

    if (row == static_cast<int>(row_headers_.size()) - 1) { // Total by rows
      double total = 0.0;
      for (int r = 1; r < row - 1; ++r) {
        auto user_data = data(this->index(r, column), Qt::UserRole);
        if (!user_data.isNull()) total += user_data.toDouble();
      }
      if (role == Qt::UserRole || role == Qt::DisplayRole) return total;
      return QVariant();
    }

    if (column == static_cast<int>(column_headers_.size()) - 1) { // Total by columns
      double total = 0.0;
      for (int c = 1; c < column - 1; ++c) {
        auto user_data = data(this->index(row, c), Qt::UserRole);
        if (!user_data.isNull()) total += user_data.toDouble();
      }
      if (role == Qt::UserRole || role == Qt::DisplayRole) return total;
      return QVariant();
    }

    if (results_.find(object) != results_.end()) {
      if (results_.at(object).find(obstacle) != results_.at(object).end()) {
        auto result = results_.at(object).at(obstacle);

      }
    }
  }
  */
  return QVariant();
}

QVariant SummaryTableModel::headerData(int section, Qt::Orientation orientation, int role) const {
  if (role == Qt::DisplayRole) {
    auto title = (orientation == Qt::Vertical) ? row_variable_levels_[section] :
                 (orientation == Qt::Horizontal) ? column_variable_levels_[section] : QVariant();
    auto is_file_variable = (orientation == Qt::Vertical) ? IsFileVariable(row_variable_) :
                            (orientation == Qt::Horizontal) ? IsFileVariable(column_variable_) : false;
    if (is_file_variable) {
      title = QVariant(FileName(title.toString()));
    }
    return title;
  }
  return QVariant();
}

void SummaryTableModel::AddResult(SingleResult result) {
  results_.push_back(result);
  min_time_ = std::min(min_time_, result.time);
  max_time_ = std::max(max_time_, result.time);
}

void SummaryTableModel::Sort() {
  /*
  int count = 0;
  for (auto& column : columns_) {
    column.second = ++count;
    column_headers_[count] = column.first;
  }
  count = 0;
  for (auto& row : rows_) {
    row.second = ++count;
    row_headers_[count] = row.first;
  }
  */
}

std::vector<QVariant> SummaryTableModel::CalculateLevels(Variable variable) {
  std::set<decltype(SingleResult().Value(variable))> levels_set;

  for (auto& result : results_) {
    levels_set.insert(result.Value(variable));
  }
  std::vector<QVariant> levels;
  for (auto& level : levels_set) {
    levels.push_back(QVariant(level));
  }
  return levels;
}

void SummaryTableModel::CalculateLevels() {
  column_variable_levels_ = CalculateLevels(column_variable_);
  row_variable_levels_ = CalculateLevels(row_variable_);
}
