#ifndef CPP3_SMARTCALC_V2_0_SRC_MODEL_LEXEME_H_
#define CPP3_SMARTCALC_V2_0_SRC_MODEL_LEXEME_H_

#include <string>

namespace s21 {

/**
 * @brief 表示运算符优先级的枚举类型。
 * @details 该枚举定义了表达式中不同运算符及括号的优先级，值越大表示优先级越高。
 */
enum class Priority {
  kPriority_0,  ///< 数字与括号。
  kPriority_1,  ///< 加法与减法（`+`、`-`）。
  kPriority_2,  ///< 乘法、除法与取模（`*`、`/`、`%`）。
  kPriority_3,  ///< 幂运算与平方根（`pow`、`sqrt`）。
  kPriority_4,  ///< 三角函数（`cos`、`sin`、`tan`、`acos`、`asin`、`atan`）。
};

/**
 * @brief 表示词法单元类型的枚举。
 */
enum class LexemeType { kNumber, kOperator };

/**
 * @brief 表示表达式中单个词法单元的类。
 * @details 该类封装了词法单元的值、优先级与类型，用于表达式的解析与求值。
 */
class Lexeme {
 public:
  std::string value;  ///< 词法单元的字符串表示。
  Priority priority;  ///< 词法单元的优先级。
  LexemeType type;    ///< 词法单元的类型。

  /**
   * @brief 默认构造函数。
   */
  Lexeme()
      : value(""), priority(Priority::kPriority_0), type(LexemeType::kNumber){};

  /**
   * @brief 带参构造函数。
   * @param new_value 词法单元的字符值。
   * @param elem_priority 词法单元的优先级。
   * @param elem_type 词法单元的类型。
   */
  Lexeme(char new_value, Priority elem_priority, LexemeType elem_type)
      : priority(elem_priority), type(elem_type) {
    value.push_back(new_value);
  };

  /**
   * @brief 拷贝构造函数。
   * @param other 用于拷贝的另一个词法单元。
   */
  Lexeme(const Lexeme &other) { *this = other; }

  /**
   * @brief 默认析构函数。
   */
  ~Lexeme() = default;

  /**
   * @brief 赋值运算符。
   * @param other 用于赋值的另一个词法单元。
   * @return 返回赋值后的当前对象引用。
   */
  Lexeme &operator=(const Lexeme &other) = default;
};

}

#endif
