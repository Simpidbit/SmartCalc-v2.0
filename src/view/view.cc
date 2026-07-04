#include "view.h"

#include <QApplication>
#include <QDateTime>
#include <QDialog>
#include <QHBoxLayout>
#include <QInputDialog>
#include <QLabel>
#include <QLineEdit>
#include <QListWidget>
#include <QMessageBox>
#include <QRegularExpression>
#include <QRegularExpressionValidator>
#include <QStatusBar>
#include <QVBoxLayout>

#include <algorithm>
#include <cmath>
#include <iostream>
#include <optional>

#include "./ui_view.h"

namespace {

bool IsBinaryOperatorSymbol(QChar symbol) {
  return symbol == '+' || symbol == '-' || symbol == '*' || symbol == '/' ||
         symbol == '%' || symbol == '^';
}

bool IsScientificNotationSign(const QString &expression, int index) {
  if (index <= 0 || index >= expression.length()) {
    return false;
  }

  const QChar symbol = expression.at(index);
  return (symbol == '+' || symbol == '-') && expression.at(index - 1) == 'e';
}

bool IsBinaryOperatorAt(const QString &expression, int index) {
  if (index < 0 || index >= expression.length()) {
    return false;
  }
  return IsBinaryOperatorSymbol(expression.at(index)) &&
         !IsScientificNotationSign(expression, index);
}

bool IsStandaloneOperandSymbol(QChar symbol) {
  return symbol == 'x' || symbol == 'E';
}

bool CanStartOperand(const QString &expression) {
  if (expression.isEmpty()) {
    return true;
  }

  const int last_index = expression.length() - 1;
  return expression.at(last_index) == '(' ||
         IsBinaryOperatorAt(expression, last_index);
}

int CurrentTokenStart(const QString &expression) {
  int token_start = expression.length() - 1;
  while (token_start >= 0) {
    const QChar symbol = expression.at(token_start);
    if (symbol == '(' || symbol == ')' ||
        IsBinaryOperatorAt(expression, token_start)) {
      break;
    }
    --token_start;
  }
  return token_start + 1;
}

QString CurrentToken(const QString &expression) {
  if (expression.isEmpty()) {
    return QString();
  }
  return expression.mid(CurrentTokenStart(expression));
}

bool CanAppendScientificNotationMarker(const QString &expression) {
  const QString token = CurrentToken(expression);
  if (token.isEmpty() || !token.back().isDigit() || token.contains('e')) {
    return false;
  }

  bool has_digit = false;
  bool has_point = false;
  for (const QChar symbol : token) {
    if (symbol.isDigit()) {
      has_digit = true;
    } else if (symbol == '.' && !has_point) {
      has_point = true;
    } else {
      return false;
    }
  }
  return has_digit;
}

bool CanAppendScientificNotationSign(const QString &expression) {
  return !expression.isEmpty() && expression.back() == 'e';
}

bool HasIncompleteScientificNotationAtEnd(const QString &expression) {
  const QString token = CurrentToken(expression);
  const int exponent_index = token.lastIndexOf('e');
  if (exponent_index < 0) {
    return false;
  }

  if (exponent_index == token.length() - 1) {
    return true;
  }

  const bool only_sign_after_exponent = exponent_index == token.length() - 2 &&
                                        (token.back() == '+' ||
                                         token.back() == '-');
  return only_sign_after_exponent;
}

bool CurrentTokenHasScientificNotation(const QString &expression) {
  return CurrentToken(expression).contains('e');
}

}  // namespace

