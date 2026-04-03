#include "reverse_polish_notation.h"

using namespace s21;

/**
 * @brief 将中缀表达式转换为逆波兰表达式（RPN）。
 * @details 使用 Shunting-yard 算法：顺序扫描输入字符串，
 * 数字直接输出到 RPN 列表，运算符依据优先级在运算符栈中进出，
 * 括号触发局部出栈，扫描结束后将栈中剩余运算符全部输出。
 * @param str 待转换的中缀表达式。
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
 * @brief 解析数字（含小数、科学计数法、变量 x）并写入 RPN 列表。
 * @details 连续读取数字相关字符，支持 `e` 指数表示中紧随其后的符号位，
 * 例如 `1.2e-3`。解析完成后返回外层迭代器应前进的位移。
 * @param it 指向当前数字起始位置的迭代器。
 * @return size_t 外层迭代器位移长度。
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
 * @brief 解析并处理一个运算符。
 * @details 当 `+/-` 被识别为一元符号时，先向 RPN 输出一个 `0`，
 * 将其转化为二元运算语义；随后依据优先级规则处理运算符栈：
 * 当前运算符优先级低于或等于栈顶时持续出栈（直到左括号或栈空）。
 * @param operators_stack 运算符栈。
 * @param it 指向当前运算符的迭代器。
 * @param str 原始输入表达式。
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
 * @brief 处理右括号。
 * @details 持续弹出运算符并输出到 RPN 列表，直到遇到左括号；
 * 左括号仅用于分组控制，不写入输出列表。
 * @param operators_stack 运算符栈。
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
 * @brief 将左括号压入运算符栈。
 * @param operators_stack 运算符栈。
 */
void ReversePolishNotation::PushOpenParenth(
    std::stack<Lexeme> &operators_stack) {
  Lexeme parenthesis('(', Priority::kPriority_0, LexemeType::kOperator);
  operators_stack.push(parenthesis);
}

/**
 * @brief 获取当前符号的优先级。
 * @details `+/-` 为一级，`*//%` 为二级，`^/r` 为三级，
 * 其余函数类符号为四级。
 * @param it 指向当前符号的迭代器。
 * @return Priority 当前符号对应优先级。
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
 * @brief 判断当前 `+` 或 `-` 是否为一元符号。
 * @details 在表达式起始位置，或紧跟左括号 `(` 时，
 * 视为一元正负号。
 * @param it 指向当前符号的迭代器。
 * @param str 原始输入表达式。
 * @return bool 是一元符号返回 `true`，否则返回 `false`。
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
