#ifndef CPP3_SMARTCALC_V2_0_SRC_MODEL_PLOT_GRAPH_H_
#define CPP3_SMARTCALC_V2_0_SRC_MODEL_PLOT_GRAPH_H_

/**
 * @file plot_graph.h
 * @brief Declares helpers for generating plot coordinates from an expression.
 */

#include <string>
#include <vector>

#include "calculation.h"

namespace s21 {

/**
 * @brief Class for generating coordinate data used for plotting.
 * @details This class accepts a mathematical expression and an x-axis range,
 * samples x values at a fixed step, evaluates the expression for each sample,
 * and returns coordinate vectors that can be passed directly to the plotting
 * widget.
 */
class PlotGraph {
 public:
  /**
   * @brief Default constructor.
   */
  PlotGraph() = default;

  /**
   * @brief Default destructor.
   */
  ~PlotGraph() = default;

  /**
   * @brief Computes x and y coordinate sequences for an expression over a
   * specified x-range.
   * @details The function samples from `x_range.first` up to
   * `x_range.second` using a fixed step of `0.1`, reevaluates the expression
   * for each sample via Calculation, and stores matching x and y values at the
   * same indices in the returned vectors.
   * @param expression Mathematical expression string.
   * @param x_range X-axis range, where `first` is the minimum value and
   * `second` is the maximum value.
   * @return Pair of vectors where the first stores x-coordinates and the
   * second stores the corresponding y-coordinates.
   */
  std::pair<std::vector<double>, std::vector<double>> Calculate(
      std::string &expression, std::pair<double, double> x_range) {
    std::vector<double> x_vector;
    std::vector<double> y_vector;

    // Store the requested bounds so the sampling loop can iterate over a
    // stable copy of the current plotting interval.
    x_min_ = x_range.first;
    x_max_ = x_range.second;

    double y_coordinate = 0;
    double x_coordinate = 0;
    double step = 0.1;

    // Sample the function at a fixed step and keep x/y values aligned by index
    // so the plotting widget can consume both vectors directly.
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
  long double x_min_;  ///< Minimum x-axis value
  long double x_max_;  ///< Maximum x-axis value
  std::pair<std::vector<double>, std::vector<double>>
      coordinates_;  ///< Plot coordinate data (not used directly in the current version)
  Calculation
      calc_expression_;  ///< Calculator instance used to evaluate expression samples

};  // class PlotGraph

}  // namespace s21

#endif  // CPP3_SMARTCALC_V2_0_SRC_MODEL_PLOT_GRAPH_H_
