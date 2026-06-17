#include "history_store.h"

#include <QCoreApplication>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QJsonDocument>
#include <QJsonObject>
#include <QSaveFile>
#include <QStandardPaths>

#include <optional>

namespace s21 {
namespace {

constexpr char kHistoryFileName[] = "history.jsonl";

QJsonObject ToJsonObject(const HistoryRecord &record) {
  QJsonObject object;
  object["record_id"] = record.record_id;
  object["expression"] = record.expression;
  object["display_expression"] = record.display_expression;
  object["x_value"] = record.x_value;
  object["result"] = record.result;
  object["created_at"] = record.created_at;
  return object;
}

std::optional<HistoryRecord> FromJsonObject(const QJsonObject &object) {
  const QStringList required_fields = {"record_id", "expression",
                                       "display_expression", "x_value",
                                       "result", "created_at"};
  for (const QString &field : required_fields) {
    if (!object.contains(field) || !object.value(field).isString()) {
      return std::nullopt;
    }
  }

  HistoryRecord record;
  record.record_id = object.value("record_id").toString();
  record.expression = object.value("expression").toString();
  record.display_expression = object.value("display_expression").toString();
  record.x_value = object.value("x_value").toString();
  record.result = object.value("result").toString();
  record.created_at = object.value("created_at").toString();
  return record;
}

}  // namespace

HistoryStore::HistoryStore(const QString &file_path)
    : file_path_(file_path.isEmpty() ? ResolveDefaultFilePath() : file_path) {}

QString HistoryStore::FilePath() const { return file_path_; }

QString HistoryStore::ResolveDefaultFilePath() {
  const QString app_data_location =
      QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
  if (!app_data_location.isEmpty()) {
    QDir app_data_dir(app_data_location);
    if (app_data_dir.mkpath(".")) {
      return app_data_dir.filePath(kHistoryFileName);
    }
  }

  QString base_path = QCoreApplication::applicationDirPath();
  if (base_path.isEmpty()) {
    base_path = QDir::currentPath();
  }

  QDir fallback_dir(base_path);
  if (fallback_dir.mkpath("data")) {
    return fallback_dir.filePath(QStringLiteral("data/") + kHistoryFileName);
  }

  QDir current_dir(QDir::currentPath());
  current_dir.mkpath("data");
  return current_dir.filePath(QStringLiteral("data/") + kHistoryFileName);
}

bool HistoryStore::EnsureStorageDirectory(QString *error_message) const {
  if (file_path_.trimmed().isEmpty()) {
    if (error_message != nullptr) {
      *error_message = "History file path is empty";
    }
    return false;
  }

  QFileInfo file_info(file_path_);
  QDir directory = file_info.absoluteDir();
  if (directory.exists() || directory.mkpath(".")) {
    return true;
  }

  if (error_message != nullptr) {
    *error_message = "Unable to create history directory";
  }
  return false;
}

bool HistoryStore::ReadAll(std::vector<HistoryRecord> *records,
                           QString *error_message) const {
  if (records == nullptr) {
    if (error_message != nullptr) {
      *error_message = "History output container is null";
    }
    return false;
  }

  records->clear();
  QFile file(file_path_);
  if (!file.exists()) {
    return true;
  }
  if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
    if (error_message != nullptr) {
      *error_message = file.errorString();
    }
    return false;
  }

  while (!file.atEnd()) {
    const QByteArray line = file.readLine().trimmed();
    if (line.isEmpty()) {
      continue;
    }

    QJsonParseError parse_error;
    const QJsonDocument document = QJsonDocument::fromJson(line, &parse_error);
    if (parse_error.error != QJsonParseError::NoError ||
        !document.isObject()) {
      if (error_message != nullptr && error_message->isEmpty()) {
        *error_message = "History file contains damaged records";
      }
      continue;
    }

    std::optional<HistoryRecord> record = FromJsonObject(document.object());
    if (record.has_value()) {
      records->push_back(*record);
    } else if (error_message != nullptr && error_message->isEmpty()) {
      *error_message = "History file contains incomplete records";
    }
  }

  return true;
}

bool HistoryStore::WriteAll(const std::vector<HistoryRecord> &records,
                            QString *error_message) const {
  if (!EnsureStorageDirectory(error_message)) {
    return false;
  }

  QSaveFile file(file_path_);
  if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
    if (error_message != nullptr) {
      *error_message = file.errorString();
    }
    return false;
  }

  for (const HistoryRecord &record : records) {
    const QJsonDocument document(ToJsonObject(record));
    file.write(document.toJson(QJsonDocument::Compact));
    file.write("\n");
  }

  if (!file.commit()) {
    if (error_message != nullptr) {
      *error_message = file.errorString();
    }
    return false;
  }
  return true;
}

bool HistoryStore::Clear(QString *error_message) const {
  return WriteAll({}, error_message);
}

}  // namespace s21
