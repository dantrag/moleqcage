#ifndef UTILS_FILE_UTILITIES_H
#define UTILS_FILE_UTILITIES_H

#include <string>

#include <QString>

QString Extension(QString path);
QString ReplaceExtension(QString path, QString extension);
QString FileDirectory(QString path);
QString FileName(QString path, bool include_extension = false);
QString FileName(std::string path, bool include_extension = false);

#endif // UTILS_FILE_UTILITIES_H
