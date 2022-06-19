#ifndef CHEMISTRY_MOLECULE_COLLECTION_H
#define CHEMISTRY_MOLECULE_COLLECTION_H

#include <memory>
#include <QMap>

#include <QString>
#include <QStringList>

#include "chemistry/molecule.h"

class MoleculeCollection : public QObject {
  Q_OBJECT

 public:
  MoleculeCollection(ElementCollection* elements)
      : elements_(elements) {}

  Molecule* operator[](QString file_path) const;

  void AddMolecule(QString file_path);
  void AddMolecules(QStringList file_paths = {});
  void RemoveMolecule(QString file_path);
  void RemoveMolecules(QStringList file_paths = {});

  void LoadAll(bool force_reload = false);

 signals:
  void MoleculeLoaded(QString file_path);
  void MoleculeUnloaded(QString file_path);

 private:
  std::map<QString, std::unique_ptr<Molecule>> collection_;
  ElementCollection* elements_;
};

#endif // CHEMISTRY_MOLECULE_COLLECTION_H