View::View(QWidget *parent, s21::Controller *controller)
    : QMainWindow(parent), ui_(new Ui::View), controller_(controller) {
  ui_->setupUi(this);
  setFocusPolicy(Qt::StrongFocus);

  QRegularExpressionValidator *regexXValidator =
      new QRegularExpressionValidator(QRegularExpression(
          "^(-?100000000000(\\.0{1,6})?|(-?[0-9]{1,11})(\\.[0-9]{1,6})?)$"),
                                      this);
  ui_->x_value_input->setValidator(regexXValidator);

  history_button_ = new QPushButton("History", ui_->centralwidget);
  history_button_->setGeometry(72, 130, 125, 30);
  history_button_->setToolTip("show calculation history");
  history_button_->setFocusPolicy(Qt::NoFocus);
  ui_->e_button->setToolTip(
      "Euler constant; after a number, scientific notation");

  advanced_access_button_ = new QPushButton("Unlock", ui_->centralwidget);
  advanced_access_button_->setGeometry(15, 5, 80, 24);
  advanced_access_button_->setToolTip("unlock advanced user features");
  advanced_access_button_->setFocusPolicy(Qt::NoFocus);

  std::vector<QPushButton *> num_buttons = {
      ui_->zero_button,  ui_->one_button,  ui_->two_button, ui_->three_button,
      ui_->four_button,  ui_->five_button, ui_->six_button, ui_->seven_button,
      ui_->eight_button, ui_->nine_button};
  for (QPushButton *num_button : num_buttons) {
    QObject::connect(num_button, SIGNAL(clicked()), this,
                     SLOT(NumberClicked()));
  }

  std::vector<QPushButton *> plus_minus_operators = {ui_->minus_button,
                                                     ui_->plus_button};
  for (QPushButton *op_button : plus_minus_operators) {
    QObject::connect(op_button, SIGNAL(clicked()), this,
                     SLOT(PlusMinusOperatorClicked()));
  }

  std::vector<QPushButton *> mul_div_operators = {ui_->multiply_button,
                                                  ui_->divide_button};
  for (QPushButton *mul_div_op_button : mul_div_operators) {
    QObject::connect(mul_div_op_button, SIGNAL(clicked()), this,
                     SLOT(MulDivOperatorClicked()));
  }

  std::vector<QPushButton *> math_functions = {
      ui_->tan_button,  ui_->cos_button,  ui_->sin_button, ui_->atan_button,
      ui_->acos_button, ui_->asin_button, ui_->log_button, ui_->ln_button};
  for (QPushButton *function_button : math_functions) {
    QObject::connect(function_button, SIGNAL(clicked()), this,
                     SLOT(MathFunctionClicked()));
  }

  connect(ui_->clear_button, SIGNAL(clicked()), this,
          SLOT(ClearButtonClicked()));
  connect(ui_->point_button, SIGNAL(clicked()), this,
          SLOT(PointButtonClicked()));
  connect(ui_->open_parenthesis_button, SIGNAL(clicked()), this,
          SLOT(OpenParenthesisButtonClicked()));
  connect(ui_->close_parenthesis_button, SIGNAL(clicked()), this,
          SLOT(CloseParenthesisButtonClicked()));
  connect(ui_->x_button, SIGNAL(clicked()), this, SLOT(XButtonClicked()));
  connect(ui_->mod_button, SIGNAL(clicked()), this, SLOT(ModButtonClicked()));
  connect(ui_->pow_button, SIGNAL(clicked()), this, SLOT(PowButtonClicked()));
  connect(ui_->sqrt_button, SIGNAL(clicked()), this, SLOT(SqrtButtonClicked()));
  connect(ui_->e_button, SIGNAL(clicked()), this, SLOT(EButtonClicked()));
  connect(ui_->backspace_button, SIGNAL(clicked()), this,
          SLOT(BackspaceClicked()));
  connect(ui_->equal_button, SIGNAL(clicked()), this,
           SLOT(EqualButtonClicked()));
  connect(ui_->graphing, SIGNAL(clicked()), this, SLOT(OpenGraphWindow()));
  connect(history_button_, SIGNAL(clicked()), this, SLOT(OpenHistoryWindow()));
  connect(advanced_access_button_, SIGNAL(clicked()), this,
          SLOT(UnlockAdvancedAccess()));

  const QList<QPushButton *> buttons = findChildren<QPushButton *>();
  for (QPushButton *button : buttons) {
    button->setFocusPolicy(Qt::NoFocus);
  }

  InstallAccessStyles();
  ApplyAccessState();
}

View::~View() { delete ui_; }

void View::keyPressEvent(QKeyEvent *event) {
  if (!IsExpressionKeyboardFocusAllowed()) {
    QMainWindow::keyPressEvent(event);
    return;
  }

  const s21::KeyboardAction action = s21::KeyboardAdapter::HandleKey(*event);
  if (HandleKeyboardAction(action)) {
    event->accept();
    return;
  }

  QMainWindow::keyPressEvent(event);
}

void View::ClearButtonClicked() {
  string_to_calculate_.clear();
  string_to_show_.clear();
  ui_->display->setText("0");
  ui_->x_value_input->clear();
  num_clicked_ = false;
  point_clicked_ = false;
  operator_clicked_ = false;
  x_clicked_ = false;
  open_parenthesis_clicked_ = 0;
  flag_first_zero_ = false;
  e_clicked_ = false;
}

void View::NumberClicked() {
  QPushButton *button = qobject_cast<QPushButton *>(sender());

  if (string_to_calculate_.length() != 0 && !point_clicked_ &&
      string_to_calculate_.back() == '0' && button->text() == '0' &&
      !flag_first_zero_) {
    flag_first_zero_ = false;
  }

  if (string_to_calculate_.length() != 0 && !point_clicked_ &&
      button->text() != '0') {
    if (string_to_calculate_.back() == '0' && !flag_first_zero_) {
      string_to_calculate_.chop(1);
      string_to_show_.chop(1);
    }

    flag_first_zero_ = true;
  }

  if ((flag_first_zero_ || (!num_clicked_ || point_clicked_)) &&
      button->text() != '0') {
    if (string_to_calculate_.length() == 0) {
      string_to_calculate_ += button->text();
      string_to_show_ += button->text();
      ui_->display->setText(string_to_show_);
      num_clicked_ = true;
      operator_clicked_ = false;
      flag_first_zero_ = true;

    } else if (string_to_calculate_.length() != 0 &&
               string_to_calculate_.back() != ')' &&
               !IsStandaloneOperandSymbol(string_to_calculate_.back())) {
      string_to_calculate_ += button->text();
      string_to_show_ += button->text();
      ui_->display->setText(string_to_show_);
      num_clicked_ = true;
      operator_clicked_ = false;
      flag_first_zero_ = true;
    }

  } else if ((flag_first_zero_ || (!num_clicked_ || point_clicked_)) &&
             button->text() == '0') {
    if (string_to_calculate_.length() == 0) {
      string_to_calculate_ += button->text();
      string_to_show_ += button->text();
      ui_->display->setText(string_to_show_);
      num_clicked_ = true;
      operator_clicked_ = false;

    } else if (string_to_calculate_.length() != 0 &&
               string_to_calculate_.back() != ')' &&
               !IsStandaloneOperandSymbol(string_to_calculate_.back())) {
      string_to_calculate_ += button->text();
      string_to_show_ += button->text();
      ui_->display->setText(string_to_show_);
      num_clicked_ = true;
      operator_clicked_ = false;
    }
  }
}

