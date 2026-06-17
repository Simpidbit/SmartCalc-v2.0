#ifndef CPP3_SMARTCALC_V2_0_SRC_HISTORY_HISTORY_STORE_H_
#define CPP3_SMARTCALC_V2_0_SRC_HISTORY_HISTORY_STORE_H_

#include <QString>
#include <vector>

#include "history_record.h"

namespace s21 {

class HistoryStore {
 public:
  explicit HistoryStore(const QString &file_path = QString());

  QString FilePath() const;
  static QString ResolveDefaultFilePath();

  bool ReadAll(std::vector<HistoryRecord> *records,
               QString *error_message) const;
  bool WriteAll(const std::vector<HistoryRecord> &records,
                QString *error_message) const;
  bool Clear(QString *error_message) const;

 private:
  bool EnsureStorageDirectory(QString *error_message) const;

  QString file_path_;
};

}  // namespace s21

#endif  // CPP3_SMARTCALC_V2_0_SRC_HISTORY_HISTORY_STORE_H_
