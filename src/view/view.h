#ifndef CPP3_SMARTCALC_V2_0_SRC_VIEW_VIEW_H_
#define CPP3_SMARTCALC_V2_0_SRC_VIEW_VIEW_H_

#include <QMainWindow>
#include <QPushButton>
#include <QStack>

#include "../controller/controller.h"
#include "../history/history_manager.h"
#include "format_string.h"
#include "graph.h"
#include "keyboard_adapter.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class View;
}
QT_END_NAMESPACE

/**
 * @brief 计算器程序的主界面类。
 * @details 该类负责提供计算器的图形界面，处理用户交互、显示计算结果，
 * 并管理用于绘制数学函数图像的绘图窗口。
 */
class View : public QMainWindow {
  Q_OBJECT

 public:
  /**
   * @brief 构造主界面对象。
   * @param parent 继承的父级窗口。
   * @param controller 指向 Controller 实例的指针。
   */
  View(QWidget *parent = nullptr, s21::Controller *controller = nullptr);

  /**
   * @brief 默认析构函数。
   */
  ~View();

 protected:
  void keyPressEvent(QKeyEvent *event) override;

 public slots:
  /**
   * @brief 处理数字按钮点击事件。
   * @details 对数字词法单元首位为 0 的情况进行控制，避免重复输入多个前导 0。
   */
  void NumberClicked();

  /**
   * @brief 处理加号和减号按钮点击事件。
   */
  void PlusMinusOperatorClicked();

  /**
   * @brief 处理乘号和除号按钮点击事件。
   */
  void MulDivOperatorClicked();

  /**
   * @brief 处理三角函数和对数函数按钮点击事件。
   * @details 支持 `sin`、`cos`、`tan`、`asin`、`acos`、`atan`、`ln` 和 `log`。
   */
  void MathFunctionClicked();

  /**
   * @brief 处理“全部清空”按钮点击事件。
   */
  void ClearButtonClicked();

  /**
   * @brief 处理左括号按钮点击事件。
   */
  void OpenParenthesisButtonClicked();

  /**
   * @brief 处理右括号按钮点击事件。
   */
  void CloseParenthesisButtonClicked();

  /**
   * @brief 处理小数点按钮点击事件。
   */
  void PointButtonClicked();

  /**
   * @brief 处理 `mod` 按钮点击事件。
   */
  void ModButtonClicked();

  /**
   * @brief 处理幂运算按钮点击事件。
   */
  void PowButtonClicked();

  /**
   * @brief 处理平方根按钮点击事件。
   */
  void SqrtButtonClicked();

  /**
   * @brief 处理变量 `x` 按钮点击事件。
   */
  void XButtonClicked();

  /**
   * @brief 处理科学计数法符号 `e` 按钮点击事件。
   */
  void EButtonClicked();

  /**
   * @brief 处理退格按钮点击事件。
   */
  void BackspaceClicked();

  /**
   * @brief 处理等号按钮点击事件。
   */
  void EqualButtonClicked();

  /**
   * @brief 处理打开绘图窗口按钮点击事件。
   */
  void OpenGraphWindow();

  /**
   * @brief 打开计算历史记录窗口。
   */
  void OpenHistoryWindow();

 private slots:

  /**
   * @brief 将绘图窗口实例重置为 `nullptr`。
   */
  void GraphWindowClosed();

  /**
   * @brief 从 `string_to_calculate_` 和 `string_to_show_` 末尾删除指定数量的字符。
   * @param number_to_chop 要删除的字符数量。
   */
  void ChopString(size_t number_to_chop);

  /**
   * @brief 获取当前数字词法单元中是否已经输入过小数点。
   * @param str 指向 `string_to_calculate_` 当前字符的迭代器。
   * @return 若当前数字中已存在小数点则返回 `true`，否则返回 `false`。
   */
  bool GetPointStatus(QString::ConstIterator str);

  /**
   * @brief 获取最后一个词法单元是否为运算符。
   * @param str 指向 `string_to_calculate_` 当前字符的迭代器。
   * @return 若最后一个词法单元为运算符则返回 `true`，否则返回 `false`。
   */
  bool GetOperatorStatus(QString::ConstIterator str);

  /**
   * @brief 获取当前数字词法单元首位 0 的状态。
   * @param str 指向 `string_to_calculate_` 当前字符的迭代器。
   * @return 返回 `true` 表示允许继续输入 0，返回 `false` 表示不允许。
   */
  bool GetZeroStatus(QString::ConstIterator str);

