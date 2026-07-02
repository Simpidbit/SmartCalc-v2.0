/**
 * @file graph.cc
 * @brief Implements the dialog used to display function plots.
 */

#include "graph.h"

#include <qcustomplot.h>

#include "ui_graph.h"
#include "view.h"

/**
 * @brief Constructs the graph dialog and initializes its UI.
 * @param parent Pointer to the parent widget.
 */
Graph::Graph(QWidget *parent) : QDialog(parent), ui_(new Ui::Graph) {
  ui_->setupUi(this);
  setWindowTitle("Graph");
}

/**
 * @brief Destroys the graph dialog and releases the generated UI.
 */
Graph::~Graph() { delete ui_; }

/**
 * @brief Draws the graph using the provided coordinate vectors.
 * @param coordinates Pair of vectors containing x-values and y-values.
 */
void Graph::BuildPlot(
    std::pair<std::vector<double>, std::vector<double>> &coordinates) {
  // Remove any previous curve so the widget always shows only the latest
  // expression.
  ui_->plot->clearGraphs();

  // Read the currently selected visible ranges from the dialog controls.
  double x_begin = ui_->spin_box_min_x->value();
  double x_end = ui_->spin_box_max_x->value();
  double y_begin = ui_->spin_box_min_y->value();
  double y_end = ui_->spin_box_max_y->value();

  // QCustomPlot consumes QVector data, so convert the sampled STL vectors
  // before attaching them to the graph.
  QVector<double> x_vector(coordinates.first.begin(), coordinates.first.end());
  QVector<double> y_vector(coordinates.second.begin(),
                           coordinates.second.end());

  // Apply the user-selected viewport before drawing the new data.
  ui_->plot->xAxis->setRange(x_begin, x_end);
  ui_->plot->yAxis->setRange(y_begin, y_end);

  // Create the plot curve, bind the new coordinates, and repaint the widget.
  ui_->plot->addGraph();
  ui_->plot->graph(0)->setData(x_vector, y_vector);
  ui_->plot->replot();
  ui_->plot->update();

  // Keep interactive navigation enabled after each redraw.
  ui_->plot->setInteraction(QCP::iRangeZoom, true);
  ui_->plot->setInteraction(QCP::iRangeDrag, true);
}

/**
 * @brief Updates the expression label shown in the graph dialog.
 * @param expression Expression text to display.
 */
void Graph::SetExpression(QString expression) {
  ui_->expression_to_plot->setText(expression);
}

/**
 * @brief Clears the expression label and removes plotted data.
 */
void Graph::Clear() {
  // Reset both the textual expression label and the visual plot contents.
  ui_->expression_to_plot->clear();
  ui_->plot->clearGraphs();
  ui_->plot->replot();
  ui_->plot->update();
}

/**
 * @brief Returns the current x-axis range selected in the dialog controls.
 * @return Pair containing the minimum and maximum x-values.
 */
std::pair<double, double> Graph::GetXRange() {
  std::pair<double, double> x_range;
  // Return the currently configured horizontal interval for function sampling.
  x_range.first = ui_->spin_box_min_x->value();
  x_range.second = ui_->spin_box_max_x->value();
  return x_range;
}
