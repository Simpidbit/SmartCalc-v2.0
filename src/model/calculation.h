#ifndef CPP3_SMARTCALC_V2_0_SRC_MODEL_CALCULATION_H_
#define CPP3_SMARTCALC_V2_0_SRC_MODEL_CALCULATION_H_

#include <list>
#include <stack>
#include <stdexcept>
#include <string>

#include "cmath"
#include "lexeme.h"
#include "reverse_polish_notation.h"

namespace s21 {

/**
 * @brief 基于数学表达式执行算术与三角函数计算的类。
 * @details 该类负责解析输入表达式，将其中缀形式转换为逆波兰表达式（RPN），
 * 并按逆波兰序列完成求值。支持在表达式中使用变量 `x`，
 * 计算完成后可通过 GetResult 获取结果。
 */
class Calculation {
 public:
  /**
   * @brief 默认构造函数。
   */
  Calculation() = default;

  /**
   * @brief 默认析构函数。
   */
  ~Calculation() = default;

  /**
   * @brief 获取最近一次计算结果。
   * @return long double 类型的计算结果。
   */
  long double GetResult();

  /**
   * @brief 解析并计算输入表达式。
   * @details 函数会遍历逆波兰表达式序列：遇到数字（或变量 x）则入栈，
   * 遇到运算符则从栈中取操作数执行运算，并将结果重新压栈。
   * 序列处理结束后，栈顶值即为最终结果，保存到 `calculation_result_`。
   * @param expression 待计算的输入表达式。
   * @param x_value 变量 x 的替换值。
   */
  void Parse(std::string &expression, long double x_value);

 private:
  ReversePolishNotation rpn_;  ///< 负责将输入表达式转换为逆波兰表达式的对象
  std::list<Lexeme> expression_;    ///< 待计算的逆波兰表达式序列
  long double calculation_result_;  ///< 计算结果

  /**
   * @brief 根据当前运算符与操作数执行具体计算。
   * @param current_operator 当前运算符对应的词法单元引用。
   * @param numbers 存放操作数的栈。
   * @return long double 类型的运算结果。
   */
  long double Calculate(s21::Lexeme &current_operator,
                        std::stack<long double> &numbers);

};  // class Calculation

}  // namespace s21

#endif  // CPP3_SMARTCALC_V2_0_SRC_MODEL_CALCULATION_H_
