#ifndef CPP3_SMARTCALC_V2_0_SRC_CONTROLLER_CONTROLLER_H_
#define CPP3_SMARTCALC_V2_0_SRC_CONTROLLER_CONTROLLER_H_

#include "../model/calculation.h"
#include "../model/plot_graph.h"

namespace s21 {

/**
 * @brief 管理 Model 与 View 之间交互的控制器类。
 * @details 该类作为计算模型与图形界面之间的中介层，
 * 接收视图层请求，调用模型层完成表达式计算，
 * 并返回绘图所需的坐标数据。
 */
class Controller {
 public:
  /**
   * @brief 使用指定的 Calculation 模型构造 Controller。
   * @param calculation 指向 Calculation 对象的指针，
   * 用于表达式解析与结果计算。
   */
  Controller(Calculation *calculation) : model_(calculation){};

  /**
   * @brief 默认析构函数。
   */
  ~Controller() = default;

  /**
   * @brief 计算数学表达式结果。
   * @param expression 待计算的数学表达式字符串。
   * @param x_value 计算时用于替换变量 x 的数值。
   * @return long double 类型的计算结果。
   */
  long double Calculate(std::string expression, double x_value) {
    model_->Parse(expression, x_value);
    return model_->GetResult();
  }

  /**
   * @brief 根据表达式计算绘图所需的 x、y 坐标序列。
   * @param expression 待绘制的数学表达式字符串。
   * @param x_range x 轴范围，`first` 为最小值，`second` 为最大值。
   * @return `std::pair<std::vector<double>, std::vector<double>>`，
   * 其中第一个向量为 x 坐标，第二个向量为对应 y 坐标。
   */
  std::pair<std::vector<double>, std::vector<double>> CalculateGraphCoordinates(
      std::string &expression, std::pair<long double, long double> x_range) {
    return plot_.Calculate(expression, x_range);
  }

 private:
  Calculation
      *model_;  ///< 指向 Calculation 模型对象的指针，用于表达式求值
  PlotGraph plot_;  ///< PlotGraph 实例，用于生成绘图坐标数据

};  // class controller

}  // namespace s21

#endif  // CPP3_SMARTCALC_V2_0_SRC_CONTROLLER_CONTROLLER_H_
