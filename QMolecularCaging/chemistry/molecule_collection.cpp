#include "molecule_collection.h"

// TODO: paths are case-sensitive in Linux, case-insensitive in Windows

Molecule* MoleculeCollection::operator[](QString file_path) const {
  auto iterator = collection_.find(file_path);
  if (iterator == collection_.end()) {
    return nullptr;
  } else {
    return (*iterator).second.get();
  }
}

void MoleculeCollection::RemoveMolecule(QString file_path) {
  collection_[file_path].get()->blockSignals(true);
  collection_.erase(file_path);
  emit MoleculeUnloaded(file_path);
}

void MoleculeCollection::RemoveMolecules(QStringList file_paths) {
  if (file_paths.empty()) {
    for (const auto& molecule : collection_) {
      file_paths.push_back(molecule.second->FilePath());
    }
  }
  for (const auto& file_path : file_paths) {
    RemoveMolecule(file_path);
  }
}

void MoleculeCollection::AddMolecule(QString file_path) {
  collection_[file_path] = std::unique_ptr<Molecule>(new Molecule(file_path, elements_));
  connect(collection_[file_path].get(), &Molecule::Loaded, [=]() {
    emit MoleculeCollection::MoleculeLoaded(file_path);
  });
  connect(collection_[file_path].get(), &Molecule::Unloaded, [=]() {
    emit MoleculeCollection::MoleculeUnloaded(file_path);
  });
}

void MoleculeCollection::AddMolecules(QStringList file_paths) {
  for (const auto& file_path : file_paths) {
    AddMolecule(file_path);
  }
}

void MoleculeCollection::LoadAll(bool force_reload) {
  for (const auto& molecule : collection_) {
    if (force_reload || !molecule.second->IsLoaded()) {
      molecule.second->Load();
    }
  }
}