void View::PlusMinusOperatorClicked() {
  QPushButton *button = qobject_cast<QPushButton *>(sender());
  if (button == nullptr) {
    return;
  }

  if (CanAppendScientificNotationSign(string_to_calculate_)) {
    string_to_calculate_ += button->text();
    string_to_show_ += button->text();
    ui_->display->setText(string_to_show_);
    num_clicked_ = false;
    flag_first_zero_ = false;
    point_clicked_ = false;
    operator_clicked_ = true;
    x_clicked_ = false;
    e_clicked_ = true;
    return;
  }

  if (string_to_calculate_.length() != 0 && !operator_clicked_ &&
      string_to_calculate_.back() != '.' &&
      !HasIncompleteScientificNotationAtEnd(string_to_calculate_)) {

    if (string_to_calculate_.back() != 'e') {
      e_clicked_ = false;
    }

    string_to_calculate_ += button->text();
    string_to_show_ += button->text();
    ui_->display->setText(string_to_show_);
    num_clicked_ = false;
    flag_first_zero_ = false;
    point_clicked_ = false;
    operator_clicked_ = true;
    x_clicked_ = false;

  } else if (string_to_calculate_.length() == 0) {
    string_to_calculate_ += button->text();
    string_to_show_ += button->text();
    ui_->display->setText(string_to_show_);
    operator_clicked_ = true;
  }
}

void View::MulDivOperatorClicked() {
  if (!operator_clicked_ && string_to_calculate_.length() != 0 &&
      string_to_calculate_.back() != '(' &&
      string_to_calculate_.back() != '.' &&
      !HasIncompleteScientificNotationAtEnd(string_to_calculate_)) {
    QPushButton *button = qobject_cast<QPushButton *>(sender());

    if (button->text() == "×") {
      string_to_calculate_ += "*";

    } else if (button->text() == "÷") {
      string_to_calculate_ += "/";
    }

    string_to_show_ += button->text();
    ui_->display->setText(string_to_show_);
    num_clicked_ = false;
    point_clicked_ = false;
    operator_clicked_ = true;
    x_clicked_ = false;
  }
}

void View::MathFunctionClicked() {
  if (!EnsureAdvancedAccess("Math functions")) {
    return;
  }

  bool flag = false;
  if (string_to_calculate_.length() != 0) {
    if (!num_clicked_ && !point_clicked_ &&
        !IsStandaloneOperandSymbol(string_to_calculate_.back()) &&
        string_to_calculate_.back() != ')' &&
        !HasIncompleteScientificNotationAtEnd(string_to_calculate_)) {
      flag = true;
      QPushButton *button = qobject_cast<QPushButton *>(sender());
      string_to_calculate_ += button->text() + "(";
      string_to_show_ += button->text() + "(";
      ui_->display->setText(string_to_show_);
      open_parenthesis_clicked_++;
      num_clicked_ = false;
      operator_clicked_ = false;
      e_clicked_ = false;
    }

  } else if ((operator_clicked_ || string_to_calculate_.length() == 0 ||
              open_parenthesis_clicked_ > 0) &&
             !flag) {
    QPushButton *button = qobject_cast<QPushButton *>(sender());
    string_to_calculate_ += button->text() + "(";
    string_to_show_ += button->text() + "(";
    ui_->display->setText(string_to_show_);
    open_parenthesis_clicked_++;
    num_clicked_ = false;
    operator_clicked_ = false;
  }
}

void View::OpenParenthesisButtonClicked() {
  if (string_to_calculate_.length() == 0) {
    QPushButton *button = qobject_cast<QPushButton *>(sender());
    string_to_calculate_ += button->text();
    string_to_show_ += button->text();
    ui_->display->setText(string_to_show_);
    open_parenthesis_clicked_++;

  } else if (string_to_calculate_.length() != 0 &&
             (operator_clicked_ || open_parenthesis_clicked_ > 0) &&
             !IsStandaloneOperandSymbol(string_to_calculate_.back()) &&
             !num_clicked_ &&
             !HasIncompleteScientificNotationAtEnd(string_to_calculate_)) {
    QPushButton *button = qobject_cast<QPushButton *>(sender());
    string_to_calculate_ += button->text();
    string_to_show_ += button->text();
    ui_->display->setText(string_to_show_);
    open_parenthesis_clicked_++;
    operator_clicked_ = false;
    num_clicked_ = false;
  }
}

void View::CloseParenthesisButtonClicked() {
  if (open_parenthesis_clicked_ > 0 && string_to_calculate_.back() != '.' &&
      (num_clicked_ || string_to_calculate_.back() == ')' || x_clicked_)) {
    QPushButton *button = qobject_cast<QPushButton *>(sender());
    string_to_calculate_ += button->text();
    string_to_show_ += button->text();
    ui_->display->setText(string_to_show_);
    open_parenthesis_clicked_--;
    operator_clicked_ = false;
  }
}

