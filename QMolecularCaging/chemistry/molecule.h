#ifndef CHEMISTRY_MOLECULE_H
#define CHEMISTRY_MOLECULE_H

#include <vector>
#include <tuple>

#include <QString>

#include "chemistry/element_collection.h"

class Atom {
 public:
  Atom(double x, double y, double z, int atomic_number)
      : x_(x), y_(y), z_(z), atomic_number_(atomic_number) {}

  Atom(double x, double y, double z, double radius)
      : x_(x), y_(y), z_(z), radius_(radius) {}

  double X() {
    return x_;
  }

  double Y() {
    return y_;
  }

  double Z() {
    return z_;
  }

  int AtomicNumber() {
    return atomic_number_;
  }

  double Radius() {
    return radius_;
  }

  void UpdateRadius(double radius);

 private:
   double x_ = 0.0;
   double y_ = 0.0;
   double z_ = 0.0;
   double radius_ = 0.0;
   int atomic_number_ = 0;
   std::vector<Atom*> neighbours_;
};

class Molecule : public QObject {
  Q_OBJECT

 public:
  Molecule(QString file_path, ElementCollection* elements);
  ~Molecule();

  bool IsLoaded() {
    return loaded_;
  }

  QString FilePath() {
    return file_path_;
  }

  bool Load();
  bool LoadFromBabelFormat();
  bool LoadFromXYZR();
  bool SaveToXYZR();
  void Unload();

  std::vector<Atom*> Atoms() {
    return atoms_;
  }

  std::tuple<double, double, double> MassCentre();

  friend class Converter;

 signals:
  void Loaded();
  void Unloaded();

 private:
  void Clear();

  bool loaded_ = false;
  QString file_path_;
  ElementCollection* elements_;

  int atom_count_ = 0;
  std::vector<Atom*> atoms_;
};

#endif // CHEMISTRY_MOLECULE_H
