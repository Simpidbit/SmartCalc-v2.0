#ifndef CPP3_SMARTCALC_V2_0_SRC_HISTORY_HISTORY_MANAGER_H_
#define CPP3_SMARTCALC_V2_0_SRC_HISTORY_HISTORY_MANAGER_H_

#include <optional>
#include <vector>

#include "history_store.h"

namespace s21 {

enum class HistoryErrorCode {
  kNone,
  kInvalidRecord,
  kReadFailed,
  kWriteFailed,
};

struct HistoryErrorInfo {
  HistoryErrorCode code = HistoryErrorCode::kNone;
  QString message;
};

class HistoryManager {
 public:
  explicit HistoryManager(const QString &file_path = QString(),
                          int max_records = 100);

  bool AppendHistory(HistoryRecord record);
  std::vector<HistoryRecord> LoadRecent(int limit = 100);
  bool ClearHistory();
  bool TrimToLimit(int limit = 100);
  std::optional<HistoryRecord> GetRecordById(const QString &record_id);

  HistoryErrorInfo GetLastError() const;
  QString FilePath() const;

 private:
  bool IsValidRecord(const HistoryRecord &record) const;
  void ResetError();
  void SetError(HistoryErrorCode code, const QString &message);

  HistoryStore store_;
  int max_records_ = 100;
  HistoryErrorInfo last_error_;
};

}  // namespace s21

#endif  // CPP3_SMARTCALC_V2_0_SRC_HISTORY_HISTORY_MANAGER_H_
