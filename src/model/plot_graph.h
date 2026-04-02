#ifndef CPP3_SMARTCALC_V2_0_SRC_MODEL_PLOT_GRAPH_H_
#define CPP3_SMARTCALC_V2_0_SRC_MODEL_PLOT_GRAPH_H_

#include <string>
#include <vector>

#include "calculation.h"

namespace s21 {

/**
 * @brief 计算并保存绘图坐标的类。
 * @details 该类接收数学表达式和 `x` 轴范围，计算对应的 `y` 值，
 * 并将生成的坐标保存到向量中，供后续绘图使用。
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
   * @brief 根据给定表达式和区间计算横纵坐标。
   * @param expression 数学表达式字符串。
   * @param x_range `x` 轴取值范围。
   * @return 返回横坐标向量及其对应纵坐标向量组成的二元组。
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
  long double x_min_;  ///< `x` 轴最小值。
  long double x_max_;  ///< `x` 轴最大值。
  std::pair<std::vector<double>, std::vector<double>>
      coordinates_;  ///< 用于绘图的坐标集合。
  Calculation calc_expression_;  ///< 用于计算表达式值的 Calculation 实例。

};

}

#endif
