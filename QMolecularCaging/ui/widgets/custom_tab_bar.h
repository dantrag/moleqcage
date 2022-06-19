#ifndef UI_WIDGETS_CUSTOM_TAB_BAR_H
#define UI_WIDGETS_CUSTOM_TAB_BAR_H

#include <QTabBar>
#include <QStylePainter>

class CustomTabBar : public QTabBar {
 public:
  CustomTabBar(QWidget* parent = 0);
  CustomTabBar(QWidget* parent, int tab_width, int tab_height);
  CustomTabBar(QWidget* parent, QSize tab_size);
  CustomTabBar(int tab_width, int tab_height);
  CustomTabBar(QSize tab_size);
  ~CustomTabBar();

  QSize tabSizeHint(int index) const override;

 protected:
  void paintEvent(QPaintEvent* event) override;

  static constexpr int kDefaultWidth_ = 90;
  static constexpr int kDefaultHeight_ = 90;

  static constexpr auto kDefaultStyleSheet_ = "QTabBar::tab {\
                                                 color: #000000;\
                                                 font-weight: bold;\
                                                 font-family: Gotham, Helvetica Neue, Helvetica, Arial, sans-serif;\
                                               }\
                                               QTabBar::tab:selected {\
                                                 background-color: #E0E0E0;\
                                                 color: #000000;\
                                                 border: 1px solid #D0D0D0;\
                                                }\
                                               QTabBar::tab:!selected {\
                                                 background-color: #F0F0F0;\
                                                 color: #000000;\
                                                 border: 1px solid #D0D0D0;\
                                               }\
                                               QTabBar::tab:hover:selected {\
                                                 color: #000000;\
                                                 border: 1px solid #D0D0D0;\
                                                 background-color: #E8E8E8;\
                                               }\
                                               QTabBar::tab:hover:!selected {\
                                                 color: #000000;\
                                                 border: 1px solid #D0D0D0;\
                                                 background-color: #F4F4F4;\
                                               }";

 private:
  QSize getTabSize(int index) const;
  int getMaximumTabHeight() const;

  int width_ = kDefaultWidth_;
  int height_ = kDefaultHeight_;

  QStylePainter* helper_painter_ = nullptr;
};

#endif // UI_WIDGETS_CUSTOM_TAB_BAR_H