void View::PointButtonClicked() {
  if (num_clicked_ && !point_clicked_ && string_to_calculate_.back() != ')' &&
      !e_clicked_) {
    QPushButton *button = qobject_cast<QPushButton *>(sender());
    string_to_calculate_ += button->text();
    string_to_show_ += button->text();
    ui_->display->setText(string_to_show_);
    point_clicked_ = true;
  }
}

void View::ModButtonClicked() {
  if (!EnsureAdvancedAccess("Modulo")) {
    return;
  }

  if (!operator_clicked_ && (num_clicked_ || x_clicked_) &&
      string_to_calculate_.back() != '.' &&
      !HasIncompleteScientificNotationAtEnd(string_to_calculate_)) {
    string_to_calculate_ += "%";
    string_to_show_ += "%";
    ui_->display->setText(string_to_show_);
    operator_clicked_ = true;
    point_clicked_ = false;
    num_clicked_ = false;
    x_clicked_ = false;
  }
}

void View::PowButtonClicked() {
  if (!EnsureAdvancedAccess("Power")) {
    return;
  }

  AppendPowerOperator(true);
}

void View::AppendPowerOperator(bool open_group) {
  if (string_to_calculate_.length() != 0 &&
      string_to_calculate_.back() != '.' && string_to_calculate_.back() != 'e' &&
      !HasIncompleteScientificNotationAtEnd(string_to_calculate_) &&
      (num_clicked_ || string_to_calculate_.back() == ')' ||
       IsStandaloneOperandSymbol(string_to_calculate_.back()))) {
    if (open_group) {
      string_to_calculate_ += "^(";
      string_to_show_ += "^(";
      open_parenthesis_clicked_++;
    } else {
      string_to_calculate_ += "^";
      string_to_show_ += "^";
    }
    ui_->display->setText(string_to_show_);
    num_clicked_ = false;
    x_clicked_ = false;
    operator_clicked_ = true;
    point_clicked_ = false;
    e_clicked_ = false;
    flag_first_zero_ = false;
  }
}

void View::SqrtButtonClicked() {
  if (!EnsureAdvancedAccess("Square root")) {
    return;
  }

  if (string_to_calculate_.length() == 0) {
    QPushButton *button = qobject_cast<QPushButton *>(sender());
    string_to_calculate_ += "r(";
    string_to_show_ += button->text() + "(";
    ui_->display->setText(string_to_show_);
    open_parenthesis_clicked_++;
    operator_clicked_ = false;

  } else if (string_to_calculate_.length() != 0 &&
             (operator_clicked_ || string_to_calculate_.back() == '(') &&
             !HasIncompleteScientificNotationAtEnd(string_to_calculate_)) {
    QPushButton *button = qobject_cast<QPushButton *>(sender());
    string_to_calculate_ += "r(";
    string_to_show_ += button->text() + "(";
    ui_->display->setText(string_to_show_);
    point_clicked_ = false;
    open_parenthesis_clicked_++;
    operator_clicked_ = false;
  }
}

void View::XButtonClicked() {
  if (!EnsureAdvancedAccess("Variable x")) {
    return;
  }

  if (((string_to_calculate_.length() == 0 || operator_clicked_) &&
       !HasIncompleteScientificNotationAtEnd(string_to_calculate_)) ||
      (open_parenthesis_clicked_ > 0 && !num_clicked_ && !x_clicked_)) {
    QPushButton *button = qobject_cast<QPushButton *>(sender());
    string_to_calculate_ += "x";
    string_to_show_ += button->text();
    ui_->display->setText(string_to_show_);
    x_clicked_ = true;
    operator_clicked_ = false;
    num_clicked_ = false;
  }
}

void View::EButtonClicked() {
  if (!EnsureAdvancedAccess("Euler constant and scientific notation")) {
    return;
  }

  QPushButton *button = qobject_cast<QPushButton *>(sender());
  if (button == nullptr) {
    return;
  }

  if (CanAppendScientificNotationMarker(string_to_calculate_)) {
    string_to_show_ += button->text();
    string_to_calculate_ += "e";
    ui_->display->setText(string_to_show_);
    e_clicked_ = true;
    num_clicked_ = false;
    point_clicked_ = false;
    operator_clicked_ = true;
    x_clicked_ = false;
    flag_first_zero_ = false;
    return;
  }

  if (CanStartOperand(string_to_calculate_)) {
    string_to_show_ += button->text();
    string_to_calculate_ += "E";
    ui_->display->setText(string_to_show_);
    e_clicked_ = false;
    num_clicked_ = false;
    point_clicked_ = false;
    operator_clicked_ = false;
    x_clicked_ = true;
    flag_first_zero_ = true;
  }
}

void View::ChopString(size_t number_to_chop) {
  string_to_calculate_.chop(number_to_chop);
  string_to_show_.chop(number_to_chop);
}

