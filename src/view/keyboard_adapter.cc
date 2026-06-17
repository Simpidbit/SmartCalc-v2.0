#include "keyboard_adapter.h"

namespace s21 {

KeyboardAction KeyboardAdapter::HandleKey(const QKeyEvent &event) {
  switch (event.key()) {
    case Qt::Key_Return:
    case Qt::Key_Enter:
      return {KeyboardActionType::kCalculate, {}};
    case Qt::Key_Backspace:
      return {KeyboardActionType::kBackspace, {}};
    case Qt::Key_Escape:
    case Qt::Key_Delete:
      return {KeyboardActionType::kClear, {}};
    default:
      break;
  }

  const QString text = event.text();
  if (text.size() != 1) {
    return {};
  }

  const QChar token = text.at(0).toLower();
  if (token.isDigit()) {
    return {KeyboardActionType::kDigit, token};
  }

  if (token == '+') {
    return {KeyboardActionType::kPlus, token};
  }
  if (token == '-') {
    return {KeyboardActionType::kMinus, token};
  }
  if (token == '*') {
    return {KeyboardActionType::kMultiply, token};
  }
  if (token == '/') {
    return {KeyboardActionType::kDivide, token};
  }
  if (token == '%') {
    return {KeyboardActionType::kModulo, token};
  }
  if (token == '^') {
    return {KeyboardActionType::kPower, token};
  }
  if (token == '.') {
    return {KeyboardActionType::kPoint, token};
  }
  if (token == 'e') {
    return {KeyboardActionType::kScientificE, token};
  }
  if (token == 'x') {
    return {KeyboardActionType::kVariableX, token};
  }
  if (token == '(') {
    return {KeyboardActionType::kOpenParenthesis, token};
  }
  if (token == ')') {
    return {KeyboardActionType::kCloseParenthesis, token};
  }

  return {};
}

}  // namespace s21
