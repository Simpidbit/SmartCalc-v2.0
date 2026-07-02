#ifndef CPP3_SMARTCALC_V2_0_SRC_MODEL_REVERSE_POLISH_NOTATION_H_
#define CPP3_SMARTCALC_V2_0_SRC_MODEL_REVERSE_POLISH_NOTATION_H_

/**
 * @file reverse_polish_notation.h
 * @brief Declares conversion from infix expressions to Reverse Polish Notation.
 */

#include <list>
#include <stack>
#include <string>

#include "lexeme.h"

namespace s21 {

/**
 * @brief Class for converting infix expressions to Reverse Polish Notation
 * (RPN).
 * @details This class accepts a string expression, splits it into lexemes,
 * and converts it to RPN stored in a `std::list`. The conversion follows
 * Dijkstra's Shunting-yard algorithm, using a stack to manage operators that
 * have not yet been emitted while scanning tokens in order.
 */
class ReversePolishNotation {
 public:
  /**
   * @brief Default constructor.
   */
  ReversePolishNotation() = default;
  /**
   * @brief Default destructor.
   */
  ~ReversePolishNotation() = default;

  /**
   * @brief Returns the current Reverse Polish lexeme sequence.
   * @return RPN lexeme list as `std::list<Lexeme>`.
   */
  std::list<Lexeme> GetRpnList() { return rpn_list_; };

  /**
   * @brief Converts an input expression string to an RPN sequence.
   * @param str Infix expression string to convert.
   */
  void Convert(std::string &str);

 private:
  std::list<Lexeme> rpn_list_;  ///< Lexeme list after conversion to RPN

  /**
   * @brief Parses a numeric lexeme and appends it to the RPN list.
   * @param it Iterator pointing to the beginning of the number.
   * @return Number of characters by which the outer iterator should advance.
   */
  size_t ParseNumber(std::string::iterator it);

  /**
   * @brief Parses the current operator and updates the operator stack based on
   * precedence rules.
   * @param operators_stack Operator stack.
   * @param it Iterator pointing to the current character.
   * @param str Input expression string.
   */
  void ParseOperator(std::stack<Lexeme> &operators_stack,
                     std::string::iterator it, std::string &str);

  /**
   * @brief Handles a closing parenthesis by popping operators until an opening
   * parenthesis is reached.
   * @param operators_stack Operator stack.
   */
  void CloseParenth(std::stack<Lexeme> &operators_stack);

  /**
   * @brief Pushes an opening parenthesis onto the operator stack.
   * @param operators_stack Operator stack.
   */
  void PushOpenParenth(std::stack<Lexeme> &operators_stack);

  /**
   * @brief Returns the precedence of the current symbol.
   * @param it Iterator pointing to the current symbol.
   * @return Corresponding `Priority` value for the symbol.
   */
  Priority GetPriority(std::string::iterator it);

  /**
   * @brief Determines whether `+` or `-` is being used as a unary sign.
   * @param it Iterator pointing to the current symbol.
   * @param str Input expression string.
   * @return `true` if the symbol is unary, otherwise `false`.
   */
  bool IsUnary(std::string::iterator it, std::string &str);

};  // class ReversePolishNotation

}  // namespace s21

#endif  // CPP3_SMARTCALC_V2_0_SRC_MODEL_REVERSE_POLISH_NOTATION_H_
