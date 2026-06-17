#ifndef CPP3_SMARTCALC_V2_0_SRC_EXPORT_GRAPH_EXPORT_H_
#define CPP3_SMARTCALC_V2_0_SRC_EXPORT_GRAPH_EXPORT_H_

#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QString>
#include <QTemporaryFile>

class QCustomPlot;

namespace s21 {

enum class ExportFormat {
  kUnsupported,
  kPng,
  kPdf,
};

enum class ExportErrorCode {
  kNone,
  kNoValidGraph,
  kInvalidPath,
  kUnsupportedFormat,
  kWriteFailed,
};

struct ExportInfo {
  QString path;
  ExportFormat format = ExportFormat::kUnsupported;
  int width = 0;
  int height = 0;
  int quality = -1;
  QString title;
};

struct ExportValidationResult {
  bool ok = false;
  QString normalized_path;
  ExportErrorCode error = ExportErrorCode::kNone;
  QString message;
};

class GraphExporter {
 public:
  bool HasValidGraph(const QCustomPlot *plot) const;
  bool ExportCurrentGraph(QCustomPlot *plot, const ExportInfo &info);

  ExportErrorCode LastError() const;
  QString LastErrorMessage() const;

  static ExportFormat FormatFromPath(const QString &path) {
    const QString suffix = QFileInfo(path).suffix().toLower();
    if (suffix == "png") {
      return ExportFormat::kPng;
    }
    if (suffix == "pdf") {
      return ExportFormat::kPdf;
    }
    return ExportFormat::kUnsupported;
  }

  static QString ExtensionForFormat(ExportFormat format) {
    switch (format) {
      case ExportFormat::kPng:
        return "png";
      case ExportFormat::kPdf:
        return "pdf";
      default:
        return {};
    }
  }

  static ExportValidationResult ValidateGraphState(bool has_valid_graph) {
    if (has_valid_graph) {
      return {true, {}, ExportErrorCode::kNone, {}};
    }
    return {false, {}, ExportErrorCode::kNoValidGraph,
            "No valid graph is available for export"};
  }

  static ExportValidationResult ValidateExportPath(const QString &path,
                                                   ExportFormat format) {
    if (format == ExportFormat::kUnsupported) {
      return {false, {}, ExportErrorCode::kUnsupportedFormat,
              "Unsupported export format"};
    }
    if (path.trimmed().isEmpty()) {
      return {false, {}, ExportErrorCode::kInvalidPath,
              "Export path is empty"};
    }

    QString normalized_path = path.trimmed();
    QFileInfo file_info(normalized_path);
    if (file_info.exists() && file_info.isDir()) {
      return {false, {}, ExportErrorCode::kInvalidPath,
              "Export path points to a directory"};
    }

    const QString required_extension = ExtensionForFormat(format);
    const QString suffix = file_info.suffix().toLower();
    if (suffix.isEmpty()) {
      normalized_path += "." + required_extension;
      file_info.setFile(normalized_path);
    } else if (suffix != required_extension) {
      return {false, {}, ExportErrorCode::kInvalidPath,
              "Export path extension does not match selected format"};
    }

    if (file_info.fileName().isEmpty()) {
      return {false, {}, ExportErrorCode::kInvalidPath,
              "Export file name is empty"};
    }

    const QDir directory = file_info.absoluteDir();
    if (!directory.exists()) {
      return {false, {}, ExportErrorCode::kInvalidPath,
              "Export directory does not exist"};
    }

    if (file_info.exists()) {
      if (!file_info.isFile() || !file_info.isWritable()) {
        return {false, {}, ExportErrorCode::kInvalidPath,
                "Export path is not writable"};
      }
      return {true, normalized_path, ExportErrorCode::kNone, {}};
    }

    QTemporaryFile probe_file(
        directory.absoluteFilePath(".smartcalc_export_write_test.XXXXXX"));
    if (!probe_file.open()) {
      return {false, {}, ExportErrorCode::kInvalidPath,
              "Export directory is not writable"};
    }
    probe_file.close();
    return {true, normalized_path, ExportErrorCode::kNone, {}};
  }

 private:
  void ResetError();
  void SetError(ExportErrorCode error, const QString &message);

  ExportErrorCode last_error_ = ExportErrorCode::kNone;
  QString last_error_message_;
};

}  // namespace s21

#endif  // CPP3_SMARTCALC_V2_0_SRC_EXPORT_GRAPH_EXPORT_H_
