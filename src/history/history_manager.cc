#include "history_manager.h"

#include <QDateTime>
#include <QUuid>

#include <algorithm>
#include <cmath>

namespace s21 {

HistoryManager::HistoryManager(const QString &file_path, int max_records)
    : store_(file_path), max_records_(std::max(1, max_records)) {}

bool HistoryManager::AppendHistory(HistoryRecord record) {
  ResetError();
  if (record.record_id.trimmed().isEmpty()) {
    record.record_id = QUuid::createUuid().toString().remove('{').remove('}');
  }
  if (record.created_at.trimmed().isEmpty()) {
    record.created_at =
        QDateTime::currentDateTimeUtc().toString(Qt::ISODateWithMs);
  }
  if (!IsValidRecord(record)) {
    SetError(HistoryErrorCode::kInvalidRecord,
             "History record contains invalid calculation data");
    return false;
  }

  QString error_message;
  std::vector<HistoryRecord> records;
  if (!store_.ReadAll(&records, &error_message)) {
    SetError(HistoryErrorCode::kReadFailed, error_message);
    return false;
  }

  records.push_back(record);
  if (static_cast<int>(records.size()) > max_records_) {
    records.erase(records.begin(), records.end() - max_records_);
  }

  if (!store_.WriteAll(records, &error_message)) {
    SetError(HistoryErrorCode::kWriteFailed, error_message);
    return false;
  }
  return true;
}

std::vector<HistoryRecord> HistoryManager::LoadRecent(int limit) {
  ResetError();
  if (limit <= 0) {
    return {};
  }

  QString error_message;
  std::vector<HistoryRecord> records;
  if (!store_.ReadAll(&records, &error_message)) {
    SetError(HistoryErrorCode::kReadFailed, error_message);
    return {};
  }

  if (static_cast<int>(records.size()) > limit) {
    records.erase(records.begin(), records.end() - limit);
  }
  std::reverse(records.begin(), records.end());
  return records;
}

bool HistoryManager::ClearHistory() {
  ResetError();
  QString error_message;
  if (!store_.Clear(&error_message)) {
    SetError(HistoryErrorCode::kWriteFailed, error_message);
    return false;
  }
  return true;
}

bool HistoryManager::TrimToLimit(int limit) {
  ResetError();
  if (limit <= 0) {
    SetError(HistoryErrorCode::kInvalidRecord,
             "History limit must be greater than zero");
    return false;
  }

  QString error_message;
  std::vector<HistoryRecord> records;
  if (!store_.ReadAll(&records, &error_message)) {
    SetError(HistoryErrorCode::kReadFailed, error_message);
    return false;
  }

  if (static_cast<int>(records.size()) > limit) {
    records.erase(records.begin(), records.end() - limit);
  }
  if (!store_.WriteAll(records, &error_message)) {
    SetError(HistoryErrorCode::kWriteFailed, error_message);
    return false;
  }
  return true;
}

std::optional<HistoryRecord> HistoryManager::GetRecordById(
    const QString &record_id) {
  const std::vector<HistoryRecord> records = LoadRecent(max_records_);
  const auto match = std::find_if(
      records.begin(), records.end(), [&record_id](const HistoryRecord &record) {
        return record.record_id == record_id;
      });
  if (match == records.end()) {
    return std::nullopt;
  }
  return *match;
}

HistoryErrorInfo HistoryManager::GetLastError() const { return last_error_; }

QString HistoryManager::FilePath() const { return store_.FilePath(); }

bool HistoryManager::IsValidRecord(const HistoryRecord &record) const {
  if (record.expression.trimmed().isEmpty() ||
      record.display_expression.trimmed().isEmpty() ||
      record.result.trimmed().isEmpty() || record.created_at.trimmed().isEmpty() ||
      record.record_id.trimmed().isEmpty()) {
    return false;
  }

  bool ok = false;
  const double numeric_result = record.result.toDouble(&ok);
  return ok && std::isfinite(numeric_result) &&
         record.result != "calculation error";
}

void HistoryManager::ResetError() { last_error_ = {}; }

void HistoryManager::SetError(HistoryErrorCode code, const QString &message) {
  last_error_.code = code;
  last_error_.message = message;
}

}  // namespace s21
