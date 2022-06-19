#include "ui/widgets/custom_tab_widget.h"

#include "ui/widgets/custom_tab_bar.h"

QCustomTabWidget::QCustomTabWidget(QWidget* parent)
    : QTabWidget(parent) {
  this->setTabBar(new CustomTabBar(parent));
  this->setTabPosition(QTabWidget::West);
}

QCustomTabWidget::QCustomTabWidget(QWidget* parent, int width, int height)
    : QTabWidget(parent), tab_width_(width), tab_height_(height) {
  this->setTabBar(new CustomTabBar(parent, tab_width_, tab_height_));
  this->setTabPosition(QTabWidget::West);
}

QCustomTabWidget::QCustomTabWidget(QWidget* parent, QSize size)
    : QTabWidget(parent), tab_size_(size) {
  this->setTabBar(new CustomTabBar(parent, tab_size_));
  this->setTabPosition(QTabWidget::West);
}

void QCustomTabWidget::SetTabControlSize(int width, int height) {
  tab_width_ = width;
  tab_height_ = height;
}

void QCustomTabWidget::SetTabControlSize(QSize size) {
  tab_size_ = size;
}

QCustomTabWidget::~QCustomTabWidget() {}
