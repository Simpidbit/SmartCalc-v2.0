#ifndef CPP3_SMARTCALC_V2_0_SRC_VIEW_FORMAT_STRING_H_
#define CPP3_SMARTCALC_V2_0_SRC_VIEW_FORMAT_STRING_H_

#include <QString>
#include <locale>
#include <string>

namespace s21 {

/**
 * @brief 将输入的 QString 转换为适合计算的 `std::string`。
 * @details 该类负责把界面中的表达式字符串转换为模型层可解析的基础字符串，
 * 同时会缩短较长的函数词法单元并完成解析前预处理。
 */
class FormatString {
 public:
  /**
   * @brief 带参构造函数。
   * @param input_str 待转换的 QString。
   */
  FormatString(QString const input_str) : q_str_(input_str), basic_str_("") {
    Convert();
  }

  /**
   * @brief 默认析构函数。
   */
  ~FormatString() = default;

  /**
   * @brief 获取转换后的字符串。
   * @return 返回适合计算的表达式字符串。
   */
  std::string GetString() { return basic_str_; }

 private:
  std::string basic_str_;  ///< 转换后的结果字符串。
  QString q_str_;          ///< 待转换的输入字符串。

  /**
   * @brief 将 QString 转换为适合计算的 `std::string`。
   */
  void Convert();

};

}

#endif
