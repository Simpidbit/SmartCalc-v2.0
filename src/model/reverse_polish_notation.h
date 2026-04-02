#ifndef CPP3_SMARTCALC_V2_0_SRC_MODEL_REVERSE_POLISH_NOTATION_H_
#define CPP3_SMARTCALC_V2_0_SRC_MODEL_REVERSE_POLISH_NOTATION_H_

#include <list>
#include <stack>
#include <string>

#include "lexeme.h"

namespace s21 {

/**
 * @brief 将表达式转换为逆波兰表示法的类。
 * @details 该类会读取字符串表达式，将其拆分为词法单元，并使用 Dijkstra 的
 * 调度场算法将中缀表达式转换为逆波兰表达式，最终结果保存在 `std::list` 中。
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
   * @brief 获取逆波兰表达式词法单元列表。
   * @return 以 `std::list<Lexeme>` 形式返回逆波兰表达式。
   */
  std::list<Lexeme> GetRpnList() { return rpn_list_; };

  /**
   * @brief 将字符串表达式转换为逆波兰词法单元列表。
   * @param str 待转换的表达式字符串。
   */
  void Convert(std::string &str);

 private:
  std::list<Lexeme> rpn_list_;  ///< 保存逆波兰表达式词法单元的列表。

  /**
   * @brief 将数字解析为单个词法单元并压入逆波兰列表。
   * @param it 指向数字首字符的迭代器。
   * @return 解析完成后字符串迭代器需要移动的步数。
   */
  size_t ParseNumber(std::string::iterator it);

  /**
   * @brief 解析运算符并将其压入运算符栈。
   * @param operators_stack 运算符栈。
   * @param it 指向当前字符串元素的迭代器。
   * @param str 输入表达式字符串。
   */
  void ParseOperator(std::stack<Lexeme> &operators_stack,
                     std::string::iterator it, std::string &str);

  /**
   * @brief 将运算符栈内容弹出并加入逆波兰列表，直到遇到 `(`。
   * @param operators_stack 运算符栈。
   */
  void CloseParenth(std::stack<Lexeme> &operators_stack);

  /**
   * @brief 将左括号压入运算符栈。
   * @param operators_stack 运算符栈。
   */
  void PushOpenParenth(std::stack<Lexeme> &operators_stack);

  /**
   * @brief 获取当前元素的优先级。
   * @param it 指向当前元素的迭代器。
   * @return 当前元素的优先级。
   */
  Priority GetPriority(std::string::iterator it);

  /**
   * @brief 判断 `-` 或 `+` 是否为一元运算符。
   * @param it 指向当前元素的迭代器。
   * @param str 输入表达式字符串。
   * @return 若为一元运算符则返回 `true`，否则返回 `false`。
   */
  bool IsUnary(std::string::iterator it, std::string &str);

};

}

#endif
