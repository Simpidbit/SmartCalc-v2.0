/**
 * @file view.cc
 * @brief Implements the main calculator window and its UI event handlers.
 */

#include "view.h"

#include <iostream>

#include "./ui_view.h"

/**
 * @brief Constructs the main calculator window and connects UI signals.
 * @param parent Pointer to the parent widget.
 * @param controller Pointer to the controller used for calculations and
 * plotting.
 */
View::View(QWidget *parent, s21::Controller *controller)
    : QMainWindow(parent), controller_(controller), ui_(new Ui::View) {
  ui_->setupUi(this);

  QRegularExpressionValidator *regexXValidator =
      new QRegularExpressionValidator(QRegularExpression(
          "^(-?100000000000(\\.0{1,6})?|(-?[0-9]{1,11})(\\.[0-9]{1,6})?)$"));
  ui_->x_value_input->setValidator(regexXValidator);

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
}

/**
 * @brief Destroys the main window and releases the generated UI.
 */
View::~View() { delete ui_; }

/**
 * @brief Resets the current expression, display text, and input state flags.
 */
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

/**
 * @brief Appends a digit to the current expression when input rules allow it.
 * @details The handler keeps the internal parser string and the user-facing
 * display string in sync, suppresses repeated leading zeroes inside a numeric
 * lexeme, and updates state flags that control whether digits, operators, or a
 * decimal point may follow.
 */
void View::NumberClicked() {
  QPushButton *button = qobject_cast<QPushButton *>(sender());

  // Suppress repeated leading zeroes such as `000` while a numeric lexeme is
  // still in its protected "starts with zero" state.
  if (string_to_calculate_.length() != 0 && !point_clicked_ &&
      string_to_calculate_.back() == '0' && button->text() == '0' &&
      !flag_first_zero_) {
    flag_first_zero_ = false;
  }

  // When the user replaces a protected leading zero with a non-zero digit,
  // drop the old zero first so `05` becomes `5`.
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
    // Non-zero digit branch.
    if (string_to_calculate_.length() == 0) {
      string_to_calculate_ += button->text();
      string_to_show_ += button->text();
      ui_->display->setText(string_to_show_);
      num_clicked_ = true;
      operator_clicked_ = false;
      flag_first_zero_ = true;

    } else if (string_to_calculate_.length() != 0 &&
               string_to_calculate_.back() != ')' &&
               string_to_calculate_.back() != 'x') {
      string_to_calculate_ += button->text();
      string_to_show_ += button->text();
      ui_->display->setText(string_to_show_);
      num_clicked_ = true;
      operator_clicked_ = false;
      flag_first_zero_ = true;
    }

  } else if ((flag_first_zero_ || (!num_clicked_ || point_clicked_)) &&
             button->text() == '0') {
    // Zero digit branch. This path keeps a single leading zero valid and also
    // allows zeroes after a decimal point.
    if (string_to_calculate_.length() == 0) {
      string_to_calculate_ += button->text();
      string_to_show_ += button->text();
      ui_->display->setText(string_to_show_);
      num_clicked_ = true;
      operator_clicked_ = false;

    } else if (string_to_calculate_.length() != 0 &&
               string_to_calculate_.back() != ')' &&
               string_to_calculate_.back() != 'x') {
      string_to_calculate_ += button->text();
      string_to_show_ += button->text();
      ui_->display->setText(string_to_show_);
      num_clicked_ = true;
      operator_clicked_ = false;
    }
  }
}

/**
 * @brief Handles insertion of `+` and `-` operators into the expression.
 */
void View::PlusMinusOperatorClicked() {
  if (string_to_calculate_.length() != 0 && !operator_clicked_ &&
      string_to_calculate_.back() != '.') {
    QPushButton *button = qobject_cast<QPushButton *>(sender());

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
    QPushButton *button = qobject_cast<QPushButton *>(sender());
    string_to_calculate_ += button->text();
    string_to_show_ += button->text();
    ui_->display->setText(string_to_show_);
    operator_clicked_ = true;
  }
}

