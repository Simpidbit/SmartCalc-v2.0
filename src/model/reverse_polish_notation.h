#ifndef CPP3_SMARTCALC_V2_0_SRC_MODEL_REVERSE_POLISH_NOTATION_H_
#define CPP3_SMARTCALC_V2_0_SRC_MODEL_REVERSE_POLISH_NOTATION_H_

#include <list>
#include <stack>
#include <string>

#include "lexeme.h"

namespace s21 {

/**
 * @brief 将中缀表达式转换为逆波兰表达式（RPN）的类。
 * @details 该类接收字符串形式的表达式，将其拆分为词法单元并转换为 RPN，
 * 结果存储在 `std::list` 容器中。
 * 转换过程采用 Dijkstra 的 Shunting-yard（调度场）算法，
 * 基于栈管理尚未输出的运算符，并按顺序扫描输入 token 逐步生成输出序列。
 */
class ReversePolishNotation {
 public:
  /**
   * @brief 默认构造函数。
   */
  ReversePolishNotation() = default;
  /**
   * @brief 默认析构函数。
   */
  ~ReversePolishNotation() = default;

  /**
   * @brief 获取逆波兰表达式词法单元序列。
   * @return `std::list<Lexeme>` 类型的 RPN 词法单元列表。
   */
  std::list<Lexeme> GetRpnList() { return rpn_list_; };

  /**
   * @brief 将输入字符串表达式转换为 RPN 序列。
   * @param str 待转换的中缀表达式字符串。
   */
  void Convert(std::string &str);

 private:
  std::list<Lexeme> rpn_list_;  ///< 转换后的逆波兰词法单元列表

  /**
   * @brief 解析数字词法单元并追加到 RPN 列表。
   * @param it 指向数字起始位置的字符串迭代器。
   * @return 解析完成后，外层迭代器应前进的字符数。
   */
  size_t ParseNumber(std::string::iterator it);

  /**
   * @brief 解析当前运算符并根据优先级规则处理运算符栈。
   * @param operators_stack 运算符栈。
   * @param it 指向当前字符的迭代器。
   * @param str 输入表达式字符串。
   */
  void ParseOperator(std::stack<Lexeme> &operators_stack,
                     std::string::iterator it, std::string &str);

  /**
   * @brief 处理右括号：持续弹栈并输出，直到遇到左括号。
   * @param operators_stack 运算符栈。
   */
  void CloseParenth(std::stack<Lexeme> &operators_stack);

  /**
   * @brief 将左括号压入运算符栈。
   * @param operators_stack 运算符栈。
   */
  void PushOpenParenth(std::stack<Lexeme> &operators_stack);

  /**
   * @brief 获取当前符号对应的优先级。
   * @param it 指向当前符号的迭代器。
   * @return 当前符号对应的 Priority。
   */
  Priority GetPriority(std::string::iterator it);

  /**
   * @brief 判断 `+` 或 `-` 是否为一元符号。
   * @param it 指向当前符号的迭代器。
   * @param str 输入表达式字符串。
   * @return 若为一元符号返回 `true`，否则返回 `false`。
   */
  bool IsUnary(std::string::iterator it, std::string &str);

};  // class ReversePolishNotation

}  // namespace s21

#endif  // CPP3_SMARTCALC_V2_0_SRC_MODEL_REVERSE_POLISH_NOTATION_H_
