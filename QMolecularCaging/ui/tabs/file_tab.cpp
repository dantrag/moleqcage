#include "ui/tabs/file_tab.h"

#include <set>

#include <QColorDialog>
#include <QFileDialog>
#include <QMessageBox>

#include "ui_file_tab.h"

FileTab::FileTab(MoleculeCollection* all_molecules, ElementCollection* all_elements, QWidget* parent)
    : QWidget(parent), ui_(new Ui::FileTab), molecules_(all_molecules), elements_(all_elements) {
  ui_->setupUi(this);

  FillElementTypes();
  ui_->element_table->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);

  auto sp = ui_->preprocess_group->sizePolicy();
  sp.setHorizontalStretch(1);
  ui_->preprocess_group->setSizePolicy(sp);
  sp = ui_->file_list_group->sizePolicy();
  sp.setHorizontalStretch(3);
  ui_->file_list_group->setSizePolicy(sp);

  // File selection
  connect(ui_->add_file_button, &QPushButton::clicked, this, &FileTab::AddFiles);
  connect(ui_->show_full_path_check_box, &QPushButton::clicked, this, &FileTab::UpdateFileList);
  connect(ui_->file_list_widget, &CustomListWidget::itemChanged, this, &FileTab::FileListChanged);
  connect(ui_->remove_checked_file_button, &QPushButton::clicked, this, &FileTab::RemoveCheckedFiles);

  // Conversion
  connect(ui_->element_table, &QTableWidget::cellChanged, this, &FileTab::ElementTableCellChanged);
  connect(ui_->element_table, &QTableWidget::cellDoubleClicked, this, &FileTab::ElementTableCellDoubleClicked);
  connect(ui_->new_element_button, &QPushButton::clicked, this, &FileTab::NewElement);

  connect(ui_->load_molecules, &QPushButton::clicked, this, &FileTab::LoadMolecules);
  connect(ui_->show_preprocessing_options, &QPushButton::toggled, ui_->preprocess_group, &QGroupBox::setVisible);
}

void FileTab::UpdateFileList() {
  ui_->file_list_widget->SetDisplayMode(ui_->show_full_path_check_box->isChecked());
}

void FileTab::PopulateFileList(QListWidget* list_widget, const QStringList full_path_list,
                               const bool replace, const QBitArray selection) {
  if (!full_path_list.isEmpty()) {
    if (replace) {
      list_widget->clear();
    }
    list_widget->addItems(full_path_list);
    molecules_->AddMolecules(full_path_list);
    for (int i = 0; i < full_path_list.size(); ++i) {
      int row = list_widget->count() - i - 1;
      const auto& list_item = list_widget->item(row);
      list_item->setData(Qt::UserRole, full_path_list[i]);
      list_item->setFlags(list_item->flags() | Qt::ItemIsUserCheckable);
      if (row < selection.size()) {
        list_item->setCheckState(selection[row] ? Qt::Checked : Qt::Unchecked);
      } else {
        list_item->setCheckState(Qt::Checked);
      }
      list_item->setSelected(true);
    }
  }

  UpdateFileList();
}

void FileTab::AddFiles() {
  auto files_to_add = QFileDialog::getOpenFileNames(this, "Select files with molecules", QString(),
                                                    "All files (*)");
  if (!files_to_add.empty()) {
    bool replace = false;
    if (ui_->file_list_widget->count() > 0) {
      QMessageBox msgbox(this);
      msgbox.setWindowTitle("Add files...");
      msgbox.setText(QString("%1 new files are to be added, would you like to replace already present entries?")
                     .arg(files_to_add.size()));
      msgbox.addButton(QString("Replace"), QMessageBox::YesRole);
      msgbox.addButton(QString("Append"), QMessageBox::NoRole);
      msgbox.addButton(QString("Cancel"), QMessageBox::RejectRole);
      QMessageBox::ButtonRole further_action = QMessageBox::NoRole;
      msgbox.exec();
      further_action = msgbox.buttonRole(msgbox.clickedButton());

      switch (further_action) {
        case QMessageBox::RejectRole : {
          /* return point */
          return;
        }
        case QMessageBox::YesRole : {
          replace = true;
          break;
        }
        case QMessageBox::NoRole : {
          replace = false;
          break;
        }
        default : {
          replace = false;
        }
      }
    }
    PopulateFileList(ui_->file_list_widget, files_to_add, replace);
  }
}

