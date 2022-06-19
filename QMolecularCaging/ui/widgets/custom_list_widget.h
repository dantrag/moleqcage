#ifndef UI_WIDGETS_CUSTOM_LIST_WIDGET_H
#define UI_WIDGETS_CUSTOM_LIST_WIDGET_H

#include <QListWidget>
#include <QKeyEvent>

class CustomListWidget : public QListWidget {
 public:
  CustomListWidget(QWidget* parent = nullptr);

  void AddCheckableFileItem(QString full_path, bool check = true);
  void AddCheckablePointerItem(QString item_text, void* pointer, bool check = true);

  void SetDisplayMode(bool show_full_path);
  void RemoveItem(QString full_path);

 protected:
  virtual void keyPressEvent(QKeyEvent* event);
};

#endif // UI_WIDGETS_CUSTOM_LIST_WIDGET_H