void View::BackspaceClicked() {
  if (string_to_calculate_.length() != 0) {
    if (string_to_calculate_.length() == 1) {
      ClearButtonClicked();
      return;
    }

    QString last_five_chars = string_to_calculate_.right(5);
    QString last_four_chars = string_to_calculate_.right(4);
    QString last_three_chars = string_to_calculate_.right(3);
    QString last_two_chars = string_to_calculate_.right(2);

    if (string_to_calculate_.back() == '.') {
      ChopString(1);
      point_clicked_ = false;
      QString::ConstIterator str = string_to_calculate_.end() - 1;
      flag_first_zero_ = GetZeroStatus(str);

    } else if (string_to_calculate_.back() == '%' ||
               string_to_calculate_.back() == '+' ||
               string_to_calculate_.back() == '-' ||
               string_to_calculate_.back() == '*' ||
               string_to_calculate_.back() == '/' ||
               string_to_calculate_.back() == '^') {
      ChopString(1);
      QString::ConstIterator str = string_to_calculate_.end() - 1;
      point_clicked_ = GetPointStatus(str);
      flag_first_zero_ = GetZeroStatus(str);
      e_clicked_ = GetEStatus(str);

    } else if (string_to_calculate_.back() == ')') {
      ChopString(1);
      open_parenthesis_clicked_++;

    } else if (string_to_calculate_.back() == 'x') {
      string_to_calculate_.chop(1);
      string_to_show_.chop(2);

    } else if (string_to_calculate_.back() == 'E') {
      ChopString(1);

    } else if (string_to_calculate_.back() == 'e') {
      ChopString(1);
      e_clicked_ = false;

    } else if (last_five_chars == "acos(" || last_five_chars == "asin(" ||
               last_five_chars == "atan(") {
      ChopString(5);
      open_parenthesis_clicked_--;

    } else if (last_four_chars == "cos(" || last_four_chars == "sin(" ||
               last_four_chars == "tan(" || last_four_chars == "log(") {
      ChopString(4);
      open_parenthesis_clicked_--;

    } else if (last_three_chars == "ln(") {
      ChopString(3);
      open_parenthesis_clicked_--;

    } else if (last_two_chars == "^(" || last_two_chars == "r(") {
      ChopString(2);
      open_parenthesis_clicked_--;

    } else if (string_to_calculate_.back() == '(') {
      ChopString(1);
      open_parenthesis_clicked_--;

    } else {
      /// 如果当前删除的是数字
      ChopString(1);
    }

    if (string_to_calculate_.length() == 0) {
      ClearButtonClicked();

    } else {
      ui_->display->setText(string_to_show_);
      QString::ConstIterator str = string_to_calculate_.end() - 1;
      num_clicked_ = GetNumStatus(str);
      operator_clicked_ = GetOperatorStatus(str);
      x_clicked_ = GetXStatus(str);
    }

  } else {
    /// 如果字符串已经为空
    ClearButtonClicked();
  }
}

bool View::GetPointStatus(QString::ConstIterator str) {
  Q_UNUSED(str);
  return CurrentToken(string_to_calculate_).contains('.');
}

bool View::GetOperatorStatus(QString::ConstIterator str) {
  if (str->isNull()) {
    return false;
  }
  return IsBinaryOperatorAt(
      string_to_calculate_,
      static_cast<int>(str - string_to_calculate_.constBegin()));
}

bool View::GetZeroStatus(QString::ConstIterator str) {
  Q_UNUSED(str);
  const QString token = CurrentToken(string_to_calculate_);
  return token != "0";
}

bool View::GetNumStatus(QString::ConstIterator str) {
  if (!str->isNull() && (str->isDigit() || *str == '.')) {
    return true;
  }
  return false;
}

bool View::GetXStatus(QString::ConstIterator str) {
  if (!str->isNull() && IsStandaloneOperandSymbol(*str)) {
    return true;
  }
  return false;
}

bool View::GetEStatus(QString::ConstIterator str) {
  Q_UNUSED(str);
  return CurrentTokenHasScientificNotation(string_to_calculate_);
}

bool View::HandleKeyboardAction(const s21::KeyboardAction &action) {
  if (!s21::AccessControl::IsKeyboardActionAllowed(user_role_, action)) {
    EnsureAdvancedAccess("This keyboard shortcut");
    return true;
  }

  switch (action.type) {
    case s21::KeyboardActionType::kIgnored:
      return false;
    case s21::KeyboardActionType::kCalculate:
      EqualButtonClicked();
      return true;
    case s21::KeyboardActionType::kBackspace:
      BackspaceClicked();
      return true;
    case s21::KeyboardActionType::kClear:
      ClearButtonClicked();
      return true;
    case s21::KeyboardActionType::kPower:
      AppendPowerOperator(false);
      return true;
    default:
      break;
  }

  QPushButton *button = ButtonForKeyboardAction(action);
  if (button == nullptr) {
    return false;
  }
  button->click();
  return true;
}

bool View::IsExpressionKeyboardFocusAllowed() const {
  const QWidget *focused_widget = QApplication::focusWidget();
  if (focused_widget == nullptr) {
    return true;
  }
  return focused_widget != ui_->x_value_input &&
         !ui_->x_value_input->isAncestorOf(focused_widget);
}