void FileTab::FileListChanged() {
  for (int row = 0; row < ui_->file_list_widget->count(); ++row) {
    if (ui_->file_list_widget->item(row)->checkState() == Qt::Checked) {
      ui_->remove_checked_file_button->setEnabled(true);
      return;
    }
  }
  ui_->remove_checked_file_button->setEnabled(false);
}

void FileTab::RemoveCheckedFiles() {
  for (int row = ui_->file_list_widget->count() - 1; row >= 0; --row) {
    if (ui_->file_list_widget->item(row)->checkState() == Qt::Checked) {
      //molecules_->RemoveMolecule(ui_->file_list_widget->item(row)->data(Qt::UserRole).toString());
      delete ui_->file_list_widget->takeItem(row);
    }
  }
  FileListChanged();
}

void FileTab::AddElementRow(int index, int atomic_number, QString symbol, double radius, QColor colour) {
  ui_->element_table->blockSignals(true);

  auto dummy_item = new QTableWidgetItem();
  dummy_item->setTextAlignment(Qt::AlignCenter);

  auto new_item = dummy_item->clone();
  new_item->setText(QString::number(atomic_number));
  ui_->element_table->setItem(index, static_cast<int>(ElementTableColumn::AtomicNumber), new_item);

  new_item = dummy_item->clone();
  new_item->setText(symbol);
  ui_->element_table->setItem(index, static_cast<int>(ElementTableColumn::Symbol), new_item);

  new_item = dummy_item->clone();
  new_item->setText(QString::number(radius, 'f', 2));
  ui_->element_table->setItem(index, static_cast<int>(ElementTableColumn::Radius), new_item);

  new_item = dummy_item->clone();
  new_item->setBackground(colour);
  new_item->setFlags(new_item->flags() ^ Qt::ItemIsEditable);
  ui_->element_table->setItem(index, static_cast<int>(ElementTableColumn::Colour), new_item);

  delete dummy_item;

  ui_->element_table->blockSignals(false);
}

