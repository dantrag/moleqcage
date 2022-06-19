#include "ui/widgets/custom_tab_bar.h"

#include <QStyleOptionTab>
#include <QStylePainter>
#include <QLabel>

// This is a default constructor, it should eventually be called in either case
CustomTabBar::CustomTabBar(QWidget* parent)
    : QTabBar(parent), width_(kDefaultWidth_), height_(kDefaultHeight_) {
  setStyleSheet(QString(kDefaultStyleSheet_));
  helper_painter_ = new QStylePainter(this);
}

CustomTabBar::CustomTabBar(QWidget* parent, int tab_width, int tab_height)
    : QTabBar(parent), width_(tab_width), height_(tab_height) {}

CustomTabBar::CustomTabBar(QWidget* parent, QSize tab_size)
    : QTabBar(parent), width_(tab_size.width()), height_(tab_size.height()) {}

CustomTabBar::CustomTabBar(int tab_width, int tab_height)
    : QTabBar(), width_(tab_width), height_(tab_height) {}

CustomTabBar::CustomTabBar(QSize tab_size)
    : QTabBar(), width_(tab_size.width()), height_(tab_size.height()) {}

CustomTabBar::~CustomTabBar() {}

QSize CustomTabBar::getTabSize(int index) const {
  auto size = QTabBar::tabSizeHint(index);
  auto label = new QLabel(dynamic_cast<QWidget*>(this->parent()));
  label->setText(this->tabText(index));
  label->setStyleSheet(this->styleSheet());
  int w = std::max(label->sizeHint().width(), size.width());
  int h = std::max(label->sizeHint().height(), size.height());
  if (w > 2 * h) h = w / 2;
  else
  if (w < 2 * h) w = h * 2;
  delete label;
  return QSize(w, h);
}

int CustomTabBar::getMaximumTabHeight() const {
  int height = 0;
  for (int i = 0; i < this->count(); i++) {
    height = std::max(height, getTabSize(i).height());
  }
  return height;
}

QSize CustomTabBar::tabSizeHint(int index) const {
  auto size = getTabSize(index);
  size.setHeight(std::max(size.height(), getMaximumTabHeight()));
  return size;
}

void CustomTabBar::paintEvent(QPaintEvent *event) {
  QStylePainter painter(this);
  QStyleOptionTab option;

  for (int i = 0; i < this->count(); i++) {
    initStyleOption(&option, i);
    painter.drawControl(QStyle::CE_TabBarTabShape, option);
    painter.save();

    auto rect_size = option.rect.size();
    rect_size.transpose();
    QRect rect(QPoint(), rect_size);
    rect.moveCenter(option.rect.center());
    option.rect = rect;

    QPoint center = tabRect(i).center();
    painter.translate(center);
    painter.rotate(90);
    painter.translate(-center);
    painter.drawControl(QStyle::CE_TabBarTabLabel, option);
    painter.restore();
  }

  QWidget::paintEvent(event);
}
