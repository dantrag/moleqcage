#include "ui/widgets/custom_list_widget.h"

#include "utils/file_utilities.h"

CustomListWidget::CustomListWidget(QWidget* parent)
    : QListWidget(parent) {}

void CustomListWidget::AddCheckableFileItem(QString full_path, bool check) {
  addItem(full_path);
  const auto& list_item = item(count() - 1);
  list_item->setData(Qt::UserRole, full_path);
  list_item->setFlags(list_item->flags() | Qt::ItemIsUserCheckable);
  list_item->setCheckState(check ? Qt::Checked : Qt::Unchecked);
}

void CustomListWidget::AddCheckablePointerItem(QString item_text, void* pointer, bool check) {
  addItem(item_text);
  const auto& list_item = item(count() - 1);
  list_item->setData(Qt::UserRole, QVariant::fromValue(pointer));
  list_item->setFlags(list_item->flags() | Qt::ItemIsUserCheckable);
  list_item->setCheckState(check ? Qt::Checked : Qt::Unchecked);
}

void CustomListWidget::SetDisplayMode(bool show_full_path) {
  for (int row = 0; row < count(); ++row) {
    const auto& list_item = item(row);
    const auto full_path = list_item->data(Qt::UserRole).toString();
    if (show_full_path) {
      list_item->setText(full_path);
    } else {
      list_item->setText(FileName(full_path));
    }
  }
}

void CustomListWidget::RemoveItem(QString full_path) {
  for (int row = 0; row < count(); ++row) {
    if (item(row)->data(Qt::UserRole).toString() == full_path) {
      delete takeItem(row);
      break;
    }
  }
}

void CustomListWidget::keyPressEvent(QKeyEvent* event) {
  if (event->key() == Qt::Key_Space) {
    bool only_checked = true;
    for (auto& item : this->selectedItems()) {
      if (item->checkState() != Qt::Checked) {
        only_checked = false;
        break;
      }
    }
    Qt::CheckState check_state = only_checked ? Qt::Unchecked : Qt::Checked;
    for (auto& item : this->selectedItems()) {
      item->setCheckState(check_state);
    }
  } else {
    QAbstractItemView::keyPressEvent(event);
  }
}