void FileTab::FillElementTypes() {
  elements_->AddItem(new Element(1, "H", 1.20, Qt::white));
  elements_->AddItem(new Element(6, "C", 1.70, Qt::gray));
  elements_->AddItem(new Element(7, "N", 1.55, Qt::blue));
  elements_->AddItem(new Element(8, "O", 1.52, Qt::red));
  elements_->AddItem(new Element(9, "F", 1.47, Qt::green));
  elements_->AddItem(new Element(15, "P", 1.80, QColor(0xfd771e)));
  elements_->AddItem(new Element(16, "S", 1.80, QColor(0xe2ff36)));
  elements_->AddItem(new Element(17, "Cl", 1.75, Qt::darkGreen));
  elements_->AddItem(new Element(35, "Br", 1.85, "brown"));
  elements_->AddItem(new Element(53, "I", 1.98, Qt::darkMagenta));
  elements_->AddItem(new Element(2, "He", 1.40, QColor(0xd5ffff)));
  elements_->AddItem(new Element(3, "Li", 1.40, QColor(0xfbff2a)));
  elements_->AddItem(new Element(4, "Be", 1.40, QColor(0xbfff29)));
  elements_->AddItem(new Element(5, "B", 1.40, QColor(0xfaaab1)));
  elements_->AddItem(new Element(10, "Ne", 1.54, QColor(0xadeff3)));
  elements_->AddItem(new Element(11, "Na", 2.27, QColor(0xddeeff)));
  elements_->AddItem(new Element(12, "Mg", 1.73, QColor(0x9ae629)));
  elements_->AddItem(new Element(13, "Al", 2.00, QColor(0xe39a9f)));
  elements_->AddItem(new Element(14, "Si", 2.10, QColor(0xeec8a0)));
  elements_->AddItem(new Element(18, "Ar", 1.88, QColor(0x8cdae3)));
  elements_->AddItem(new Element(19, "K", 2.75, QColor(0x895bb2)));
  elements_->AddItem(new Element(20, "Ca", 2.00, QColor(0x75ca28)));
  elements_->AddItem(new Element(21, "Sc", 2.00, QColor(0xe3e3e3)));
  elements_->AddItem(new Element(22, "Ti", 2.00, QColor(0xbec0c6)));
  elements_->AddItem(new Element(23, "V", 2.00, QColor(0xa5a4a9)));
  elements_->AddItem(new Element(24, "Cr", 2.00, QColor(0x8b92c5)));
  elements_->AddItem(new Element(25, "Mn", 2.00, QColor(0x9d69c5)));
  elements_->AddItem(new Element(26, "Fe", 2.00, QColor(0xde5a3a)));
  elements_->AddItem(new Element(27, "Co", 2.00, QColor(0xef839f)));
  elements_->AddItem(new Element(28, "Ni", 1.63, QColor(0x4add54)));
  elements_->AddItem(new Element(29, "Cu", 1.40, QColor(0xc57e3a)));
  elements_->AddItem(new Element(30, "Zn", 1.39, QColor(0x7e78ae)));
  elements_->AddItem(new Element(31, "Ga", 1.87, QColor(0xca8387)));
  elements_->AddItem(new Element(32, "Ge", 2.00, QColor(0x98a292)));
  elements_->AddItem(new Element(33, "As", 1.85, QColor(0xbe65e1)));
  elements_->AddItem(new Element(34, "Se", 1.90, QColor(0xe7a927)));
  elements_->AddItem(new Element(36, "Kr", 2.02, QColor(0x8fc0cd)));
  elements_->AddItem(new Element(37, "Rb", 2.00, QColor(0x6e4793)));
  elements_->AddItem(new Element(38, "Sr", 2.00, QColor(0x4faf25)));
  elements_->AddItem(new Element(39, "Y", 2.00, QColor(0x88fffe)));
  elements_->AddItem(new Element(40, "Zr", 2.00, QColor(0x76efea)));
  elements_->AddItem(new Element(41, "Nb", 2.00, QColor(0x64d9d5)));
  elements_->AddItem(new Element(42, "Mo", 2.00, QColor(0x53c3c2)));
  elements_->AddItem(new Element(43, "Tc", 2.00, QColor(0x42adb1)));
  elements_->AddItem(new Element(44, "Ru", 2.00, QColor(0x3197a1)));
  elements_->AddItem(new Element(45, "Rh", 2.00, QColor(0x228092)));
  elements_->AddItem(new Element(46, "Pd", 1.63, QColor(0x176a84)));
  elements_->AddItem(new Element(47, "Ag", 1.72, QColor(0xbfbfbf)));
  elements_->AddItem(new Element(48, "Cd", 1.58, QColor(0xfddc90)));
  elements_->AddItem(new Element(49, "In", 1.93, QColor(0xb8696c)));
  elements_->AddItem(new Element(50, "Sn", 2.17, QColor(0x647c7c)));
  elements_->AddItem(new Element(51, "Sb", 2.00, QColor(0x9f4cb3)));
  elements_->AddItem(new Element(52, "Te", 2.06, QColor(0xce6f1a)));
  elements_->AddItem(new Element(54, "Xe", 2.16, QColor(0x55a3b3)));
  elements_->AddItem(new Element(55, "Cs", 2.00, QColor(0x582779)));
  elements_->AddItem(new Element(56, "Ba", 2.00, QColor(0x279820)));
  elements_->AddItem(new Element(57, "La", 2.00, QColor(0xeab959)));
  elements_->AddItem(new Element(58, "Ce", 2.00, QColor(0xe2be58)));
  elements_->AddItem(new Element(59, "Pr", 2.00, QColor(0xdab855)));

  ui_->element_table->setRowCount(elements_->size());
  int index = 0;
  for (auto item = elements_->Items().begin(); item != elements_->Items().end(); ++index, ++item) {
    auto& element = item->second;
    AddElementRow(index, element->AtomicNumber(), element->Symbol(), element->Radius(), element->Colour());
  }
}

