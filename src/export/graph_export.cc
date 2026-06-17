#include "graph_export.h"

#include <qcustomplot.h>

#include <QCoreApplication>

namespace s21 {

bool GraphExporter::HasValidGraph(const QCustomPlot *plot) const {
  if (plot == nullptr || plot->graphCount() == 0) {
    return false;
  }

  for (int i = 0; i < plot->graphCount(); ++i) {
    const QCPGraph *graph = plot->graph(i);
    if (graph != nullptr && graph->data() != nullptr &&
        !graph->data()->isEmpty()) {
      return true;
    }
  }
  return false;
}

bool GraphExporter::ExportCurrentGraph(QCustomPlot *plot,
                                       const ExportInfo &info) {
  ResetError();
  if (!HasValidGraph(plot)) {
    SetError(ExportErrorCode::kNoValidGraph,
             "No valid graph is available for export");
    return false;
  }

  const ExportValidationResult validation =
      ValidateExportPath(info.path, info.format);
  if (!validation.ok) {
    SetError(validation.error, validation.message);
    return false;
  }

  const QString temporary_path =
      validation.normalized_path +
      QString(".tmp.%1").arg(QCoreApplication::applicationPid());
  QFile::remove(temporary_path);

  bool saved = false;
  if (info.format == ExportFormat::kPng) {
    saved = plot->savePng(temporary_path, info.width, info.height, 1.0,
                          info.quality);
  } else if (info.format == ExportFormat::kPdf) {
    saved = plot->savePdf(temporary_path, info.width, info.height,
                          QCP::epAllowCosmetic, "SmartCalc-v2.0",
                          info.title);
  } else {
    SetError(ExportErrorCode::kUnsupportedFormat, "Unsupported export format");
    return false;
  }

  if (!saved || !QFileInfo::exists(temporary_path)) {
    QFile::remove(temporary_path);
    SetError(ExportErrorCode::kWriteFailed, "Graph export backend failed");
    return false;
  }

  const bool target_exists = QFileInfo::exists(validation.normalized_path);
  const QString backup_path =
      validation.normalized_path +
      QString(".backup.%1").arg(QCoreApplication::applicationPid());
  if (target_exists) {
    QFile::remove(backup_path);
    if (!QFile::rename(validation.normalized_path, backup_path)) {
      QFile::remove(temporary_path);
      SetError(ExportErrorCode::kWriteFailed,
               "Unable to prepare existing export file for replacement");
      return false;
    }
  }

  if (!QFile::rename(temporary_path, validation.normalized_path)) {
    QFile::remove(temporary_path);
    if (target_exists) {
      QFile::rename(backup_path, validation.normalized_path);
    }
    SetError(ExportErrorCode::kWriteFailed,
             "Unable to move temporary export file into place");
    return false;
  }

  if (target_exists) {
    QFile::remove(backup_path);
  }

  return true;
}

ExportErrorCode GraphExporter::LastError() const { return last_error_; }

QString GraphExporter::LastErrorMessage() const { return last_error_message_; }

void GraphExporter::ResetError() {
  last_error_ = ExportErrorCode::kNone;
  last_error_message_.clear();
}

void GraphExporter::SetError(ExportErrorCode error, const QString &message) {
  last_error_ = error;
  last_error_message_ = message;
}

}  // namespace s21
