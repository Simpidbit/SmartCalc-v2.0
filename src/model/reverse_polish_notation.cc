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

  /* check if a lexeme is a number or exponential notation or x */
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

  /* if `+` or `-` is an unary sign, push 0 to RPN list */
  if (IsUnary(it, str)) {
    Lexeme add_zero('0', Priority::kPriority_0, LexemeType::kNumber);
    rpn_list_.push_back(add_zero);
  }

  if (operators_stack.empty()) {
    operators_stack.push(new_element);

    /* if stack is not empty */
  } else {
    if (new_element.priority > operators_stack.top().priority) {
      operators_stack.push(new_element);
    } else {
      /* if current element priority is less or equal then top element priority,
       * pop and add top lexeme to the RPN list until '(' is met */
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
    element_priority = Priority::kPriority_3;
  } else {
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
    /* if the operator is first in the string */
    if (it == str.begin()) {
      return true;
    }

    /* if the operator follows '(' */
    if (*(it - 1) == '(') {
      return true;
    }
  }
  return false;
}
