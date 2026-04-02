#ifndef CPP3_SMARTCALC_V2_0_SRC_VIEW_GRAPH_H_
#define CPP3_SMARTCALC_V2_0_SRC_VIEW_GRAPH_H_

#include <qcustomplot.h>

#include <QDialog>

namespace Ui {
class Graph;
}

/**
 * @brief 用于显示和管理函数图像的类。
 * @details 该类基于 QCustomPlot 提供图像绘制与展示功能，支持设置表达式、
 * 清空图像以及获取当前 `x` 轴范围。
 */
class Graph : public QDialog {
  Q_OBJECT

 public:
  /**
   * @brief 构造绘图对话框。
   * @param parent 指向父级 QWidget 的指针。
   */
  Graph(QWidget *parent = nullptr);

  /**
   * @brief 默认析构函数。
   */
  ~Graph();

 public slots:

  /**
   * @brief 根据传入坐标构建图像。
   * @param coordinates 由横纵坐标向量组成的二元组。
   */
  void BuildPlot(
      std::pair<std::vector<double>, std::vector<double>> &coordinates);

  /**
   * @brief 设置待绘制的表达式。
   * @param expression 数学表达式的 QString 表示。
   */
  void SetExpression(QString expression);

  /**
   * @brief 清空图像数据并重置绘图区。
   */
  void Clear();

  /**
   * @brief 获取当前图像的 `x` 轴范围。
   * @return 返回表示 `x` 轴区间的二元组。
   */
  std::pair<double, double> GetXRange();

 private:
  Ui::Graph *ui_;  ///< 窗口界面实例。
};

#endif
