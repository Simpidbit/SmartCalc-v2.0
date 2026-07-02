#ifndef CPP3_SMARTCALC_V2_0_SRC_MODEL_LEXEME_H_
#define CPP3_SMARTCALC_V2_0_SRC_MODEL_LEXEME_H_

/**
 * @file lexeme.h
 * @brief Defines token types used while parsing and evaluating expressions.
 */

#include <string>

namespace s21 {

/**
 * @brief Enumeration of operator precedence levels.
 * @details Defines the precedence assigned to different operators and
 * parenthesis semantics within an expression. Later enum values represent
 * higher precedence.
 */
enum class Priority {
  kPriority_0,  ///< Numbers and parentheses
  kPriority_1,  ///< Addition and subtraction (`+`, `-`)
  kPriority_2,  ///< Multiplication, division, and modulo (`*`, `/`, `%`)
  kPriority_3,  ///< Power and square root (`pow`, `sqrt`)
  kPriority_4,  ///< Trigonometric, inverse trigonometric, and logarithmic functions
};

/**
 * @brief Enumeration of lexeme types.
 */
enum class LexemeType { kNumber, kOperator };

/**
 * @brief Represents a single lexeme in an expression.
 * @details This class stores the essential properties of a lexeme: its string
 * value, precedence, and type. It is used to pass token data through parsing
 * and evaluation stages.
 */
class Lexeme {
 public:
  std::string value;  ///< String representation of the lexeme
  Priority priority;  ///< Precedence level of the lexeme
  LexemeType type;    ///< Type of the lexeme

  /**
   * @brief Default constructor.
   */
  Lexeme()
      : value(""), priority(Priority::kPriority_0), type(LexemeType::kNumber){};

  /**
   * @brief Parameterized constructor.
   * @param new_value Initial character value of the lexeme.
   * @param elem_priority Precedence level of the lexeme.
   * @param elem_type Type of the lexeme.
   */
  Lexeme(char new_value, Priority elem_priority, LexemeType elem_type)
      : priority(elem_priority), type(elem_type) {
    value.push_back(new_value);
  };

  /**
   * @brief Copy constructor.
   * @param other Source lexeme to copy from.
   */
  Lexeme(const Lexeme &other) { *this = other; }

  /**
   * @brief Default destructor.
   */
  ~Lexeme() = default;

  /**
   * @brief Assignment operator overload.
   * @param other Source lexeme to assign from.
   * @return Reference to the current object after assignment.
   */
  Lexeme &operator=(const Lexeme &other) = default;
};  // class Lexeme

}  // namespace s21

#endif  // CPP3_SMARTCALC_V2_0_SRC_MODEL_LEXEME_H_
