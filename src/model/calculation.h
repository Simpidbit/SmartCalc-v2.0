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
 * @brief 根据数学表达式执行算术与三角计算。
 * @details 该类负责解析数学表达式，将其转换为逆波兰表示法并完成求值。
 * 对于包含变量 `x` 的表达式，也支持在计算时进行变量替换。
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
   * @brief 获取计算结果。
   * @return 以 `long double` 形式返回计算结果。
   */
  long double GetResult();

  /**
   * @brief 解析表达式并完成求值。
   * @details 该函数会遍历逆波兰表达式，将数字压入栈中，在遇到运算符时执行对应运算，
   * 并将中间结果继续压栈，最终结果保存在 `calculation_result_` 中。
   * @param expression 待计算的输入表达式。
   * @param x_value 变量 `x` 的取值。
   */
  void Parse(std::string &expression, long double x_value);

 private:
  ReversePolishNotation rpn_;  ///< 负责将输入表达式转换为逆波兰表达式的对象。
  std::list<Lexeme> expression_;    ///< 待计算的逆波兰表达式。
  long double calculation_result_;  ///< 计算结果。

  /**
   * @brief 根据给定运算符和操作数执行具体计算。
   * @param current_operator 包含待执行运算符的词法单元引用。
   * @param numbers 保存操作数的栈。
   * @return 计算得到的结果。
   */
  long double Calculate(s21::Lexeme &current_operator,
                        std::stack<long double> &numbers);

};

}

#endif
