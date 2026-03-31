#ifndef CPP3_SMARTCALC_V2_0_SRC_MODEL_PLOT_GRAPH_H_
#define CPP3_SMARTCALC_V2_0_SRC_MODEL_PLOT_GRAPH_H_

#include <string>
#include <vector>

#include "calculation.h"

namespace s21 {

/**
 * @brief 计算绘图坐标数据的类。
 * @details 该类接收数学表达式及 x 轴取值范围，
 * 按固定步长对 x 进行采样，并调用计算模块得到对应 y 值，
 * 最终返回可直接用于绘图的坐标向量。
 */
class PlotGraph {
 public:
  /**
   * @brief 默认构造函数。
   */
  PlotGraph() = default;

  /**
   * @brief 默认析构函数。
   */
  ~PlotGraph() = default;

  /**
   * @brief 在指定 x 范围内计算表达式对应的 x、y 坐标序列。
   * @param expression 数学表达式字符串。
   * @param x_range x 轴范围，`first` 为最小值，`second` 为最大值。
   * @return `std::pair<std::vector<double>, std::vector<double>>`，
   * 其中第一个向量为 x 坐标，第二个向量为对应 y 坐标。
   */
  std::pair<std::vector<double>, std::vector<double>> Calculate(
      std::string &expression, std::pair<double, double> x_range) {
    std::vector<double> x_vector;
    std::vector<double> y_vector;

    x_min_ = x_range.first;
    x_max_ = x_range.second;

    double y_coordinate = 0;
    double x_coordinate = 0;
    double step = 0.1;

    for (double i = x_min_; i < x_max_; i += step) {
      calc_expression_.Parse(expression, i);
      y_coordinate = static_cast<double>(calc_expression_.GetResult());
      x_coordinate = i;
      x_vector.push_back(x_coordinate);
      y_vector.push_back(y_coordinate);
    }

    return std::pair<std::vector<double>, std::vector<double>>{x_vector,
                                                               y_vector};
  }

 private:
  long double x_min_;  ///< x 轴最小值
  long double x_max_;  ///< x 轴最大值
  std::pair<std::vector<double>, std::vector<double>>
      coordinates_;  ///< 绘图坐标数据（当前版本中未直接使用）
  Calculation
      calc_expression_;  ///< 计算器实例，用于按采样点求表达式值

};  // class PlotGraph

}  // namespace s21

#endif  // CPP3_SMARTCALC_V2_0_SRC_MODEL_PLOT_GRAPH_H_
