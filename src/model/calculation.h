#ifndef CPP3_SMARTCALC_V2_0_SRC_MODEL_CALCULATION_H_
#define CPP3_SMARTCALC_V2_0_SRC_MODEL_CALCULATION_H_

/**
 * @file calculation.h
 * @brief Declares the expression evaluation engine for SmartCalc.
 */

#include <list>
#include <stack>
#include <stdexcept>
#include <string>

#include "cmath"
#include "lexeme.h"
#include "reverse_polish_notation.h"

namespace s21 {

/**
 * @brief Class for evaluating arithmetic and trigonometric expressions.
 * @details This class parses an input expression, converts it from infix form
 * to Reverse Polish Notation (RPN), and evaluates the resulting sequence. It
 * supports the variable `x`, and the final result can be retrieved with
 * GetResult.
 */
class Calculation {
 public:
  /**
   * @brief Default constructor.
   */
  Calculation() = default;

  /**
   * @brief Default destructor.
   */
  ~Calculation() = default;

  /**
   * @brief Returns the most recent calculation result.
   * @return Calculation result as a `long double`.
   */
  long double GetResult();

  /**
   * @brief Parses and evaluates an input expression.
   * @details The function iterates through the RPN sequence: numbers and the
   * variable `x` are pushed onto the stack, operators consume operands from
   * the stack, and each computed result is pushed back. When processing ends,
   * the value at the top of the stack is stored in `calculation_result_`.
   * @param expression Input expression to evaluate.
   * @param x_value Value used to substitute the variable `x`.
   */
  void Parse(std::string &expression, long double x_value);

 private:
  ReversePolishNotation rpn_;  ///< Converts input expressions to Reverse Polish Notation
  std::list<Lexeme> expression_;    ///< Reverse Polish sequence currently being evaluated
  long double calculation_result_;  ///< Stored calculation result

  /**
   * @brief Executes one calculation step for the current operator.
   * @param current_operator Reference to the lexeme representing the current
   * operator.
   * @param numbers Stack containing operands.
   * @return Result of the operation as a `long double`.
   */
  long double Calculate(s21::Lexeme &current_operator,
                        std::stack<long double> &numbers);

};  // class Calculation

}  // namespace s21

#endif  // CPP3_SMARTCALC_V2_0_SRC_MODEL_CALCULATION_H_
