#ifndef CPP3_SMARTCALC_V2_0_SRC_VIEW_ACCESS_CONTROL_H_
#define CPP3_SMARTCALC_V2_0_SRC_VIEW_ACCESS_CONTROL_H_

#include <QString>

#include "keyboard_adapter.h"

namespace s21 {

enum class UserRole {
  kOrdinary,
  kAdvanced,
};

class AccessControl {
 public:
  static bool IsAdvancedKey(const QString &key);
  static bool IsKeyboardActionAllowed(UserRole role,
                                      const KeyboardAction &action);
  static bool IsAdvancedOnlyKeyboardAction(const KeyboardAction &action);
};

}  // namespace s21

#endif  // CPP3_SMARTCALC_V2_0_SRC_VIEW_ACCESS_CONTROL_H_
