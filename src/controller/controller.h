#ifndef CPP3_SMARTCALC_V2_0_SRC_CONTROLLER_CONTROLLER_H_
#define CPP3_SMARTCALC_V2_0_SRC_CONTROLLER_CONTROLLER_H_

/**
 * @file controller.h
 * @brief Declares the controller that coordinates SmartCalc model and view.
 */

#include "../model/calculation.h"
#include "../model/plot_graph.h"

namespace s21 {

/**
 * @brief Controller class that manages interactions between the model and view.
 * @details This class acts as an intermediary between the calculation model
 * and the graphical interface. It receives requests from the view, asks the
 * model to evaluate expressions, and returns coordinate data for plotting.
 */
class Controller {
 public:
  /**
   * @brief Constructs a Controller with the specified Calculation model.
   * @param calculation Pointer to the Calculation object used for expression
   * parsing and result evaluation.
   */
  Controller(Calculation *calculation) : model_(calculation){};

  /**
   * @brief Default destructor.
   */
  ~Controller() = default;

  /**
   * @brief Evaluates a mathematical expression.
   * @param expression Mathematical expression string to evaluate.
   * @param x_value Numeric value used to substitute the variable `x`.
   * @return Calculation result as a `long double`.
   */
  long double Calculate(std::string expression, double x_value) {
    model_->Parse(expression, x_value);
    return model_->GetResult();
  }

  /**
   * @brief Computes the x and y coordinate sequences needed to plot an
   * expression.
   * @param expression Mathematical expression string to plot.
   * @param x_range X-axis range, where `first` is the minimum value and
   * `second` is the maximum value.
   * @return Pair of vectors where the first contains x-coordinates and the
   * second contains the corresponding y-coordinates.
   */
  std::pair<std::vector<double>, std::vector<double>> CalculateGraphCoordinates(
      std::string &expression, std::pair<long double, long double> x_range) {
    return plot_.Calculate(expression, x_range);
  }

 private:
  Calculation
      *model_;  ///< Pointer to the Calculation model used for expression evaluation
  PlotGraph plot_;  ///< PlotGraph instance used to generate plotting coordinates

};  // class controller

}  // namespace s21

#endif  // CPP3_SMARTCALC_V2_0_SRC_CONTROLLER_CONTROLLER_H_
