#ifndef CPP3_SMARTCALC_V2_0_SRC_MODEL_LEXEME_H_
#define CPP3_SMARTCALC_V2_0_SRC_MODEL_LEXEME_H_

#include <string>

namespace s21 {

/**
 * @brief 运算符优先级枚举。
 * @details 定义表达式中不同运算符（及括号语义）对应的优先级等级，
 * 枚举值越靠后表示优先级越高。
 */
enum class Priority {
  kPriority_0,  ///< 数字与括号
  kPriority_1,  ///< 加减运算（+, -）
  kPriority_2,  ///< 乘除与取模（*, /, %）
  kPriority_3,  ///< 幂与平方根（pow, sqrt）
  kPriority_4,  ///< 三角/反三角/对数等函数
};

/**
 * @brief 词法单元类型枚举。
 */
enum class LexemeType { kNumber, kOperator };

/**
 * @brief 表示表达式中的单个词法单元（Lexeme）。
 * @details 该类封装词法单元的核心属性：字符串值、优先级和类型，
 * 用于表达式解析与求值流程中的数据传递。
 */
class Lexeme {
 public:
  std::string value;  ///< 词法单元的字符串表示
  Priority priority;  ///< 词法单元的优先级
  LexemeType type;    ///< 词法单元的类型

  /**
   * @brief 默认构造函数。
   */
  Lexeme()
      : value(""), priority(Priority::kPriority_0), type(LexemeType::kNumber){};

  /**
   * @brief 带参构造函数。
   * @param new_value 词法单元字符值。
   * @param elem_priority 词法单元优先级。
   * @param elem_type 词法单元类型。
   */
  Lexeme(char new_value, Priority elem_priority, LexemeType elem_type)
      : priority(elem_priority), type(elem_type) {
    value.push_back(new_value);
  };

  /**
   * @brief 拷贝构造函数。
   * @param other 用于拷贝的源词法单元。
   */
  Lexeme(const Lexeme &other) { *this = other; }

  /**
   * @brief 默认析构函数。
   */
  ~Lexeme() = default;

  /**
   * @brief 赋值运算符重载。
   * @param other 用于赋值的源词法单元。
   * @return 赋值后的当前对象引用。
   */
  Lexeme &operator=(const Lexeme &other) = default;
};  // class Lexeme

}  // namespace s21

#endif  // CPP3_SMARTCALC_V2_0_SRC_MODEL_LEXEME_H_
