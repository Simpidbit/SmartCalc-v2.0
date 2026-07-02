/**
 * @file reverse_polish_notation.cc
 * @brief Implements conversion from infix expressions to Reverse Polish Notation.
 */

#include "reverse_polish_notation.h"

using namespace s21;

/**
 * @brief Converts an infix expression to Reverse Polish Notation (RPN).
 * @details The function applies the Shunting-yard algorithm: it scans the
 * input string from left to right, writes numbers directly to the RPN list,
 * manages operators in a precedence-aware stack, and flushes any remaining
 * operators after the scan completes.
 * @param str Infix expression to convert.
 */
void ReversePolishNotation::Convert(std::string &str) {
  std::stack<Lexeme> operators;

  size_t move_iter = 1;  ///< value to move the iterator
                         ///< in the string after parsing a lexeme

  std::string::iterator symbol = str.begin();
  while (symbol != str.end()) {
    move_iter = 1;
    // Dispatch each symbol to the parser branch that knows how many
    // characters belong to the current lexeme.
    if (std::isdigit(*symbol) || *symbol == 'x') {
      move_iter = ParseNumber(symbol);
    } else if (*symbol == '(') {
      PushOpenParenth(operators);
    } else if (*symbol == ')') {
      CloseParenth(operators);
    } else {
      ParseOperator(operators, symbol, str);
    }
    symbol += move_iter;
  }

  // Once the input ends, every remaining operator can be emitted in stack
  // order because no later token can affect precedence anymore.
  while (!operators.empty()) {
    rpn_list_.push_back(operators.top());
    operators.pop();
  }
}

/**
 * @brief Parses a number and writes it to the RPN list.
 * @details The parser reads decimal numbers, scientific notation, and the
 * variable `x`. It also accepts the sign immediately following `e`, such as
 * in `1.2e-3`. The returned value tells the outer loop how far to advance.
 * @param it Iterator pointing to the first character of the number.
 * @return Number of characters consumed from the input.
 */
size_t ReversePolishNotation::ParseNumber(std::string::iterator it) {
  Lexeme new_lexeme;

  // Extend the lexeme while characters still belong to the same numeric token,
  // including scientific notation such as `1e-7`, or the standalone variable
  // `x`.
  while (std::isdigit(*it) || *it == '.' || *it == 'e' ||
         (*it == '-' && *(it - 1) == 'e') || (*it == '+' && *(it - 1) == 'e') ||
         *it == 'x') {
    new_lexeme.value.push_back(*it);
    ++it;
  }
  new_lexeme.priority = Priority::kPriority_0;
  new_lexeme.type = LexemeType::kNumber;
  rpn_list_.push_back(new_lexeme);
  size_t move_iter = new_lexeme.value.length();
  return (move_iter);
}

/**
 * @brief Parses and processes a single operator.
 * @details When `+` or `-` is recognized as unary, the function first outputs
 * a `0` to transform it into equivalent binary semantics. It then processes
 * the operator stack according to precedence rules, popping operators while
 * the current one has lower or equal precedence than the stack top.
 * @param operators_stack Operator stack.
 * @param it Iterator pointing to the current operator.
 * @param str Original input expression.
 */
void ReversePolishNotation::ParseOperator(std::stack<Lexeme> &operators_stack,
                                          std::string::iterator it,
                                          std::string &str) {
  Lexeme new_element(*it, GetPriority(it), LexemeType::kOperator);

  // Represent unary `+` and `-` as binary operations against zero so the
  // evaluator can use one consistent operator model.
  if (IsUnary(it, str)) {
    Lexeme add_zero('0', Priority::kPriority_0, LexemeType::kNumber);
    rpn_list_.push_back(add_zero);
  }

  if (operators_stack.empty()) {
    operators_stack.push(new_element);

    // Otherwise compare precedence against the stack top and emit any operators
    // that must be evaluated before the current one.
  } else {
    if (new_element.priority > operators_stack.top().priority) {
      operators_stack.push(new_element);
    } else {
      // Stop at `(` because it is only a grouping marker and must never appear
      // in the final RPN output.
      while (!operators_stack.empty() &&
             (new_element.priority <= operators_stack.top().priority) &&
             operators_stack.top().value != "(") {
        Lexeme top_lexeme = operators_stack.top();
        rpn_list_.push_back(top_lexeme);
        operators_stack.pop();
      }
      operators_stack.push(new_element);
    }
  }
}

/**
 * @brief Handles a closing parenthesis.
 * @details Operators are popped and appended to the RPN list until an opening
 * parenthesis is found. The opening parenthesis itself is used only for
 * grouping and is not written to the output list.
 * @param operators_stack Operator stack.
 */
void ReversePolishNotation::CloseParenth(std::stack<Lexeme> &operators_stack) {
  Lexeme element;
  while (!operators_stack.empty()) {
    element = operators_stack.top();
    if (element.value == "(") {
      // Discard the matching opening parenthesis instead of emitting it.
      operators_stack.pop();
      break;
    }
    rpn_list_.push_back(element);
    operators_stack.pop();
  }
}

/**
 * @brief Pushes an opening parenthesis onto the operator stack.
 * @param operators_stack Operator stack.
 */
void ReversePolishNotation::PushOpenParenth(
    std::stack<Lexeme> &operators_stack) {
  Lexeme parenthesis('(', Priority::kPriority_0, LexemeType::kOperator);
  operators_stack.push(parenthesis);
}

/**
 * @brief Returns the precedence level of the current symbol.
 * @details `+` and `-` are level 1, `*`, `/`, and `%` are level 2, `^` and
 * `r` are level 3, and the remaining function-like symbols are level 4.
 * @param it Iterator pointing to the current symbol.
 * @return `Priority` corresponding to the current symbol.
 */
Priority ReversePolishNotation::GetPriority(std::string::iterator it) {
  Priority element_priority;
  if (*it == '(') {
    element_priority = Priority::kPriority_0;
  } else if (*it == '+' || *it == '-') {
    element_priority = Priority::kPriority_1;
  } else if (*it == '*' || *it == '/' || *it == '%') {
    element_priority = Priority::kPriority_2;
  } else if (*it == '^' || *it == 'r') { /* pow and sqrt */
    // `r` is the compact internal token used for square root.
    element_priority = Priority::kPriority_3;
  } else {
    // Trigonometric and logarithmic function markers are evaluated after their
    // argument has been fully formed, so they receive the highest priority.
    element_priority = Priority::kPriority_4;
  }
  return element_priority;
}

/**
 * @brief Determines whether the current `+` or `-` is unary.
 * @details A plus or minus sign is treated as unary when it appears at the
 * beginning of the expression or immediately after an opening parenthesis.
 * @param it Iterator pointing to the current symbol.
 * @param str Original input expression.
 * @return `true` if the symbol is unary, otherwise `false`.
 */
bool ReversePolishNotation::IsUnary(std::string::iterator it,
                                    std::string &str) {
  if (*it == '+' || *it == '-') {
    // A sign at the beginning of the whole expression is unary.
    if (it == str.begin()) {
      return true;
    }

    // A sign immediately after `(` starts a nested signed operand, so it is
    // also unary.
    if (*(it - 1) == '(') {
      return true;
    }
  }
  return false;
}
