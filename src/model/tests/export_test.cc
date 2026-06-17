#include "../../export/graph_export.h"

#include <gtest/gtest.h>

#include <QFile>
#include <QTemporaryDir>

TEST(graph_exporter, rejects_missing_valid_graph) {
  const s21::ExportValidationResult validation =
      s21::GraphExporter::ValidateGraphState(false);

  EXPECT_FALSE(validation.ok);
  EXPECT_EQ(validation.error, s21::ExportErrorCode::kNoValidGraph);
}

TEST(graph_exporter, accepts_valid_graph_state) {
  const s21::ExportValidationResult validation =
      s21::GraphExporter::ValidateGraphState(true);

  EXPECT_TRUE(validation.ok);
  EXPECT_EQ(validation.error, s21::ExportErrorCode::kNone);
}

TEST(graph_exporter, validates_supported_formats_from_extension) {
  EXPECT_EQ(s21::GraphExporter::FormatFromPath("plot.png"),
            s21::ExportFormat::kPng);
  EXPECT_EQ(s21::GraphExporter::FormatFromPath("plot.PDF"),
            s21::ExportFormat::kPdf);
  EXPECT_EQ(s21::GraphExporter::FormatFromPath("plot.svg"),
            s21::ExportFormat::kUnsupported);
}

TEST(graph_exporter, rejects_empty_path_and_unsupported_format) {
  EXPECT_FALSE(s21::GraphExporter::ValidateExportPath(
                   QString(), s21::ExportFormat::kPng)
                   .ok);
  EXPECT_EQ(s21::GraphExporter::ValidateExportPath(
                "plot.png", s21::ExportFormat::kUnsupported)
                .error,
            s21::ExportErrorCode::kUnsupportedFormat);
}

TEST(graph_exporter, appends_missing_extension) {
  QTemporaryDir directory;
  ASSERT_TRUE(directory.isValid());

  const s21::ExportValidationResult validation =
      s21::GraphExporter::ValidateExportPath(directory.filePath("plot"),
                                             s21::ExportFormat::kPng);

  EXPECT_TRUE(validation.ok);
  EXPECT_TRUE(validation.normalized_path.endsWith(".png"));
}

TEST(graph_exporter, rejects_extension_format_mismatch) {
  QTemporaryDir directory;
  ASSERT_TRUE(directory.isValid());

  const s21::ExportValidationResult validation =
      s21::GraphExporter::ValidateExportPath(directory.filePath("plot.pdf"),
                                             s21::ExportFormat::kPng);

  EXPECT_FALSE(validation.ok);
  EXPECT_EQ(validation.error, s21::ExportErrorCode::kInvalidPath);
}

TEST(graph_exporter, rejects_missing_or_non_file_target_directory) {
  QTemporaryDir directory;
  ASSERT_TRUE(directory.isValid());

  EXPECT_FALSE(s21::GraphExporter::ValidateExportPath(
                   directory.filePath("missing/plot.png"),
                   s21::ExportFormat::kPng)
                   .ok);
  EXPECT_FALSE(s21::GraphExporter::ValidateExportPath(
                   directory.path(), s21::ExportFormat::kPng)
                   .ok);
}

TEST(graph_exporter, accepts_writable_file_path) {
  QTemporaryDir directory;
  ASSERT_TRUE(directory.isValid());

  const QString export_path = directory.filePath("plot.pdf");
  const s21::ExportValidationResult validation =
      s21::GraphExporter::ValidateExportPath(export_path,
                                             s21::ExportFormat::kPdf);

  EXPECT_TRUE(validation.ok);
  EXPECT_EQ(validation.normalized_path, export_path);
}