QPushButton *View::ButtonForKeyboardAction(
    const s21::KeyboardAction &action) const {
  switch (action.type) {
    case s21::KeyboardActionType::kDigit:
      switch (action.token.toLatin1()) {
        case '0':
          return ui_->zero_button;
        case '1':
          return ui_->one_button;
        case '2':
          return ui_->two_button;
        case '3':
          return ui_->three_button;
        case '4':
          return ui_->four_button;
        case '5':
          return ui_->five_button;
        case '6':
          return ui_->six_button;
        case '7':
          return ui_->seven_button;
        case '8':
          return ui_->eight_button;
        case '9':
          return ui_->nine_button;
        default:
          return nullptr;
      }
    case s21::KeyboardActionType::kPlus:
      return ui_->plus_button;
    case s21::KeyboardActionType::kMinus:
      return ui_->minus_button;
    case s21::KeyboardActionType::kMultiply:
      return ui_->multiply_button;
    case s21::KeyboardActionType::kDivide:
      return ui_->divide_button;
    case s21::KeyboardActionType::kModulo:
      return ui_->mod_button;
    case s21::KeyboardActionType::kPoint:
      return ui_->point_button;
    case s21::KeyboardActionType::kScientificE:
      return ui_->e_button;
    case s21::KeyboardActionType::kVariableX:
      return ui_->x_button;
    case s21::KeyboardActionType::kOpenParenthesis:
      return ui_->open_parenthesis_button;
    case s21::KeyboardActionType::kCloseParenthesis:
      return ui_->close_parenthesis_button;
    default:
      return nullptr;
  }
}

void View::UnlockAdvancedAccess() {
  if (IsAdvancedUser()) {
    QMessageBox::information(this, "Access", "Advanced access is already unlocked");
    return;
  }

  bool accepted = false;
  const QString key = QInputDialog::getText(
      this, "Unlock advanced access", "Enter advanced user key:",
      QLineEdit::Password, QString(), &accepted);
  if (!accepted) {
    return;
  }

  if (!s21::AccessControl::IsAdvancedKey(key)) {
    QMessageBox::warning(this, "Access", "Invalid advanced user key");
    return;
  }

  user_role_ = s21::UserRole::kAdvanced;
  ApplyAccessState();
  statusBar()->showMessage("Advanced access unlocked", 3000);
  QMessageBox::information(this, "Access", "Advanced access unlocked");
}

void View::ApplyAccessState() {
  const bool advanced = IsAdvancedUser();
  SetAdvancedControlsEnabled(advanced);

  if (advanced_access_button_ != nullptr) {
    advanced_access_button_->setText(advanced ? "Advanced" : "Unlock");
    advanced_access_button_->setToolTip(
        advanced ? "advanced user features are unlocked"
                 : "unlock advanced user features");
  }

  ui_->x_value_input->setPlaceholderText(advanced ? "0" : "locked");
}

bool View::IsAdvancedUser() const {
  return user_role_ == s21::UserRole::kAdvanced;
}

bool View::EnsureAdvancedAccess(const QString &feature_name) {
  if (IsAdvancedUser()) {
    return true;
  }

  const QString message = QString("%1 requires advanced access").arg(feature_name);
  statusBar()->showMessage(message, 3000);
  return false;
}

void View::SetAdvancedControlsEnabled(bool enabled) {
  const QList<QWidget *> advanced_controls = {
      ui_->x_button,     ui_->e_button,    ui_->mod_button,
      ui_->pow_button,   ui_->sqrt_button, ui_->tan_button,
      ui_->cos_button,   ui_->sin_button,  ui_->atan_button,
      ui_->acos_button,  ui_->asin_button, ui_->log_button,
      ui_->ln_button,    ui_->graphing,    ui_->x_label,
      ui_->x_value_input};

  for (QWidget *control : advanced_controls) {
    control->setEnabled(enabled);
  }

  if (history_button_ != nullptr) {
    history_button_->setEnabled(enabled);
  }
}

void View::InstallAccessStyles() {
  const QString disabled_button_style =
      "\nQPushButton:disabled {"
      " background-color: #C8C8C8;"
      " border-color: #A0A0A0;"
      " color: #808080;"
      "}";
  const QList<QPushButton *> advanced_buttons = {
      ui_->x_button,    ui_->e_button,     ui_->mod_button,
      ui_->pow_button,  ui_->sqrt_button,  ui_->tan_button,
      ui_->cos_button,  ui_->sin_button,   ui_->atan_button,
      ui_->acos_button, ui_->asin_button,  ui_->log_button,
      ui_->ln_button,   ui_->graphing,     history_button_};

  for (QPushButton *button : advanced_buttons) {
    if (button != nullptr) {
      button->setStyleSheet(button->styleSheet() + disabled_button_style);
    }
  }

  ui_->x_label->setStyleSheet(
      ui_->x_label->styleSheet() +
      "\nQLabel:disabled { background-color: #C8C8C8; color: #808080; }");
  ui_->x_value_input->setStyleSheet(
      ui_->x_value_input->styleSheet() +
      "\nQLineEdit:disabled { background-color: #D6D6D6; color: #808080; }");
}

void View::EqualButtonClicked() {
  if (open_parenthesis_clicked_ == 0 && string_to_calculate_.length() != 0 &&
      operator_clicked_ == false) {
    const QString history_expression = string_to_calculate_;
    const QString history_display_expression = string_to_show_;
    const QString history_x_value = ui_->x_value_input->text();
    s21::FormatString formatted_str(string_to_calculate_);
    long double x_value = ui_->x_value_input->text().toDouble();
    long double result =
        controller_->Calculate(formatted_str.GetString(), x_value);

    SetResult(result);
    SaveHistoryRecord(history_expression, history_display_expression,
                      history_x_value, result);
  }
}

