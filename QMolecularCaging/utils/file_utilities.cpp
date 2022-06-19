#include "utils/file_utilities.h"

QString Extension(QString path) {
  auto extension_position = path.lastIndexOf('.');
  if (extension_position == -1) {
    return path;
  } else {
    QString extension = path.right(path.size() - extension_position - 1);
    return extension.toLower();
  }
}

QString ReplaceExtension(QString path, QString extension) {
  auto extension_position = path.lastIndexOf('.');
  if (extension_position == -1) {
    return path;
  } else {
    return path.left(extension_position) + '.' + extension;
  }
}

QString FileDirectory(QString path) {
  for (int position = path.size() - 1; position >= 0; --position) {
    if (path[position] == '/' || path[position] == '\\') {
      return path.left(position);
    }
  }
  return QString();
}

QString FileName(QString path, bool include_extension) {
  for (int position = path.size() - 1; position >= 0; --position) {
    if (path[position] == '/' || path[position] == '\\') {
      path = path.right(path.size() - position - 1);
      break;
    }
  }
  auto extension_position = path.lastIndexOf('.');
  if (extension_position == -1 || include_extension) {
    return path;
  } else {
    return path.left(extension_position);
  }
}

QString FileName(std::string path, bool include_extension) {
  return FileName(QString::fromStdString(path), include_extension);
}
