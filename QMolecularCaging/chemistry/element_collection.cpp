#include "chemistry/element_collection.h"

ElementCollection::~ElementCollection() {
  for (auto& item : elements_) {
    delete item.second;
  }
}

Element* ElementCollection::operator[](int atomic_number) const {
  auto iterator = elements_.find(atomic_number);
  if (iterator == elements_.end()) {
    return nullptr;
  } else {
    return (*iterator).second;
  }
}

Element* ElementCollection::operator[](QString symbol) const {
  auto atomic_number = AtomicNumber(symbol);
  if (atomic_number > 0) {
    return (*this)[atomic_number];
  } else {
    return nullptr;
  }
}

const std::map<int, Element*>& ElementCollection::Items() {
  return elements_;
}

QString ElementCollection::Symbol(int atomic_number) const {
  if (symbol_mapping_.find(atomic_number) == symbol_mapping_.end()) {
    return "";
  } else {
    return symbol_mapping_.at(atomic_number);
  }
}

int ElementCollection::AtomicNumber(QString symbol) const {
  symbol = symbol.toLower();
  for (auto iterator : symbol_mapping_) {
    if (iterator.second.toLower() == symbol) {
      return iterator.first;
    }
  }
  return 0;
}

bool ElementCollection::AddItem(Element* element) {
  if (Symbol(element->AtomicNumber()) == "" && AtomicNumber(element->Symbol()) == 0) {
    elements_[element->AtomicNumber()] = element;
    symbol_mapping_[element->AtomicNumber()] = element->Symbol();

    connect(element, &Element::AtomicNumberChanged, this, &ElementCollection::OnAtomicNumbersChanged);
    connect(element, &Element::SymbolChanged, this, &ElementCollection::OnSymbolsChanged);
    connect(element, &Element::RadiusChanged, this, &ElementCollection::OnRadiiChanged);
    connect(element, &Element::ColourChanged, this, &ElementCollection::OnColoursChanged);
    return true;
  } else {
    return false;
  }
}

void ElementCollection::RemoveItem(int atomic_number) {
  elements_.erase(atomic_number);
  symbol_mapping_.erase(atomic_number);
}

QString ElementCollection::UniqueSymbol() {
  QString symbol("Z");
  while ((*this)[symbol] != nullptr) {
    symbol.append('Z');
  }
  return symbol;
}

int ElementCollection::UniqueAtomicNumber() {
  int number = 1;
  while ((*this)[number] != nullptr) {
    number++;
  }
  return number;
}

void ElementCollection::OnAtomicNumbersChanged() {
  symbol_mapping_.clear();
  std::vector<Element*> elements_storage;
  for (auto element : elements_) {
    elements_storage.push_back(element.second);
  }
  elements_.clear();
  for (auto element : elements_storage) {
    symbol_mapping_[element->AtomicNumber()] = element->Symbol();
    elements_[element->AtomicNumber()] = element;
  }
  emit AtomicNumbersChanged();
}

void ElementCollection::OnSymbolsChanged() {
  symbol_mapping_.clear();
  for (auto& element : elements_) {
    symbol_mapping_[element.second->AtomicNumber()] = element.second->Symbol();
  }
  emit SymbolsChanged();
}

void ElementCollection::OnRadiiChanged() {
  emit RadiiChanged();
}

void ElementCollection::OnColoursChanged() {
  emit ColoursChanged();
}
