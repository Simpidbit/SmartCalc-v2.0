#ifndef CPP3_SMARTCALC_V2_0_SRC_HISTORY_HISTORY_RECORD_H_
#define CPP3_SMARTCALC_V2_0_SRC_HISTORY_HISTORY_RECORD_H_

#include <QString>

namespace s21 {

struct HistoryRecord {
  QString expression;
  QString display_expression;
  QString x_value;
  QString result;
  QString created_at;
  QString record_id;
};

}  // namespace s21

#endif  // CPP3_SMARTCALC_V2_0_SRC_HISTORY_HISTORY_RECORD_H_
