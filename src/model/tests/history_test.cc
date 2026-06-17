#include "../../history/history_manager.h"

#include <gtest/gtest.h>

#include <QFile>
#include <QTemporaryDir>
#include <QTextStream>

namespace {

s21::HistoryRecord MakeRecord(int index) {
  s21::HistoryRecord record;
  record.record_id = QString::number(index);
  record.expression = QString::number(index) + "+1";
  record.display_expression = record.expression;
  record.x_value = QString::number(index);
  record.result = QString::number(index + 1);
  record.created_at =
      QString("2026-06-18T00:00:%1.000Z").arg(index, 2, 10, QChar('0'));
  return record;
}

}  // namespace

TEST(history_manager, missing_file_returns_empty_list) {
  QTemporaryDir directory;
  ASSERT_TRUE(directory.isValid());

  s21::HistoryManager manager(directory.filePath("history.jsonl"));
  const std::vector<s21::HistoryRecord> records = manager.LoadRecent();

  EXPECT_TRUE(records.empty());
  EXPECT_EQ(manager.GetLastError().code, s21::HistoryErrorCode::kNone);
}

TEST(history_manager, append_and_load_recent_in_reverse_order) {
  QTemporaryDir directory;
  ASSERT_TRUE(directory.isValid());

  s21::HistoryManager manager(directory.filePath("history.jsonl"));
  ASSERT_TRUE(manager.AppendHistory(MakeRecord(1)));
  ASSERT_TRUE(manager.AppendHistory(MakeRecord(2)));
  ASSERT_TRUE(manager.AppendHistory(MakeRecord(3)));

  const std::vector<s21::HistoryRecord> records = manager.LoadRecent();

  ASSERT_EQ(records.size(), 3U);
  EXPECT_EQ(records[0].record_id, "3");
  EXPECT_EQ(records[1].record_id, "2");
  EXPECT_EQ(records[2].record_id, "1");
}

TEST(history_manager, rejects_invalid_results) {
  QTemporaryDir directory;
  ASSERT_TRUE(directory.isValid());

  s21::HistoryManager manager(directory.filePath("history.jsonl"));
  s21::HistoryRecord record = MakeRecord(1);
  record.result = "nan";

  EXPECT_FALSE(manager.AppendHistory(record));
  EXPECT_EQ(manager.GetLastError().code,
            s21::HistoryErrorCode::kInvalidRecord);
  EXPECT_TRUE(manager.LoadRecent().empty());
}

TEST(history_manager, clear_removes_all_records) {
  QTemporaryDir directory;
  ASSERT_TRUE(directory.isValid());

  s21::HistoryManager manager(directory.filePath("history.jsonl"));
  ASSERT_TRUE(manager.AppendHistory(MakeRecord(1)));
  ASSERT_TRUE(manager.AppendHistory(MakeRecord(2)));

  ASSERT_TRUE(manager.ClearHistory());
  EXPECT_TRUE(manager.LoadRecent().empty());
}

TEST(history_manager, keeps_only_latest_one_hundred_records) {
  QTemporaryDir directory;
  ASSERT_TRUE(directory.isValid());

  s21::HistoryManager manager(directory.filePath("history.jsonl"));
  for (int i = 0; i < 105; ++i) {
    ASSERT_TRUE(manager.AppendHistory(MakeRecord(i))) << i;
  }

  const std::vector<s21::HistoryRecord> records = manager.LoadRecent(200);

  ASSERT_EQ(records.size(), 100U);
  EXPECT_EQ(records.front().record_id, "104");
  EXPECT_EQ(records.back().record_id, "5");
}

TEST(history_manager, damaged_lines_are_skipped) {
  QTemporaryDir directory;
  ASSERT_TRUE(directory.isValid());
  const QString history_path = directory.filePath("history.jsonl");

  QFile file(history_path);
  ASSERT_TRUE(file.open(QIODevice::WriteOnly | QIODevice::Text));
  QTextStream stream(&file);
  stream << "{not-json}\n";
  stream << "{\"record_id\":\"1\",\"expression\":\"1+1\","
            "\"display_expression\":\"1+1\",\"x_value\":\"0\","
            "\"result\":\"2\",\"created_at\":\"2026-06-18T00:00:00.000Z\"}\n";
  file.close();

  s21::HistoryManager manager(history_path);
  const std::vector<s21::HistoryRecord> records = manager.LoadRecent();

  ASSERT_EQ(records.size(), 1U);
  EXPECT_EQ(records[0].record_id, "1");
}

TEST(history_manager, write_failure_is_reported) {
  QTemporaryDir directory;
  ASSERT_TRUE(directory.isValid());

  QFile blocking_file(directory.filePath("not_a_directory"));
  ASSERT_TRUE(blocking_file.open(QIODevice::WriteOnly));
  blocking_file.close();

  s21::HistoryManager manager(
      directory.filePath("not_a_directory/history.jsonl"));

  EXPECT_FALSE(manager.AppendHistory(MakeRecord(1)));
  EXPECT_EQ(manager.GetLastError().code, s21::HistoryErrorCode::kWriteFailed);
}
