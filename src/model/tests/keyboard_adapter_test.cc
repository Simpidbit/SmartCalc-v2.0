#include "../../view/keyboard_adapter.h"

#include <gtest/gtest.h>

namespace {

s21::KeyboardAction ActionFor(int key, const QString &text = QString()) {
  QKeyEvent event(QEvent::KeyPress, key, Qt::NoModifier, text);
  return s21::KeyboardAdapter::HandleKey(event);
}

}  // namespace

TEST(keyboard_adapter, maps_digits_and_expression_tokens) {
  EXPECT_EQ(ActionFor(Qt::Key_1, "1").type,
            s21::KeyboardActionType::kDigit);
  EXPECT_EQ(ActionFor(Qt::Key_Plus, "+").type,
            s21::KeyboardActionType::kPlus);
  EXPECT_EQ(ActionFor(Qt::Key_Minus, "-").type,
            s21::KeyboardActionType::kMinus);
  EXPECT_EQ(ActionFor(Qt::Key_Asterisk, "*").type,
            s21::KeyboardActionType::kMultiply);
  EXPECT_EQ(ActionFor(Qt::Key_Slash, "/").type,
            s21::KeyboardActionType::kDivide);
  EXPECT_EQ(ActionFor(Qt::Key_Percent, "%").type,
            s21::KeyboardActionType::kModulo);
  EXPECT_EQ(ActionFor(Qt::Key_AsciiCircum, "^").type,
            s21::KeyboardActionType::kPower);
  EXPECT_EQ(ActionFor(Qt::Key_Period, ".").type,
            s21::KeyboardActionType::kPoint);
  EXPECT_EQ(ActionFor(Qt::Key_E, "e").type,
            s21::KeyboardActionType::kScientificE);
  EXPECT_EQ(ActionFor(Qt::Key_X, "X").type,
            s21::KeyboardActionType::kVariableX);
  EXPECT_EQ(ActionFor(Qt::Key_ParenLeft, "(").type,
            s21::KeyboardActionType::kOpenParenthesis);
  EXPECT_EQ(ActionFor(Qt::Key_ParenRight, ")").type,
            s21::KeyboardActionType::kCloseParenthesis);
}

TEST(keyboard_adapter, maps_shortcuts) {
  EXPECT_EQ(ActionFor(Qt::Key_Return).type,
            s21::KeyboardActionType::kCalculate);
  EXPECT_EQ(ActionFor(Qt::Key_Enter).type,
            s21::KeyboardActionType::kCalculate);
  EXPECT_EQ(ActionFor(Qt::Key_Backspace).type,
            s21::KeyboardActionType::kBackspace);
  EXPECT_EQ(ActionFor(Qt::Key_Escape).type,
            s21::KeyboardActionType::kClear);
  EXPECT_EQ(ActionFor(Qt::Key_Delete).type,
            s21::KeyboardActionType::kClear);
}

TEST(keyboard_adapter, ignores_unsupported_keys) {
  EXPECT_EQ(ActionFor(Qt::Key_A, "a").type,
            s21::KeyboardActionType::kIgnored);
  EXPECT_EQ(ActionFor(Qt::Key_Space, " ").type,
            s21::KeyboardActionType::kIgnored);
}
