#include "graph.h"

#include <qcustomplot.h>

#include <QFileDialog>
#include <QFileInfo>
#include <QMessageBox>

#include <cmath>

#include "ui_graph.h"
#include "view.h"

Graph::Graph(QWidget *parent) : QDialog(parent), ui_(new Ui::Graph) {
  ui_->setupUi(this);
  setWindowTitle("Graph");
  export_button_ = new QPushButton("Export", this);
  export_button_->setGeometry(130, 555, 110, 30);
  export_button_->setToolTip("export current graph as PNG or PDF");
  connect(export_button_, SIGNAL(clicked()), this, SLOT(ExportGraph()));
}

Graph::~Graph() { delete ui_; }

void Graph::BuildPlot(
    std::pair<std::vector<double>, std::vector<double>> &coordinates) {
  ui_->plot->clearGraphs();
  has_valid_curve_ = false;

  if (coordinates.first.empty() || coordinates.second.empty() ||
      coordinates.first.size() != coordinates.second.size()) {
    ui_->plot->replot();
    ui_->plot->update();
    return;
  }

  double x_begin = ui_->spin_box_min_x->value();
  double x_end = ui_->spin_box_max_x->value();
  double y_begin = ui_->spin_box_min_y->value();
  double y_end = ui_->spin_box_max_y->value();

  QVector<double> x_vector;
  QVector<double> y_vector;
  x_vector.reserve(static_cast<int>(coordinates.first.size()));
  y_vector.reserve(static_cast<int>(coordinates.second.size()));
  for (size_t i = 0; i < coordinates.first.size(); ++i) {
    if (std::isfinite(coordinates.first[i]) &&
        std::isfinite(coordinates.second[i])) {
      x_vector.push_back(coordinates.first[i]);
      y_vector.push_back(coordinates.second[i]);
    }
  }

  if (x_vector.isEmpty()) {
    ui_->plot->replot();
    ui_->plot->update();
    return;
  }

  ui_->plot->xAxis->setRange(x_begin, x_end);
  ui_->plot->yAxis->setRange(y_begin, y_end);

  ui_->plot->addGraph();
  ui_->plot->graph(0)->setData(x_vector, y_vector);
  ui_->plot->replot();
  ui_->plot->update();
  has_valid_curve_ = true;

  ui_->plot->setInteraction(QCP::iRangeZoom, true);
  ui_->plot->setInteraction(QCP::iRangeDrag, true);
}

void Graph::SetExpression(QString expression) {
  ui_->expression_to_plot->setText(expression);
}

void Graph::Clear() {
  ui_->expression_to_plot->clear();
  ui_->plot->clearGraphs();
  ui_->plot->replot();
  ui_->plot->update();
  has_valid_curve_ = false;
}

void Graph::ExportGraph() {
  if (!HasValidGraph()) {
    QMessageBox::warning(this, "Export graph", "No valid graph to export");
    return;
  }

  QString selected_filter;
  const QString path = QFileDialog::getSaveFileName(
      this, "Export graph", QString(),
      "PNG image (*.png);;PDF document (*.pdf)", &selected_filter);
  if (path.trimmed().isEmpty()) {
    return;
  }

  s21::ExportFormat format = s21::GraphExporter::FormatFromPath(path);
  if (format == s21::ExportFormat::kUnsupported) {
    if (selected_filter.contains("pdf", Qt::CaseInsensitive)) {
      format = s21::ExportFormat::kPdf;
    } else if (selected_filter.contains("png", Qt::CaseInsensitive)) {
      format = s21::ExportFormat::kPng;
    }
  }

  const s21::ExportValidationResult validation =
      s21::GraphExporter::ValidateExportPath(path, format);
  if (!validation.ok) {
    QMessageBox::warning(this, "Export graph",
                         validation.message.isEmpty() ? "Export failed"
                                                      : validation.message);
    return;
  }

  s21::ExportInfo info;
  info.path = validation.normalized_path;
  info.format = format;
  info.width = ui_->plot->width();
  info.height = ui_->plot->height();
  info.title = ui_->expression_to_plot->text();

  if (!exporter_.ExportCurrentGraph(ui_->plot, info)) {
    QMessageBox::warning(this, "Export graph",
                         exporter_.LastErrorMessage().isEmpty()
                             ? "Export failed"
                             : exporter_.LastErrorMessage());
    return;
  }

  QMessageBox::information(this, "Export graph", "Graph exported successfully");
}

bool Graph::HasValidGraph() const {
  return has_valid_curve_ && exporter_.HasValidGraph(ui_->plot);
}

std::pair<double, double> Graph::GetXRange() {
  std::pair<double, double> x_range;
  x_range.first = ui_->spin_box_min_x->value();
  x_range.second = ui_->spin_box_max_x->value();
  return x_range;
}
