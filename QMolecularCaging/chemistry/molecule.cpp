#include "chemistry/molecule.h"

#include <iostream>
#include <sstream>

#include <QStringList>
#include <openbabel/obconversion.h>
#include <openbabel/mol.h>
#include <openbabel/atom.h>

#include "utils/file_utilities.h"

void Atom::UpdateRadius(double radius) {
  radius_ = radius;
}

Molecule::Molecule(QString file_path, ElementCollection* elements) {
  file_path_ = file_path;
  elements_ = elements;
}

Molecule::~Molecule() {
  Clear();
}

bool Molecule::Load() {
  auto extension = Extension(file_path_);
  if (extension == "pdb") {
    return LoadFromBabelFormat();
  }
  if (extension == "mol2") {
    return LoadFromBabelFormat();
  }
  if (extension == "mol") {
    return LoadFromBabelFormat();
  }
  if (extension == "xyzr") {
    return LoadFromXYZR();
  }
  if (extension == "txt") {
    if (LoadFromXYZR()) {
      if (SaveToXYZR()) {
        return true;
      } else {
        Unload();
      }
    }
    return false;
  }
  return false;
}

bool Molecule::LoadFromBabelFormat() {
  Unload();

  /*
   * This is a fix to correct OpenBabel's inappropriate use of scanf which
   * might fail on systems using comma-sepatated numeric representation
   */
  setlocale(LC_NUMERIC, "C");

  std::ifstream input_stream(file_path_.toStdString());

  try {
    if (input_stream) {
      std::stringstream output_stream;
      OpenBabel::OBConversion converter(&input_stream, &output_stream);
      converter.SetInFormat(OpenBabel::OBConversion::FormatFromExt(file_path_.toStdString()));
      converter.SetOutFormat("XYZ");
      converter.AddOption("c", OpenBabel::OBConversion::GENOPTIONS);
      if (converter.Convert() > 0) {
        OpenBabel::OBMol* molecule = new OpenBabel::OBMol();
        converter.SetInFormat("XYZ");
        if (converter.Read(molecule, &output_stream)) {
          for (auto iterator = molecule->BeginAtoms(); iterator != molecule->EndAtoms(); ++iterator) {
            auto x = (*iterator)->GetX();
            auto y = (*iterator)->GetY();
            auto z = (*iterator)->GetZ();
            auto atomic_number = static_cast<int>((*iterator)->GetAtomicNum());
            atoms_.push_back(new Atom(x, y, z, atomic_number));
            atoms_[atoms_.size() - 1]->UpdateRadius((*elements_)[atomic_number]->Radius());
          }
          loaded_ = true;
        }
      }
    }
  } catch (...) {
  }

  // TODO: add meaningful error return value
  if (loaded_) {
    SaveToXYZR();
    emit Loaded();
  } else {
    emit Unloaded();
  }
  return loaded_;
}

bool Molecule::LoadFromXYZR() {
  Unload();

  std::ifstream input_stream(file_path_.toStdString());

  try {
    if (input_stream) {
      int atom_count = 0;
      input_stream >> atom_count;
      while (atom_count--) {
        double x = 0.0;
        double y = 0.0;
        double z = 0.0;
        double radius = 0.0;
        input_stream >> x >> y >> z >> radius;
        atoms_.push_back(new Atom(x, y, z, radius));
      }
      loaded_ = true;
      input_stream.close();
    }
  } catch (...) {
  }

  // TODO: add meaningful error return value
  if (loaded_) {
    emit Loaded();
  } else {
    emit Unloaded();
  }
  return loaded_;
}

bool Molecule::SaveToXYZR() {
  if (!loaded_) {
    return false;
  }

  auto file_path = FileDirectory(file_path_) + "/" + FileName(file_path_) + ".xyzr";
  std::ofstream output_stream(file_path.toStdString());

  try {
    if (output_stream) {
      int atom_count = atoms_.size();
      output_stream << atom_count << std::endl;
      while (atom_count--) {
        const auto& atom = atoms_[atom_count];
        output_stream << atom->X() << " " << atom->Y() << " " << atom->Z() << " " <<
                         atom->Radius() << std::endl;
      }
      output_stream.close();
    }
    return true;
  } catch (...) {
    return false;
  }
}

void Molecule::Unload() {
  if (loaded_) {
    emit Unloaded();
  }
  loaded_ = false;
}

std::tuple<double, double, double> Molecule::MassCentre() {
  double x = 0.0;
  double y = 0.0;
  double z = 0.0;
  for (auto& atom : atoms_) {
    x += atom->X();
    y += atom->Y();
    z += atom->Z();
  }

  if (!atoms_.empty()) {
    x /= atoms_.size();
    y /= atoms_.size();
    z /= atoms_.size();
  }
  return std::make_tuple(x, y, z);
}

void Molecule::Clear() {
  for (auto& atom : atoms_) {
    delete atom;
  }
  atoms_.clear();
}
