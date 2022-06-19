#ifndef CHEMISTRY_ELEMENT_COLLECTION_H
#define CHEMISTRY_ELEMENT_COLLECTION_H

#include <map>

#include <QString>
#include <QColor>
#include <QObject>

class Element : public QObject {
  Q_OBJECT

 public:
  Element() {}
  Element(int atomic_number, QString symbol, double radius, QColor colour = QColor(Qt::gray))
      : QObject(), atomic_number_(atomic_number), symbol_(symbol), radius_(radius), colour_(colour) {}

  inline int AtomicNumber() const {
    return atomic_number_;
  }
  inline void SetAtomicNumber(int atomic_number) {
    atomic_number_ = atomic_number;
    emit AtomicNumberChanged();
  }
  inline QString Symbol() const {
    return symbol_;
  }
  inline void SetSymbol(QString symbol) {
    symbol_ = symbol;
    emit SymbolChanged();
  }
  inline double Radius() const {
    return radius_;
  }
  inline void SetRadius(double radius) {
    radius_ = radius;
    emit RadiusChanged();
  }
  inline QColor Colour() const {
    return colour_;
  }
  inline void SetColour(QColor colour) {
    colour_ = colour;
    emit ColourChanged();
  }

 signals:
  void AtomicNumberChanged();
  void SymbolChanged();
  void RadiusChanged();
  void ColourChanged();

 private:
  int atomic_number_ = 0;
  QString symbol_;
  double radius_ = 0.0;
  QColor colour_;
};

class ElementCollection : public QObject {
  Q_OBJECT

 public:
  ~ElementCollection();

  Element* operator[](int atomic_number) const;
  Element* operator[](QString symbol) const;

  int size() {
    return static_cast<int>(elements_.size());
  }

  const std::map<int, Element*>& Items();

  QString Symbol(int atomic_number) const;
  int AtomicNumber(QString symbol) const;

  bool AddItem(Element* element);
  void RemoveItem(int atomic_number);

  QString UniqueSymbol();
  int UniqueAtomicNumber();

 signals:
  void AtomicNumbersChanged();
  void SymbolsChanged();
  void RadiiChanged();
  void ColoursChanged();

 private slots:
  void OnAtomicNumbersChanged();
  void OnSymbolsChanged();
  void OnRadiiChanged();
  void OnColoursChanged();

 private:
  std::map<int, Element*> elements_;
  std::map<int, QString> symbol_mapping_;
};

#endif // CHEMISTRY_ELEMENT_COLLECTION_H