void View::SetResult(long double &result) {
  /// 清空所有输入状态标记
  ClearButtonClicked();

  if (std::isinf(result) || std::isnan(result)) {
    ui_->display->setText("calculation error");

  } else {
    long double truncated_result = truncl(result);

    /// 如果结果为浮点数
    if (fabs(result - truncated_result) > 1e-7) {
      string_to_calculate_ = TruncateZeros(result);
      point_clicked_ = true;

    } else {
      /// 如果结果为整数
      string_to_calculate_ = QString::number(result, 'L', 0);
    }

    /// 如果结果过长超出显示区域，则使用科学计数法
    if (string_to_calculate_.length() >= 21) {
      string_to_calculate_ = QString::number(result, 'e', 0);
      e_clicked_ = true;
    }

    string_to_show_ = string_to_calculate_;
    num_clicked_ = true;

    if (result != 0) {
      flag_first_zero_ = true;
    }

    ui_->display->setText(string_to_show_);

    if (string_to_calculate_.length() == 1 &&
        string_to_calculate_.back() == '0') {
      ClearButtonClicked();
    }
  }
}

void View::SaveHistoryRecord(const QString &expression,
                             const QString &display_expression,
                             const QString &x_value, long double result) {
  if (expression.trimmed().isEmpty() || std::isinf(result) ||
      std::isnan(result)) {
    return;
  }

  s21::HistoryRecord record;
  record.expression = expression;
  record.display_expression =
      display_expression.trimmed().isEmpty() ? expression : display_expression;
  record.x_value = x_value.trimmed().isEmpty() ? "0" : x_value;
  record.result = QString::number(static_cast<double>(result), 'g', 15);
  record.created_at =
      QDateTime::currentDateTimeUtc().toString(Qt::ISODateWithMs);

  if (!history_manager_.AppendHistory(record)) {
    const s21::HistoryErrorInfo error = history_manager_.GetLastError();
    statusBar()->showMessage(
        error.message.isEmpty() ? "History save failed" : error.message, 3000);
  }
}

void View::OpenHistoryWindow() {
  if (!EnsureAdvancedAccess("History")) {
    return;
  }

  QDialog dialog(this);
  dialog.setWindowTitle("Calculation History");
  dialog.resize(520, 420);

  QVBoxLayout *layout = new QVBoxLayout(&dialog);
  QLabel *title = new QLabel("Recent calculations", &dialog);
  QListWidget *history_list = new QListWidget(&dialog);
  QPushButton *load_button = new QPushButton("Load", &dialog);
  QPushButton *clear_button = new QPushButton("Clear", &dialog);
  QPushButton *close_button = new QPushButton("Close", &dialog);

  QHBoxLayout *button_layout = new QHBoxLayout();
  button_layout->addWidget(load_button);
  button_layout->addWidget(clear_button);
  button_layout->addStretch();
  button_layout->addWidget(close_button);

  layout->addWidget(title);
  layout->addWidget(history_list);
  layout->addLayout(button_layout);

  auto populate_history = [this, history_list]() {
    history_list->clear();
    const std::vector<s21::HistoryRecord> records =
        history_manager_.LoadRecent(100);
    if (history_manager_.GetLastError().code != s21::HistoryErrorCode::kNone) {
      QMessageBox::warning(this, "History",
                           history_manager_.GetLastError().message.isEmpty()
                               ? "Failed to read calculation history"
                               : history_manager_.GetLastError().message);
    }

    if (records.empty()) {
      QListWidgetItem *item = new QListWidgetItem("No calculation history");
      item->setFlags(item->flags() & ~Qt::ItemIsSelectable);
      history_list->addItem(item);
      return;
    }

    for (const s21::HistoryRecord &record : records) {
      const QString item_text = QString("%1 = %2\nx = %3    %4")
                                    .arg(record.display_expression,
                                         record.result, record.x_value,
                                         record.created_at);
      QListWidgetItem *item = new QListWidgetItem(item_text);
      item->setData(Qt::UserRole, record.record_id);
      history_list->addItem(item);
    }
  };

  connect(load_button, &QPushButton::clicked, &dialog, [&]() {
    QListWidgetItem *item = history_list->currentItem();
    if (item == nullptr || !item->data(Qt::UserRole).isValid()) {
      return;
    }

    const std::optional<s21::HistoryRecord> record =
        history_manager_.GetRecordById(item->data(Qt::UserRole).toString());
    if (!record.has_value()) {
      QMessageBox::warning(this, "History", "Selected history record not found");
      return;
    }
    LoadHistoryRecordToView(*record);
    dialog.accept();
  });
  connect(history_list, &QListWidget::itemDoubleClicked, &dialog,
          [&](QListWidgetItem *item) {
            if (item == nullptr || !item->data(Qt::UserRole).isValid()) {
              return;
            }

            const std::optional<s21::HistoryRecord> record =
                history_manager_.GetRecordById(
                    item->data(Qt::UserRole).toString());
            if (record.has_value()) {
              LoadHistoryRecordToView(*record);
              dialog.accept();
            }
          });
  connect(clear_button, &QPushButton::clicked, &dialog, [&]() {
    const QMessageBox::StandardButton answer = QMessageBox::question(
        &dialog, "Clear History", "Clear all calculation history?");
    if (answer != QMessageBox::Yes) {
      return;
    }
    if (!history_manager_.ClearHistory()) {
      QMessageBox::warning(&dialog, "History",
                           history_manager_.GetLastError().message.isEmpty()
                               ? "Failed to clear calculation history"
                               : history_manager_.GetLastError().message);
      return;
    }
    populate_history();
  });
  connect(close_button, &QPushButton::clicked, &dialog, &QDialog::reject);

  populate_history();
  dialog.exec();
}

