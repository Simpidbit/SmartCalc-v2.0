#include "access_control.h"

namespace s21 {

namespace {

constexpr char kAdvancedKey[] = "9DA1C5FB-D355-42E2-84F6-A92DFACC60AB";

}  // namespace

bool AccessControl::IsAdvancedKey(const QString &key) {
  return key.trimmed().compare(kAdvancedKey, Qt::CaseInsensitive) == 0;
}

bool AccessControl::IsKeyboardActionAllowed(UserRole role,
                                            const KeyboardAction &action) {
  return role == UserRole::kAdvanced ||
         !IsAdvancedOnlyKeyboardAction(action);
}

bool AccessControl::IsAdvancedOnlyKeyboardAction(
    const KeyboardAction &action) {
  switch (action.type) {
    case KeyboardActionType::kModulo:
    case KeyboardActionType::kPower:
    case KeyboardActionType::kScientificE:
    case KeyboardActionType::kVariableX:
      return true;
    default:
      return false;
  }
}

}  // namespace s21
