#ifndef CPP3_SMARTCALC_V2_0_SRC_VIEW_KEYBOARD_ADAPTER_H_
#define CPP3_SMARTCALC_V2_0_SRC_VIEW_KEYBOARD_ADAPTER_H_

#include <QChar>
#include <QKeyEvent>

namespace s21 {

enum class KeyboardActionType {
  kIgnored,
  kDigit,
  kPlus,
  kMinus,
  kMultiply,
  kDivide,
  kModulo,
  kPower,
  kPoint,
  kScientificE,
  kVariableX,
  kOpenParenthesis,
  kCloseParenthesis,
  kCalculate,
  kBackspace,
  kClear,
};

struct KeyboardAction {
  KeyboardActionType type = KeyboardActionType::kIgnored;
  QChar token;
};

class KeyboardAdapter {
 public:
  static KeyboardAction HandleKey(const QKeyEvent &event);
};

}  // namespace s21

#endif  // CPP3_SMARTCALC_V2_0_SRC_VIEW_KEYBOARD_ADAPTER_H_