void View::LoadHistoryRecordToView(const s21::HistoryRecord &record) {
  string_to_calculate_ = record.expression;
  string_to_show_ = record.display_expression.trimmed().isEmpty()
                        ? record.expression
                        : record.display_expression;
  ui_->x_value_input->setText(record.x_value);
  ui_->display->setText(string_to_show_.isEmpty() ? "0" : string_to_show_);
  RebuildInputState();
  statusBar()->showMessage(QString("Loaded history result: %1").arg(record.result),
                           3000);
}

void View::RebuildInputState() {
  num_clicked_ = false;
  point_clicked_ = false;
  operator_clicked_ = false;
  x_clicked_ = false;
  e_clicked_ = false;
  flag_first_zero_ = false;
  open_parenthesis_clicked_ = 0;

  for (const QChar symbol : string_to_calculate_) {
    if (symbol == '(') {
      open_parenthesis_clicked_++;
    } else if (symbol == ')') {
      open_parenthesis_clicked_ = std::max(0, open_parenthesis_clicked_ - 1);
    }
  }

  if (string_to_calculate_.isEmpty()) {
    return;
  }

  const QChar last_symbol = string_to_calculate_.back();
  num_clicked_ = last_symbol.isDigit() || last_symbol == '.';
  operator_clicked_ = IsBinaryOperatorAt(string_to_calculate_,
                                         string_to_calculate_.length() - 1) ||
                      HasIncompleteScientificNotationAtEnd(
                          string_to_calculate_);
  x_clicked_ = IsStandaloneOperandSymbol(last_symbol);

  int token_start = string_to_calculate_.length() - 1;
  while (token_start >= 0) {
    const QChar symbol = string_to_calculate_.at(token_start);
    if (symbol == '(' || symbol == ')' ||
        IsBinaryOperatorAt(string_to_calculate_, token_start)) {
      break;
    }
    if (symbol == '.') {
      point_clicked_ = true;
    }
    if (symbol == 'e') {
      e_clicked_ = true;
    }
    --token_start;
  }

  const QString current_token = string_to_calculate_.mid(token_start + 1);
  flag_first_zero_ = current_token != "0";
}

QString View::TruncateZeros(long double &value) {
  QString str_value = QString::number(value, 'f', 7);
  QString::iterator it = (str_value.end() - 1);

  while (*it == '0') {
    it--;
    str_value.chop(1);
  }

  return str_value;
}

void View::OpenGraphWindow() {
  if (!EnsureAdvancedAccess("Graph plotting")) {
    return;
  }

  /// 如果绘图窗口尚未打开
  if (graph_ == nullptr) {
    graph_ = new Graph(this);

    int main_window_x = this->x();
    int main_window_y = this->y();

    connect(graph_, &Graph::finished, this, &View::GraphWindowClosed);

    graph_->move(main_window_x + this->width() + 20, main_window_y);
    graph_->show();

  } else {
    /// 如果绘图窗口已经打开
    graph_->raise();
    graph_->activateWindow();
  }

  /// 如果当前表达式合法，则执行绘图
  if (open_parenthesis_clicked_ == 0 && string_to_calculate_.length() != 0 &&
      operator_clicked_ == false) {
    graph_->SetExpression(string_to_show_);

    s21::FormatString formatted_str(string_to_calculate_);
    std::string str_to_plot = formatted_str.GetString();
    std::pair<std::vector<double>, std::vector<double>> coordinates =
        controller_->CalculateGraphCoordinates(str_to_plot,
                                               graph_->GetXRange());

    graph_->BuildPlot(coordinates);

  } else if (string_to_calculate_.length() != 0) {
    graph_->Clear();
    graph_->SetExpression("invalid input");

  } else {
    /// 如果输入字符串为空，则仅清空图像
    graph_->Clear();
  }
}

void View::GraphWindowClosed() { graph_ = nullptr; }

void View::GetAllFlags() {
  std::cout << "-----------------------------" << std::endl;
  std::cout << "string: " << string_to_calculate_.toStdString() << std::endl;
  std::cout << "num_clicked_: " << num_clicked_ << std::endl;
  std::cout << "point_clicked_: " << point_clicked_ << std::endl;
  std::cout << "operator_clicked_: " << operator_clicked_ << std::endl;
  std::cout << "open_parenthesis_clicked_: " << open_parenthesis_clicked_
            << std::endl;
  std::cout << "x_clicked_: " << x_clicked_ << std::endl;
  std::cout << "e_clicked_: " << e_clicked_ << std::endl;
  std::cout << "flag_first_zero_: " << flag_first_zero_ << std::endl;
}
