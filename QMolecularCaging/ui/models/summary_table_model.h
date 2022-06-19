#ifndef UI_MODELS_SUMMARY_TABLE_MODEL_H
#define UI_MODELS_SUMMARY_TABLE_MODEL_H

#include <set>

#include <QAbstractTableModel>

class SummaryTableModel : public QAbstractTableModel {
  Q_OBJECT

 public:
  enum class Variable {
    kFirst,
    kObject = Variable::kFirst,
    kObstacle,
    kGrid,
    kEpsilon,
    kInfinity,
    kComponents,
    kFirstResult = Variable::kComponents,
    kCage,
    kTime,
    kLast = Variable::kTime,
    kNone
  };

  struct SingleResult {
    SingleResult() {}

    QVariant Value(Variable variable) const {
      switch (variable) {
        case Variable::kObject: return QVariant(object_filename);
        case Variable::kObstacle: return QVariant(obstacle_filename);
        case Variable::kGrid: return QVariant(grid_filename);
        case Variable::kEpsilon: return QVariant(epsilon);
        case Variable::kInfinity: return QVariant(infinity);
        case Variable::kComponents: return QVariant(components);
        case Variable::kCage: return QVariant(cage);
        case Variable::kTime: return QVariant(time);
        case Variable::kNone: return QVariant();
      }
      return QVariant();
    }

    void SetValue(Variable variable, QVariant value) {
      switch (variable) {
        case Variable::kObject: {
          object_filename = value.toString();
          break;
        }
        case Variable::kObstacle: {
          obstacle_filename = value.toString();
          break;
        }
        case Variable::kGrid: {
          grid_filename = value.toString();
          break;
        }
        case Variable::kEpsilon: {
          epsilon = value.toDouble();
          break;
        }
        case Variable::kInfinity: {
          infinity = value.toDouble();
          break;
        }
        case Variable::kComponents: {
          components = value.toInt();
          break;
        }
        case Variable::kCage: {
          cage = value.toBool();
          break;
        }
        case Variable::kTime: {
          time = value.toInt();
          break;
        }
        default: {
          Q_ASSERT(0);
        }
      }
    }

    QString object_filename;
    QString obstacle_filename;
    QString grid_filename;
    double epsilon = 0.0;
    double infinity = 0.0;
    int components = 0;
    bool cage = false;
    int time = 0;                 // in milliseconds
  };

  enum class TimeMode {
    kTimeMs,
    kTimeS,
    kTimeMin,
    kTimeH
  };

  SummaryTableModel(QObject* parent = nullptr);

  int rowCount(const QModelIndex& parent = QModelIndex()) const;
  int columnCount(const QModelIndex& parent = QModelIndex()) const;
  QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const;
  QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;

  void AddResult(SingleResult result);

  void SetTimeMode(TimeMode mode) {
    time_mode_ = mode;
    emit dataChanged(QModelIndex(), QModelIndex());
  }
  void SetTimeMode(int time_mode) {
    SetTimeMode(static_cast<TimeMode>(time_mode));
  }
  void SetColumnVariable(Variable variable) {
    if (variable == display_variable_) {
      display_variable_ = column_variable_;
      column_variable_ = variable;
    } else if (variable == row_variable_) {
      row_variable_ = column_variable_;
      column_variable_ = variable;
      SetRowVariable(row_variable_);
    } else {
      column_variable_ = variable;
    }
    CalculateLevels();
    emit ColumnVariableChanged(column_variable_);
    emit dataChanged(QModelIndex(), QModelIndex());
    emit headerDataChanged(Qt::Horizontal, 0, columnCount());
  }
  void SetRowVariable(Variable variable) {
    if (variable == display_variable_) {
      display_variable_ = row_variable_;
      row_variable_ = variable;
    } else if (variable == column_variable_) {
      column_variable_ = row_variable_;
      row_variable_ = variable;
      SetColumnVariable(column_variable_);
    } else {
      row_variable_ = variable;
    }
    CalculateLevels();
    emit RowVariableChanged(row_variable_);
    emit dataChanged(QModelIndex(), QModelIndex());
    emit headerDataChanged(Qt::Vertical, 0, rowCount() - 1);
  }
  void SetDisplayVariable(Variable variable) {
    if (variable == column_variable_) {
      column_variable_ = display_variable_;
      display_variable_ = variable;
      SetColumnVariable(column_variable_);
    } else if (variable == row_variable_) {
      row_variable_ = display_variable_;
      display_variable_ = variable;
      SetRowVariable(row_variable_);
    } else {
      display_variable_ = variable;
    }
    emit DisplayVariableChanged(display_variable_);
    emit dataChanged(QModelIndex(), QModelIndex());
  }
  Variable ColumnVariable() {
    return column_variable_;
  }
  Variable RowVariable() {
    return row_variable_;
  }
  Variable DisplayVariable() {
    return display_variable_;
  }
  QVariant ConstantValue(Variable variable) {
    return constants_.Value(variable);
  }
  void SetConstantValue(Variable variable, QVariant value) {
    constants_.SetValue(variable, value);
    emit dataChanged(QModelIndex(), QModelIndex());
  }

  void EnableColours(bool enable) {
    enable_colours_ = enable;
    emit dataChanged(QModelIndex(), QModelIndex());
  }
  void Sort();
  std::vector<QVariant> CalculateLevels(Variable variable);

  friend SummaryTableModel::Variable operator++(const SummaryTableModel::Variable& variable);

 signals:
  void RowVariableChanged(Variable variable);
  void ColumnVariableChanged(Variable variable);
  void DisplayVariableChanged(Variable variable);

 private:
  void CalculateLevels();

  std::vector<SingleResult> results_;
  Variable column_variable_ = Variable::kObject;
  Variable row_variable_ = Variable::kObstacle;
  Variable display_variable_ = Variable::kCage;
  std::vector<QVariant> column_variable_levels_;
  std::vector<QVariant> row_variable_levels_;
  SingleResult constants_ = {};
  TimeMode time_mode_ = TimeMode::kTimeMs;
  bool enable_colours_ = true;

  std::vector<QString> stats_headers_ = { "Min", "Max", "Mean", "Total" };
  int min_time_ = std::numeric_limits<int>::max();
  int max_time_ = 0;
};

inline bool IsFileVariable(const SummaryTableModel::Variable& variable) {
  return (variable == SummaryTableModel::Variable::kObject ||
          variable == SummaryTableModel::Variable::kObstacle ||
          variable == SummaryTableModel::Variable::kGrid);
}

inline SummaryTableModel::Variable operator++(SummaryTableModel::Variable& variable) {
  if (variable != SummaryTableModel::Variable::kNone) {
    variable = static_cast<SummaryTableModel::Variable>(static_cast<int>(variable) + 1);
  }
  return variable;
}

#endif // UI_MODELS_SUMMARY_TABLE_MODEL_H
