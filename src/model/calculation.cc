#include "calculation.h"

using namespace s21;

namespace {

constexpr long double kEulerNumber = 2.718281828459045235360287471352662L;

}  // namespace

void Calculation::Parse(std::string &expression, long double x_value) {
  rpn_.Convert(expression);

  expression_ = rpn_.GetRpnList();
  long double operation_result = 0;
  std::stack<long double> numbers;
  Lexeme lexeme;
  long double number = 0;
  while (!expression_.empty()) {
    lexeme = expression_.front();
    expression_.pop_front();
    if (lexeme.type == LexemeType::kNumber) {
      if (lexeme.value == "x") {
        numbers.push(x_value);
      } else if (lexeme.value == "e" || lexeme.value == "E") {
        numbers.push(kEulerNumber);
      } else {
        try {
          size_t parsed_length = 0;
          number = std::stold(lexeme.value, &parsed_length);
          if (parsed_length != lexeme.value.length()) {
            calculation_result_ = NAN;
            return;
          }
        } catch (const std::invalid_argument &e) {
          calculation_result_ = NAN;
          return;
        } catch (const std::out_of_range &e) {
          calculation_result_ = NAN;
          return;
        }
        numbers.push(number);
      }
    }
    if (lexeme.type == LexemeType::kOperator) {
      operation_result = Calculate(lexeme, numbers);
      numbers.push(operation_result);
    }
  }
  calculation_result_ = numbers.top();
  numbers.pop();
}

long double Calculation::Calculate(s21::Lexeme &current_operator,
                                   std::stack<long double> &numbers) {
  long double result = 0;
  long double a = numbers.top();
  long double b = 0;
  numbers.pop();

  /// 需要两个操作数的运算
  if (current_operator.priority == Priority::kPriority_1 ||
      current_operator.priority == Priority::kPriority_2) {
    b = numbers.top();
    numbers.pop();
    if (current_operator.value == "+") {
      result = b + a;
    } else if (current_operator.value == "-") {
      result = b - a;
    } else if (current_operator.value == "*") {
      result = b * a;
    } else if (current_operator.value == "/") {
      result = b / a;
    } else if (current_operator.value == "%") {
      /// 取模运算
      if (a == 0.0) {
        result = NAN;
      } else {
        result = static_cast<int>(b) % static_cast<int>(a);
      }
    }

    /// 需要一个操作数的运算（幂运算除外）
  } else {
    if (current_operator.value == "r") {
      /// 平方根
      result = sqrt(a);
    } else if (current_operator.value == "^") {
      /// 幂运算
      b = numbers.top();
      numbers.pop();
      result = pow(b, a);
    } else if (current_operator.value == "s") {
      /// 正弦
      result = sinl(a);
    } else if (current_operator.value == "c") {
      /// 余弦
      result = cosl(a);
    } else if (current_operator.value == "t") {
      /// 正切
      result = tanl(a);
    } else if (current_operator.value == "S") {
      /// 反正弦
      result = asinl(a);
    } else if (current_operator.value == "C") {
      /// 反余弦
      result = acosl(a);
    } else if (current_operator.value == "T") {
      /// 反正切
      result = atanl(a);
    } else if (current_operator.value == "l") {
      /// 自然对数
      result = logl(a);
    } else if (current_operator.value == "L") {
      /// 常用对数
      result = log10l(a);
    }
  }
  return result;
}

long double Calculation::GetResult() { return calculation_result_; }
