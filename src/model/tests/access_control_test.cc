#include "../../view/access_control.h"

#include <gtest/gtest.h>

namespace {

s21::KeyboardAction Action(s21::KeyboardActionType type) {
  return {type, {}};
}

}  // namespace

TEST(access_control, validates_advanced_key) {
  EXPECT_TRUE(s21::AccessControl::IsAdvancedKey(
      "9DA1C5FB-D355-42E2-84F6-A92DFACC60AB"));
  EXPECT_TRUE(s21::AccessControl::IsAdvancedKey(
      "  9da1c5fb-d355-42e2-84f6-a92dfacc60ab  "));
  EXPECT_FALSE(s21::AccessControl::IsAdvancedKey(""));
  EXPECT_FALSE(s21::AccessControl::IsAdvancedKey("wrong-key"));
}

TEST(access_control, ordinary_user_can_use_basic_keyboard_actions) {
  EXPECT_TRUE(s21::AccessControl::IsKeyboardActionAllowed(
      s21::UserRole::kOrdinary, Action(s21::KeyboardActionType::kDigit)));
  EXPECT_TRUE(s21::AccessControl::IsKeyboardActionAllowed(
      s21::UserRole::kOrdinary, Action(s21::KeyboardActionType::kPoint)));
  EXPECT_TRUE(s21::AccessControl::IsKeyboardActionAllowed(
      s21::UserRole::kOrdinary, Action(s21::KeyboardActionType::kPlus)));
  EXPECT_TRUE(s21::AccessControl::IsKeyboardActionAllowed(
      s21::UserRole::kOrdinary, Action(s21::KeyboardActionType::kMinus)));
  EXPECT_TRUE(s21::AccessControl::IsKeyboardActionAllowed(
      s21::UserRole::kOrdinary, Action(s21::KeyboardActionType::kMultiply)));
  EXPECT_TRUE(s21::AccessControl::IsKeyboardActionAllowed(
      s21::UserRole::kOrdinary, Action(s21::KeyboardActionType::kDivide)));
  EXPECT_TRUE(s21::AccessControl::IsKeyboardActionAllowed(
      s21::UserRole::kOrdinary,
      Action(s21::KeyboardActionType::kOpenParenthesis)));
  EXPECT_TRUE(s21::AccessControl::IsKeyboardActionAllowed(
      s21::UserRole::kOrdinary,
      Action(s21::KeyboardActionType::kCloseParenthesis)));
  EXPECT_TRUE(s21::AccessControl::IsKeyboardActionAllowed(
      s21::UserRole::kOrdinary, Action(s21::KeyboardActionType::kCalculate)));
  EXPECT_TRUE(s21::AccessControl::IsKeyboardActionAllowed(
      s21::UserRole::kOrdinary, Action(s21::KeyboardActionType::kBackspace)));
  EXPECT_TRUE(s21::AccessControl::IsKeyboardActionAllowed(
      s21::UserRole::kOrdinary, Action(s21::KeyboardActionType::kClear)));
}

TEST(access_control, ordinary_user_cannot_use_advanced_keyboard_actions) {
  EXPECT_FALSE(s21::AccessControl::IsKeyboardActionAllowed(
      s21::UserRole::kOrdinary, Action(s21::KeyboardActionType::kVariableX)));
  EXPECT_FALSE(s21::AccessControl::IsKeyboardActionAllowed(
      s21::UserRole::kOrdinary, Action(s21::KeyboardActionType::kScientificE)));
  EXPECT_FALSE(s21::AccessControl::IsKeyboardActionAllowed(
      s21::UserRole::kOrdinary, Action(s21::KeyboardActionType::kModulo)));
  EXPECT_FALSE(s21::AccessControl::IsKeyboardActionAllowed(
      s21::UserRole::kOrdinary, Action(s21::KeyboardActionType::kPower)));
}

TEST(access_control, advanced_user_can_use_all_keyboard_actions) {
  EXPECT_TRUE(s21::AccessControl::IsKeyboardActionAllowed(
      s21::UserRole::kAdvanced, Action(s21::KeyboardActionType::kVariableX)));
  EXPECT_TRUE(s21::AccessControl::IsKeyboardActionAllowed(
      s21::UserRole::kAdvanced, Action(s21::KeyboardActionType::kScientificE)));
  EXPECT_TRUE(s21::AccessControl::IsKeyboardActionAllowed(
      s21::UserRole::kAdvanced, Action(s21::KeyboardActionType::kModulo)));
  EXPECT_TRUE(s21::AccessControl::IsKeyboardActionAllowed(
      s21::UserRole::kAdvanced, Action(s21::KeyboardActionType::kPower)));
}
