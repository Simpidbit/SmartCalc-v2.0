#ifndef CPP3_SMARTCALC_V2_0_SRC_CONTROLLER_CONTROLLER_H_
#define CPP3_SMARTCALC_V2_0_SRC_CONTROLLER_CONTROLLER_H_

#include "../model/calculation.h"
#include "../model/plot_graph.h"

namespace s21 {

/**
 * @brief 管理模型层与视图层之间交互的控制器类。
 * @details 该类作为计算模型与图形界面之间的中介，负责处理用户输入、
 * 执行计算，并获取绘制函数图像所需的坐标数据。
 */
class Controller {
 public:
  /**
   * @brief 使用指定的 Calculation 模型构造控制器。
   * @param calculation 指向 Calculation 对象的指针，用于解析表达式并完成计算。
   */
  Controller(Calculation *calculation) : model_(calculation){};

  /**
   * @brief 默认析构函数。
   */
  ~Controller() = default;

  /**
   * @brief 计算数学表达式的结果。
   * @param expression 待求值的数学表达式。
   * @param x_value 计算过程中替换到表达式中的 `x` 值。
   * @return 返回计算得到的结果。
   */
  long double Calculate(std::string expression, double x_value) {
    model_->Parse(expression, x_value);
    return model_->GetResult();
  }

  /**
   * @brief 根据表达式计算绘图所需的横纵坐标。
   * @param expression 待求值的数学表达式。
   * @param x_range `x` 轴取值范围。
   * @return 返回由横坐标和对应纵坐标组成的向量对。
   */
  std::pair<std::vector<double>, std::vector<double>> CalculateGraphCoordinates(
      std::string &expression, std::pair<long double, long double> x_range) {
    return plot_.Calculate(expression, x_range);
  }

 private:
  Calculation *model_;  ///< 指向用于表达式求值的 Calculation 模型。
  PlotGraph plot_;      ///< 用于生成绘图坐标的 PlotGraph 实例。

};

}

#endif