void FileTab::ElementTableCellChanged(int row, int column) {
  switch (static_cast<ElementTableColumn>(column)) {
    case ElementTableColumn::AtomicNumber: {
      if (ui_->element_table->item(row, column)->text().isEmpty()) {
        QMessageBox msgbox;
        msgbox.setWindowTitle("Empty value!");
        msgbox.setText("Element atomic number cannot be empty.");
        msgbox.exec();
        ui_->element_table->item(row, column)->setText(QString::number(elements_->UniqueAtomicNumber()));
        break;
      }
      auto atomic_number = ui_->element_table->item(row, column)->text().toInt();
      auto symbol = ui_->element_table->item(row,
                     static_cast<int>(ElementTableColumn::Symbol))->text();
      if ((*elements_)[atomic_number] != nullptr && (*elements_)[atomic_number]->Symbol() != symbol) {
        QMessageBox msgbox;
        msgbox.setWindowTitle("Already existing atomic number!");
        msgbox.setText("This atomic number already belongs to one of the elements. Please choose another.");
        msgbox.exec();
        ui_->element_table->item(row, column)->setText(QString::number(elements_->UniqueAtomicNumber()));
      } else {
        (*elements_)[symbol]->SetAtomicNumber(atomic_number);
      }
      break;
    }
    case ElementTableColumn::Symbol: {
      auto atomic_number = ui_->element_table->item(row,
                            static_cast<int>(ElementTableColumn::AtomicNumber))->text().toInt();
      auto symbol = ui_->element_table->item(row, column)->text();
      if (symbol == "") {
        QMessageBox msgbox;
        msgbox.setWindowTitle("Empty symbol!");
        msgbox.setText("Element symbol cannot be empty.");
        msgbox.exec();
        ui_->element_table->item(row, column)->setText(elements_->UniqueSymbol());
        break;
      }
      if ((*elements_)[symbol] != nullptr && (*elements_)[symbol]->AtomicNumber() != atomic_number) {
        QMessageBox msgbox;
        msgbox.setWindowTitle("Already existing symbol!");
        msgbox.setText("This symbol already belongs to one of the elements. Please choose another.");
        msgbox.exec();
        ui_->element_table->item(row, column)->setText(elements_->UniqueSymbol());
      } else {
        (*elements_)[atomic_number]->SetSymbol(symbol);
      }
      break;
    }
    case ElementTableColumn::Radius: {
      auto atomic_number = ui_->element_table->item(row,
                            static_cast<int>(ElementTableColumn::AtomicNumber))->text().toInt();
      (*elements_)[atomic_number]->SetRadius(ui_->element_table->item(row, column)->text().toDouble());
      ui_->element_table->item(row, column)->setText(QString::number((*elements_)[atomic_number]->Radius(), 'g', 2));
      break;
    }
    default: {
      return;
    }
  }
}

void FileTab::ElementTableCellDoubleClicked(int row, int column) {
  if (static_cast<ElementTableColumn>(column) == ElementTableColumn::Colour) {
    auto new_colour = QColorDialog::getColor(ui_->element_table->item(row, column)->backgroundColor(), this);
    if (new_colour.isValid()) {
      auto symbol = ui_->element_table->item(row, static_cast<int>(ElementTableColumn::Symbol))->text();
      ui_->element_table->item(row, column)->setBackground(new_colour);
      (*elements_)[symbol]->SetColour(new_colour);
    }
  }
}

void FileTab::NewElement() {
  ui_->element_table->insertRow(ui_->element_table->rowCount());

  auto symbol = (*elements_).UniqueSymbol();
  auto atomic_number = (*elements_).UniqueAtomicNumber();

  elements_->AddItem(new Element(atomic_number, symbol, 1.5, Qt::white));
  AddElementRow(ui_->element_table->rowCount() - 1, atomic_number, symbol, 1.5, Qt::white);
}

void FileTab::LoadMolecules() {
  if (ui_->delete_loaded_check_box->isChecked()) {
    molecules_->RemoveMolecules();
  }

  QStringList files;
  for (int row = 0; row < ui_->file_list_widget->count(); ++row) {
    auto list_item = ui_->file_list_widget->item(row);
    if (list_item->checkState() == Qt::Checked) {
      const auto file_path = list_item->data(Qt::UserRole).toString();
      files.push_back(file_path);
      list_item->setCheckState(Qt::Unchecked);
    }
  }
  molecules_->AddMolecules(files);
  molecules_->LoadAll();
}
