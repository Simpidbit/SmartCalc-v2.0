#include "reverse_polish_notation.h"

#include <cctype>

using namespace s21;

void ReversePolishNotation::Convert(std::string &str) {
  rpn_list_.clear();

  std::stack<Lexeme> operators;

  size_t move_iter = 1;  ///< 解析完一个词法单元后，字符串迭代器需要前进的步数。

  std::string::iterator symbol = str.begin();
  while (symbol != str.end()) {
    move_iter = 1;
    if (std::isdigit(static_cast<unsigned char>(*symbol)) || *symbol == 'x' ||
        *symbol == 'e' || *symbol == 'E') {
      move_iter = ParseNumber(symbol, str.end());
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

size_t ReversePolishNotation::ParseNumber(std::string::iterator it,
                                          std::string::iterator end) {
  Lexeme new_lexeme;

  if (*it == 'x' || *it == 'e' || *it == 'E') {
    new_lexeme.value.push_back(*it);
    new_lexeme.priority = Priority::kPriority_0;
    new_lexeme.type = LexemeType::kNumber;
    rpn_list_.push_back(new_lexeme);
    return 1;
  }

  /// 判断当前词法单元是否为普通数字或科学计数法数字。
  while (it != end &&
         (std::isdigit(static_cast<unsigned char>(*it)) || *it == '.' ||
          *it == 'e' || *it == 'E' ||
          ((*it == '-' || *it == '+') && !new_lexeme.value.empty() &&
           (new_lexeme.value.back() == 'e' ||
            new_lexeme.value.back() == 'E')))) {
    new_lexeme.value.push_back(*it);
    ++it;
  }
  new_lexeme.priority = Priority::kPriority_0;
  new_lexeme.type = LexemeType::kNumber;
  rpn_list_.push_back(new_lexeme);
  size_t move_iter = new_lexeme.value.length();
  return (move_iter);
}

void ReversePolishNotation::ParseOperator(std::stack<Lexeme> &operators_stack,
                                          std::string::iterator it,
                                          std::string &str) {
  Lexeme new_element(*it, GetPriority(it), LexemeType::kOperator);

  /// 如果 `+` 或 `-` 是一元符号，则先向逆波兰列表压入 0
  if (IsUnary(it, str)) {
    Lexeme add_zero('0', Priority::kPriority_0, LexemeType::kNumber);
    rpn_list_.push_back(add_zero);
  }

  if (operators_stack.empty()) {
    operators_stack.push(new_element);

  } else {
    /// 如果运算符栈非空
    if (new_element.priority > operators_stack.top().priority) {
      operators_stack.push(new_element);
    } else {
      /// 当当前元素优先级小于或等于栈顶元素优先级时，
      /// 持续弹出栈顶并加入逆波兰列表，直到遇到 `(`。
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

void ReversePolishNotation::PushOpenParenth(
    std::stack<Lexeme> &operators_stack) {
  Lexeme parenthesis('(', Priority::kPriority_0, LexemeType::kOperator);
  operators_stack.push(parenthesis);
}

Priority ReversePolishNotation::GetPriority(std::string::iterator it) {
  Priority element_priority;
  if (*it == '(') {
    element_priority = Priority::kPriority_0;
  } else if (*it == '+' || *it == '-') {
    element_priority = Priority::kPriority_1;
  } else if (*it == '*' || *it == '/' || *it == '%') {
    element_priority = Priority::kPriority_2;
  } else if (*it == '^' || *it == 'r') {
    /// 幂运算与平方根
    element_priority = Priority::kPriority_3;
  } else {
    element_priority = Priority::kPriority_4;
  }
  return element_priority;
}

bool ReversePolishNotation::IsUnary(std::string::iterator it,
                                    std::string &str) {
  if (*it == '+' || *it == '-') {
    /// 如果该运算符位于字符串开头
    if (it == str.begin()) {
      return true;
    }

    /// 如果该运算符紧跟在左括号之后
    if (*(it - 1) == '(') {
      return true;
    }
  }
  return false;
}
