#ifndef UI_TABS_FILE_TAB_H
#define UI_TABS_FILE_TAB_H

#include <map>

#include <QBitArray>
#include <QListWidget>
#include <QStringList>
#include <QWidget>

#include "chemistry/element_collection.h"
#include "chemistry/molecule_collection.h"

namespace Ui {
class FileTab;
}

class FileTab : public QWidget {
  Q_OBJECT

 public:
  FileTab(MoleculeCollection* all_molecules,  ElementCollection* all_elements, QWidget* parent = nullptr);

 signals:
  void RadiusChanged();

 private slots:
  void AddFiles();
  void UpdateFileList();
  void FileListChanged();
  void RemoveCheckedFiles();

  void ElementTableCellChanged(int row, int column);
  void ElementTableCellDoubleClicked(int row, int column);
  void NewElement();

  void LoadMolecules();

 private:
  enum class ElementTableColumn {
    AtomicNumber = 0,
    Symbol,
    Radius,
    Colour
  };

  void PopulateFileList(QListWidget* list_widget, const QStringList full_path_list,
                        const bool replace, const QBitArray selection = {});
  void AddElementRow(int index, int atomic_number, QString symbol, double radius, QColor colour);
  void FillElementTypes();

  Ui::FileTab* ui_ = nullptr;
  MoleculeCollection* molecules_ = nullptr;
  ElementCollection* elements_ = nullptr;
};

#endif // UI_TABS_FILE_TAB_H