/**
 * @brief Handles insertion of multiplication and division operators.
 */
void View::MulDivOperatorClicked() {
  if (!operator_clicked_ && string_to_calculate_.length() != 0 &&
      string_to_calculate_.back() != '(' &&
      string_to_calculate_.back() != '.') {
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

/**
 * @brief Inserts a mathematical function call and its opening parenthesis.
 */
void View::MathFunctionClicked() {
  bool flag = false;
  if (string_to_calculate_.length() != 0) {
    // A function name is valid only where a new operand may start.
    if (!num_clicked_ && !point_clicked_ &&
        string_to_calculate_.back() != 'x' &&
        string_to_calculate_.back() != ')') {
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
    // Allow starting an expression, or a parenthesized subexpression, with a
    // function token that immediately opens its argument list.
    QPushButton *button = qobject_cast<QPushButton *>(sender());
    string_to_calculate_ += button->text() + "(";
    string_to_show_ += button->text() + "(";
    ui_->display->setText(string_to_show_);
    open_parenthesis_clicked_++;
    num_clicked_ = false;
    operator_clicked_ = false;
  }
}

/**
 * @brief Inserts an opening parenthesis when it is valid in the current state.
 */
void View::OpenParenthesisButtonClicked() {
  if (string_to_calculate_.length() == 0) {
    QPushButton *button = qobject_cast<QPushButton *>(sender());
    string_to_calculate_ += button->text();
    string_to_show_ += button->text();
    ui_->display->setText(string_to_show_);
    open_parenthesis_clicked_++;

  } else if (string_to_calculate_.length() != 0 &&
             (operator_clicked_ || open_parenthesis_clicked_ > 0) &&
             string_to_calculate_.back() != 'x' && !num_clicked_) {
    // Inside a non-empty expression, `(` is allowed only when it begins a new
    // operand after an operator or another opening parenthesis.
    QPushButton *button = qobject_cast<QPushButton *>(sender());
    string_to_calculate_ += button->text();
    string_to_show_ += button->text();
    ui_->display->setText(string_to_show_);
    open_parenthesis_clicked_++;
    operator_clicked_ = false;
    num_clicked_ = false;
  }
}

/**
 * @brief Inserts a closing parenthesis if there is an unmatched opening one.
 */
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

/**
 * @brief Inserts a decimal point into the current numeric lexeme.
 */
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

/**
 * @brief Inserts the modulo operator into the current expression.
 */
void View::ModButtonClicked() {
  if (!operator_clicked_ && (num_clicked_ || x_clicked_) &&
      string_to_calculate_.back() != '.' && !e_clicked_) {
    string_to_calculate_ += "%";
    string_to_show_ += "%";
    ui_->display->setText(string_to_show_);
    operator_clicked_ = true;
    point_clicked_ = false;
    num_clicked_ = false;
    x_clicked_ = false;
  }
}

/**
 * @brief Inserts the power operator followed by an opening parenthesis.
 */
void View::PowButtonClicked() {
  if (string_to_calculate_.length() != 0 &&
      string_to_calculate_.back() != '.' &&
      string_to_calculate_.back() != 'e' &&
      (num_clicked_ || string_to_calculate_.back() == ')' ||
       string_to_calculate_.back() == 'x')) {
    string_to_calculate_ += "^(";
    string_to_show_ += "^(";
    ui_->display->setText(string_to_show_);
    open_parenthesis_clicked_++;
    num_clicked_ = false;
    x_clicked_ = false;
  }
}

/**
 * @brief Inserts the square root function token into the expression.
 */
void View::SqrtButtonClicked() {
  if (string_to_calculate_.length() == 0) {
    QPushButton *button = qobject_cast<QPushButton *>(sender());
    string_to_calculate_ += "r(";
    string_to_show_ += button->text() + "(";
    ui_->display->setText(string_to_show_);
    open_parenthesis_clicked_++;
    operator_clicked_ = false;

  } else if (string_to_calculate_.length() != 0 &&
             (operator_clicked_ || string_to_calculate_.back() == '(')) {
    QPushButton *button = qobject_cast<QPushButton *>(sender());
    string_to_calculate_ += "r(";
    string_to_show_ += button->text() + "(";
    ui_->display->setText(string_to_show_);
    point_clicked_ = false;
    open_parenthesis_clicked_++;
    operator_clicked_ = false;
  }
}

/**
 * @brief Inserts the variable `x` into the expression when allowed.
 */
void View::XButtonClicked() {
  if ((string_to_calculate_.length() == 0 || operator_clicked_) ||
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

/**
 * @brief Inserts scientific notation marker `e` for the current number.
 */
void View::EButtonClicked() {
  if (!e_clicked_ && num_clicked_ && !point_clicked_) {
    QPushButton *button = qobject_cast<QPushButton *>(sender());
    string_to_show_ += button->text();
    string_to_calculate_ += button->text();
    ui_->display->setText(string_to_show_);
    e_clicked_ = true;
  }
}

/**
 * @brief Removes a given number of characters from both expression strings.
 * @param number_to_chop Number of trailing characters to remove.
 */
void View::ChopString(size_t number_to_chop) {
  string_to_calculate_.chop(number_to_chop);
  string_to_show_.chop(number_to_chop);
}

/**
 * @brief Removes the last entered token and restores the related state flags.
 * @details Depending on what was entered last, the function may remove one
 * character, an entire multi-character function call prefix, or a paired token
 * such as `^(`. After deletion it recalculates the input-state flags from the
 * new tail of the expression so validation rules remain consistent.
 */
void View::BackspaceClicked() {
  if (string_to_calculate_.length() != 0) {
    if (string_to_calculate_.length() == 1) {
      ClearButtonClicked();
      return;
    }

    // Cache the tail substrings used to recognize multi-character function
    // tokens before any deletion changes the current expression.
    QString last_five_chars = string_to_calculate_.right(5);
    QString last_four_chars = string_to_calculate_.right(4);
    QString last_three_chars = string_to_calculate_.right(3);
    QString last_two_chars = string_to_calculate_.right(2);

    if (string_to_calculate_.back() == '.') {
      // Removing a decimal point reopens the possibility of inserting one
      // again inside the current numeric lexeme.
      ChopString(1);
      point_clicked_ = false;
      QString::ConstIterator str = string_to_calculate_.end() - 1;
      flag_first_zero_ = GetZeroStatus(str);

    } else if (string_to_calculate_.back() == '%' ||
               string_to_calculate_.back() == '+' ||
               string_to_calculate_.back() == '-' ||
               string_to_calculate_.back() == '*' ||
               string_to_calculate_.back() == '/') {
      // After deleting a binary operator, rebuild flags from the preceding
      // operand because decimal-point and exponent status may become active
      // again.
      ChopString(1);
      QString::ConstIterator str = string_to_calculate_.end() - 1;
      point_clicked_ = GetPointStatus(str);
      flag_first_zero_ = GetZeroStatus(str);
      e_clicked_ = GetEStatus(str);

    } else if (string_to_calculate_.back() == ')') {
      // Deleting `)` restores one unmatched opening parenthesis.
      ChopString(1);
      open_parenthesis_clicked_++;

    } else if (string_to_calculate_.back() == 'x') {
      // The internal parser string stores `x` as one symbol, while the
      // user-facing string may contain a longer button label.
      string_to_calculate_.chop(1);
      string_to_show_.chop(2);

    } else if (string_to_calculate_.back() == 'e') {
      // Deleting the scientific-notation marker turns exponent mode off.
      ChopString(1);
      e_clicked_ = false;

    } else if (last_five_chars == "acos(" || last_five_chars == "asin(" ||
               last_five_chars == "atan(") {
      // Remove the complete function prefix together with its auto-inserted
      // opening parenthesis.
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

    } else { /* if number */
      // Default numeric branch: remove the last digit from the current lexeme.
      ChopString(1);
    }

    if (string_to_calculate_.length() == 0) {
      ClearButtonClicked();

    } else {
      // Recompute the tail-state flags from the new last symbol so the next
      // input operation uses consistent validation rules.
      ui_->display->setText(string_to_show_);
      QString::ConstIterator str = string_to_calculate_.end() - 1;
      num_clicked_ = GetNumStatus(str);
      operator_clicked_ = GetOperatorStatus(str);
      x_clicked_ = GetXStatus(str);
    }

  } else { /* string is empty */

    ClearButtonClicked();
  }
}

/**
 * @brief Checks whether the current numeric lexeme already contains a decimal
 * point.
 * @param str Iterator positioned at the last character of the expression.
 * @return `true` if a decimal point exists in the current lexeme, otherwise
 * `false`.
 */
bool View::GetPointStatus(QString::ConstIterator str) {
  // Walk backward only within the current lexeme. Encountering an operator or
  // parenthesis means the scan has reached the lexeme boundary.
  while (!str->isNull() && *str != '+' && *str != '-' && *str != '*' &&
         *str != '/' && *str != '(' && *str != ')') {
    if (*str == '.') {
      return true;
      break;
    }
    --str;
  }
  return false;
}

/**
 * @brief Checks whether the last character is a binary operator.
 * @param str Iterator positioned at the last character of the expression.
 * @return `true` if the current character is an operator, otherwise `false`.
 */
bool View::GetOperatorStatus(QString::ConstIterator str) {
  if (!str->isNull() &&
      (*str == '+' || *str == '-' || *str == '*' || *str == '/')) {
    return true;
  }
  return false;
}

/**
 * @brief Determines whether leading zero protection should remain enabled.
 * @details Returns `false` when the current numeric lexeme still starts with a
 * protected zero, for example a standalone `0` or `0.` immediately following
 * the expression start or a binary operator. In all other cases, additional
 * digits may be appended normally.
 * @param str Iterator positioned at the last character of the expression.
 * @return `true` if entering another leading zero is allowed, otherwise
 * `false`.
 */
bool View::GetZeroStatus(QString::ConstIterator str) {
  // A plain trailing `0` keeps the lexeme in protected-leading-zero mode.
  if (!str->isNull() && *str == '0') {
    return false;

    // For `0.` we must inspect the symbol before the zero to distinguish a
    // real leading zero from a regular decimal number fragment.
  } else if (!str->isNull() && *str == '.') {
    if (*(--str) == '0') {
      if (!(str - 1)->isNull() && (*(str - 1) == '+' || *(str - 1) == '-' ||
                                   *(str - 1) == '*' || *(str - 1) == '/')) {
        return false;

      } else if ((str - 1)->isNull()) {
        return false;
      }
    }
  }
  return true;
}

/**
 * @brief Checks whether the last character belongs to a numeric lexeme.
 * @param str Iterator positioned at the last character of the expression.
 * @return `true` if the current character is a digit or decimal point,
 * otherwise `false`.
 */
bool View::GetNumStatus(QString::ConstIterator str) {
  if (!str->isNull() && str->isDigit() || *str == '.') {
    return true;
  }
  return false;
}

/**
 * @brief Checks whether the last token is the variable `x`.
 * @param str Iterator positioned at the last character of the expression.
 * @return `true` if the current character is `x`, otherwise `false`.
 */
bool View::GetXStatus(QString::ConstIterator str) {
  if (!str->isNull() && *str == 'x') {
    return true;
  }
  return false;
}

/**
 * @brief Checks whether the current number uses scientific notation.
 * @param str Iterator positioned at the last character of the expression.
 * @return `true` if an `e` marker is part of the current numeric lexeme,
 * otherwise `false`.
 */
bool View::GetEStatus(QString::ConstIterator str) {
  if (!str->isNull() && *str == 'e') {
    return true;
  }

  str -= 2;

  if (!str->isNull() && *str == 'e') {
    return true;
  }
  return false;
}

/**
 * @brief Evaluates the current expression and sends the result to the display.
 */
void View::EqualButtonClicked() {
  if (open_parenthesis_clicked_ == 0 && string_to_calculate_.length() != 0 &&
      operator_clicked_ == false) {
    // Translate the UI-oriented expression into the compact token alphabet
    // expected by the parser before delegating evaluation to the controller.
    s21::FormatString formatted_str(string_to_calculate_);
    long double x_value = ui_->x_value_input->text().toDouble();
    long double result =
        controller_->Calculate(formatted_str.GetString(), x_value);

    SetResult(result);
  }
}

/**
 * @brief Formats a calculation result and updates the display state.
 * @details The function clears previous input state, converts valid finite
 * results either to integer, trimmed floating-point, or scientific notation,
 * and preserves the formatted result as the next editable expression. Invalid
 * numeric results are shown as `calculation error`.
 * @param result Result value returned by the controller.
 */
void View::SetResult(long double &result) {
  /* clear all flags */
  ClearButtonClicked();

  if (std::isinf(result) || std::isnan(result)) {
    ui_->display->setText("calculation error");

  } else {
    long double truncated_result = truncl(result);

    // Preserve fractional precision only when the result is not effectively an
    // integer.
    if (fabs(result - truncated_result) > 1e-7) {
      string_to_calculate_ = TruncateZeros(result);
      point_clicked_ = true;

      // Integer results are displayed without a fractional part.
    } else {
      string_to_calculate_ = QString::number(result, 'L', 0);
    }

    // Switch to scientific notation when the plain representation would exceed
    // the available display width.
    if (string_to_calculate_.length() >= 21) {
      string_to_calculate_ = QString::number(result, 'e', 0);
      e_clicked_ = true;
    }

    // Keep the formatted result as the next editable expression so users can
    // continue calculations from the displayed value.
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

/**
 * @brief Removes redundant trailing zeroes from a floating-point string.
 * @param value Numeric value to format.
 * @return QString containing the trimmed decimal representation.
 */
QString View::TruncateZeros(long double &value) {
  QString str_value = QString::number(value, 'f', 7);
  QString::iterator it = (str_value.end() - 1);

  while (*it == '0') {
    it--;
    str_value.chop(1);
  }

  return str_value;
}

/**
 * @brief Opens the graph window and plots the current expression when valid.
 * @details If no graph dialog exists, the function creates one next to the
 * main window and wires its close signal back to the view. It then validates
 * the current expression: valid input is converted and plotted, invalid
 * non-empty input clears the plot and shows `invalid input`, and empty input
 * simply clears the graph.
 */
void View::OpenGraphWindow() {
  /* if the window isn't open */
  if (graph_ == nullptr) {
    graph_ = new Graph(this);

    int main_window_x = this->x();
    int main_window_y = this->y();

    connect(graph_, &Graph::finished, this, &View::GraphWindowClosed);

    graph_->move(main_window_x + this->width() + 20, main_window_y);
    graph_->show();

    /* if the window is already open */
  } else {
    graph_->raise();
    graph_->activateWindow();
  }

  /* if expression is valid */
  if (open_parenthesis_clicked_ == 0 && string_to_calculate_.length() != 0 &&
      operator_clicked_ == false) {
    graph_->SetExpression(string_to_show_);

    // Reuse the same token conversion as normal evaluation so graph plotting
    // operates on the parser-ready expression form.
    s21::FormatString formatted_str(string_to_calculate_);
    std::string str_to_plot = formatted_str.GetString();
    std::pair<std::vector<double>, std::vector<double>> coordinates =
        controller_->CalculateGraphCoordinates(str_to_plot,
                                               graph_->GetXRange());

    graph_->BuildPlot(coordinates);

  } else if (string_to_calculate_.length() != 0) {
    graph_->Clear();
    graph_->SetExpression("invalid input");

    /* input string is empty */
  } else {
    graph_->Clear();
  }
}

/**
 * @brief Resets the stored graph window pointer after the dialog closes.
 */
void View::GraphWindowClosed() { graph_ = nullptr; }

/**
 * @brief Prints the current input flags and expression state for debugging.
 */
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