  /**
   * @brief 获取最后一个词法单元是否为数字。
   * @param str 指向 `string_to_calculate_` 当前字符的迭代器。
   * @return 若最后一个词法单元为数字则返回 `true`，否则返回 `false`。
   */
  bool GetNumStatus(QString::ConstIterator str);

  /**
   * @brief 获取最后一个词法单元是否为变量 `x`。
   * @param str 指向 `string_to_calculate_` 当前字符的迭代器。
   * @return 若最后一个词法单元为 `x` 则返回 `true`，否则返回 `false`。
   */
  bool GetXStatus(QString::ConstIterator str);

  /**
   * @brief 获取最近一次是否输入了科学计数法符号 `e`。
   * @param str 指向 `string_to_calculate_` 当前字符的迭代器。
   * @return 若最近输入包含 `e` 则返回 `true`，否则返回 `false`。
   */
  bool GetEStatus(QString::ConstIterator str);

  /**
   * @brief 将计算结果整理后显示到界面中。
   * @details 该函数会根据结果类型选择整数、小数或科学计数法显示；
   * 当结果异常时，将界面内容设置为 `calculation error`。
   * @param result 要显示的计算结果。
   */
  void SetResult(long double &result);

  /**
   * @brief 根据键盘动作复用现有按钮输入槽函数。
   * @param action 由 KeyboardAdapter 解析出的键盘动作。
   * @return 若动作已被处理则返回 true。
   */
  bool HandleKeyboardAction(const s21::KeyboardAction &action);

  /**
   * @brief 判断当前焦点是否允许主窗口处理表达式键盘输入。
   * @return 若可以处理表达式键盘输入则返回 true。
   */
  bool IsExpressionKeyboardFocusAllowed() const;

  /**
   * @brief 获取键盘动作对应的现有按钮。
   * @param action 键盘动作。
   * @return 对应按钮指针，不存在时返回 nullptr。
   */
  QPushButton *ButtonForKeyboardAction(
      const s21::KeyboardAction &action) const;

  /**
   * @brief 追加幂运算符，支持按钮的分组形式和键盘的普通形式。
   * @param open_group 若为 true，则追加 `^(` 并维护括号计数。
   */
  void AppendPowerOperator(bool open_group);

  /**
   * @brief 根据当前内部表达式重建输入状态标记。
   */
  void RebuildInputState();

  /**
   * @brief 将历史记录回填到主界面。
   * @param record 待回填历史记录。
   */
  void LoadHistoryRecordToView(const s21::HistoryRecord &record);

  /**
   * @brief 保存有效计算历史记录。
   */
  void SaveHistoryRecord(const QString &expression,
                         const QString &display_expression,
                         const QString &x_value, long double result);

  /**
   * @brief 去掉浮点数末尾多余的 0。
   * @param value 待处理的数值。
   * @return 去除尾部多余 0 后的字符串。
   */
  QString TruncateZeros(long double &value);

  /**
   * @brief 调试用函数，输出当前所有状态标记和表达式内容。
   */
  void GetAllFlags();

 private:
  Ui::View *ui_;                 ///< 指向界面实例的指针。
  Graph *graph_ = nullptr;       ///< 指向绘图窗口的指针。
  s21::Controller *controller_;  ///< 指向 Controller 实例的指针。
  s21::HistoryManager history_manager_;  ///< 历史记录管理器。
  QPushButton *history_button_ = nullptr;  ///< 历史记录入口按钮。
  QString string_to_calculate_;  ///< 供解析和计算使用的表达式字符串。
  QString string_to_show_;       ///< 用于在界面中展示的表达式字符串。
  bool num_clicked_ = false;       ///< 标记最后输入的词法单元是否为数字。
  bool point_clicked_ = false;     ///< 标记当前数字中是否已经输入小数点。
  bool operator_clicked_ = false;  ///< 标记最后输入的词法单元是否为运算符。
  bool x_clicked_ = false;         ///< 标记最后输入的词法单元是否为变量 `x`。
  bool e_clicked_ = false;         ///< 标记是否输入了科学计数法中的 `e`。
  bool flag_first_zero_ = false;   ///< 标记数字首位是否为单独的 `0`。
  int open_parenthesis_clicked_ = 0;  ///< 记录当前未闭合左括号的数量。
};

#endif
