#ifndef UI_WIDGETS_CUSTOM_TAB_WIDGET_H
#define UI_WIDGETS_CUSTOM_TAB_WIDGET_H

#include <QTabWidget>

class QCustomTabWidget : public QTabWidget {
 public:
  QCustomTabWidget(QWidget* parent = 0);
  QCustomTabWidget(QWidget* parent, int width, int height);
  QCustomTabWidget(QWidget* parent, QSize size);
  ~QCustomTabWidget();

  void SetTabControlSize(int width, int height);
  void SetTabControlSize(QSize tab_size);

private:
  int tab_width_ = 0;
  int tab_height_ = 0;
  QSize tab_size_ = {};
};

#endif // UI_WIDGETS_CUSTOM_TAB_WIDGET_H
