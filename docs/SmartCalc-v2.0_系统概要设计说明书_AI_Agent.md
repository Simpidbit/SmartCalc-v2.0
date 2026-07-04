---
document_id: SC-HLD-0001
title: SmartCalc-v2.0 智能计算器系统概要设计说明书
source_format: DOCX
converted_format: Markdown
converted_for: AI Agent 阅读与检索
project: SmartCalc-v2.0
team: 周雷吉雨 / 解恩地
version: V1.0
release_date: 2026-06-03
---

# SmartCalc-v2.0 智能计算器系统概要设计说明书

> 转写说明：本文由原 DOCX 转为 AI Agent 友好的 Markdown。保留原章节、表格、接口说明、流程说明和运行目录；图示保留为图片链接，并在每张图后补充“图示说明”，方便无法直接解析图片的 Agent 读取。

# 修改记录
| **版本** | **变更原因** | **变更内容简述**                                                          | **编制/修订者** | **批准者** | **发布日期** |
|----------|--------------|---------------------------------------------------------------------------|-----------------|------------|--------------|
| V0.1     | 创建         | 依据系统需求分析文档、SmartCalc-v2.0 源代码包和系统概要设计模板编制初稿。 | 周雷吉雨        | 解恩地     | 2026-06-03   |
| V1.0     | 提交         | 补充 MVC 结构、软件包、构件、流程、数据结构、部署、性能和错误处理设计。   | 周雷吉雨        | 解恩地     | 2026-06-03   |

# 目录

- [修改记录](#修改记录)
- [1 引言](#1-引言)
  - [1.1 编写目的](#11-编写目的)
  - [1.2 项目背景](#12-项目背景)
  - [1.3 定义](#13-定义)
  - [1.4 参考资料](#14-参考资料)
- [2 条件与限制](#2-条件与限制)
  - [2.1 需求约束](#21-需求约束)
  - [2.2 设计约束](#22-设计约束)
  - [2.3 隐含约束](#23-隐含约束)
- [3 需求规定](#3-需求规定)
- [4 运行环境](#4-运行环境)
- [5 物理架构](#5-物理架构)
- [6 逻辑结构设计](#6-逻辑结构设计)
  - [6.1 软件包设计](#61-软件包设计)
  - [6.2 CSCI 模块设计](#62-csci-模块设计)
  - [6.3 构件设计](#63-构件设计)
  - [6.4 软件包与构件映射关系](#64-软件包与构件映射关系)
- [7 运行与流程设计](#7-运行与流程设计)
  - [7.1 功能流程设计](#71-功能流程设计)
  - [7.2 运行设计](#72-运行设计)
- [8 数据（库）结构设计](#8-数据库结构设计)
  - [8.1 逻辑结构设计要点](#81-逻辑结构设计要点)
  - [8.2 物理结构设计要点](#82-物理结构设计要点)
- [9 部署与运行目录设计](#9-部署与运行目录设计)
  - [9.1 部署设计](#91-部署设计)
  - [9.2 运行目录结构](#92-运行目录结构)
- [10 性能设计](#10-性能设计)
- [11 系统出错处理设计](#11-系统出错处理设计)
- [12 开发环境](#12-开发环境)
- [13 尚未解决的问题](#13-尚未解决的问题)
- [14 用户需求与系统需求规格对照表](#14-用户需求与系统需求规格对照表)

# 1 引言
本章说明本文档的编写目的、项目背景、术语定义和参考资料。本文档依据《SmartCalc-v2.0 智能计算器系统需求分析》、SmartCalc-v2.0 源代码包及系统设计说明书模板编写，面向概要设计评审、详细设计、编码、测试和课程验收使用。

## 1.1 编写目的
本系统概要设计说明书用于描述 SmartCalc-v2.0 智能计算器系统的总体技术方案、运行环境、物理与逻辑架构、软件包与构件划分、主要接口、运行流程、数据结构、部署方式、性能设计和出错处理设计。

本说明书是后续详细设计、编码实现、单元测试、系统测试和维护工作的依据。文档读者包括项目开发人员、测试人员、评审人员、指导老师和后续维护人员。

## 1.2 项目背景
SmartCalc-v2.0 是一个基于 C++17 与 Qt Widgets 的单机桌面智能计算器系统，面向课程实验、数学学习和基础工程计算场景。系统提供表达式输入与编辑、表达式格式化与数学计算、变量 x 管理、函数图像绘制、界面显示与错误处理、键盘输入支持、计算历史记录管理和函数图像导出等功能。

系统采用 MVC 架构。视图层负责用户界面、按钮事件、表达式显示、变量输入、图像窗口、键盘事件、历史记录入口和导出入口；控制器层负责将计算和绘图请求转发给模型；模型层负责表达式解析、逆波兰表达式转换、数学求值和图像坐标生成。

系统为独立桌面应用，与外部业务系统无数据交换关系，不依赖服务器、数据库或网络通信；历史记录和导出图像均通过本地文件系统保存。

## 1.3 定义
**表 1.1 术语定义**

| **术语/缩写** | **定义**                                                                               |
|---------------|----------------------------------------------------------------------------------------|
| SmartCalc     | 本文档描述的智能计算器桌面应用系统。                                                   |
| CSCI          | Computer Software Configuration Item，计算机软件配置项。本文用于标识顶层软件包或模块。 |
| MVC           | Model-View-Controller，模型-视图-控制器架构模式。                                      |
| RPN           | Reverse Polish Notation，逆波兰表达式，便于栈式求值。                                  |
| Shunting-yard | 调度场算法，用于将中缀表达式转换为逆波兰表达式。                                       |
| Qt Widgets    | Qt 桌面 GUI 组件库，本系统主界面和图像窗口基于该组件库。                               |
| QCustomPlot   | Qt 绘图库，用于显示函数曲线，并可作为图像导出的基础组件。                              |
| NAN/INF       | 无效数值或无穷大结果，界面显示为 calculation error。                                   |

## 1.4 参考资料
《SmartCalc-v2.0 智能计算器系统需求分析文档》，2026 年 5 月。

SmartCalc-v2.0 源代码包：README、src/CMakeLists.txt、src/Makefile、Qt UI 文件、C++ 源码和 GTest 单元测试代码。

《系统概要设计说明书（模板）》。

Qt Widgets、Qt PrintSupport、QCustomPlot、CMake、GTest、Doxygen 相关开发资料。

# 2 条件与限制
本系统设计以需求分析文档和代码包为主要输入，设计范围覆盖基础版本功能和需求文档中列为本版本正式需求的键盘输入、计算历史记录、图像导出三项新增用户可见功能。

## 2.1 需求约束
系统必须支持表达式输入、基础运算、高级函数、变量 x、函数图像绘制、键盘输入、历史记录和图像导出。

界面应限制明显非法输入，计算无效结果统一显示 calculation error，图像非法输入显示 invalid input。

历史记录默认保存最近 100 条有效计算记录，不保存 calculation error 等失败结果。

图像导出仅支持用户触发，将当前有效函数图像导出为 PNG 或 PDF 文件。

## 2.2 设计约束
编程语言采用 C++17，GUI 基于 Qt Widgets，绘图基于 QCustomPlot。

保持 MVC 架构，视图层不直接实现核心数学算法，模型层不依赖 Qt UI。

系统不引入服务器、数据库或网络通信依赖；本地持久化使用轻量文件。

构建脚本继续使用 CMake 3.5+ 和 Makefile，测试框架使用 GTest，代码风格遵循 Google C++ 风格。

## 2.3 隐含约束
用户具备基本数学表达式输入能力，主要通过按钮和键盘进行交互。

模型层输入默认来自已通过界面层规则校验的表达式，不面向外部系统开放任意字符串接口。

图像绘制固定采样步长为 0.1，曲线用于教学与观察，不作为高精度数值分析输出。

课程验收环境需安装 Qt、CMake、GTest、编译器和必要的覆盖率/文档工具。

# 3 需求规定
系统需求分为 8 个功能域、33 项功能需求。概要设计按照功能域将需求映射到软件包和构件，确保需求、设计、测试可追踪。

**表 3.1 用例需求规定**

| **用例编号** | **功能域**             | **需求简述**                                               | **主要设计模块**                              |
|--------------|------------------------|------------------------------------------------------------|-----------------------------------------------|
| SC-UC-T01    | 表达式输入与编辑       | 数字、小数点、科学计数法、运算符、函数、括号、清空、退格。 | View / KeyboardAdapter                        |
| SC-UC-T02    | 表达式格式化与数学计算 | 界面表达式格式化、中缀转 RPN、栈式求值、结果格式化显示。   | FormatString / Controller / Calculation / RPN |
| SC-UC-T03    | 变量 x 管理            | x 值输入、表达式中插入 x、按 x 值求值。                    | View / Controller / Calculation               |
| SC-UC-T04    | 函数图像绘制           | 打开图像窗口、设置坐标范围、生成坐标、绘制曲线。           | Graph / Controller / PlotGraph                |
| SC-UC-T05    | 界面显示与错误处理     | 显示表达式、结果、错误提示，保持可恢复状态。               | View / Graph / ErrorHandler                   |
| SC-UC-T06    | 键盘输入支持           | 数字、运算符、括号、变量、Enter、Backspace、Esc/Delete。   | KeyboardAdapter / View                        |
| SC-UC-T07    | 计算历史记录管理       | 保存、查看、清空最近计算历史记录。                         | HistoryManager / HistoryStore                 |
| SC-UC-T08    | 函数图像导出           | 导出 PNG/PDF，处理无有效图像、路径不可写等异常。           | GraphExporter / Graph                         |

主要性能要求包括：表达式长度在验收规模内点击“=”后 0.5 秒内显示结果；默认绘图范围约 200 个采样点，图像刷新 1 秒内完成；键盘输入后 0.1 秒内更新显示；加载最近 100 条历史记录 0.5 秒内完成；默认绘图规模 PNG/PDF 导出 2 秒内完成。

# 4 运行环境
## 4.1 硬件环境
系统运行在普通桌面计算机或课程验收主机上。硬件需满足 Qt 桌面程序运行和显示需求，具备鼠标、键盘和显示器。建议显示分辨率不低于 1366×768，以保证主窗口和图像窗口完整显示。

**表 4.1 硬件环境要求**

| **硬件项** | **要求**                                                                |
|------------|-------------------------------------------------------------------------|
| 处理器     | 支持 C++17/Qt 程序运行的 x86_64 或 ARM 桌面处理器。                     |
| 内存       | 建议 4GB 及以上；默认计算和绘图规模下满足默认计算和绘图规模的运行需求。 |
| 显示设备   | 支持图形桌面环境，分辨率建议 1366×768 及以上。                          |
| 输入设备   | 鼠标、键盘。                                                            |
| 存储设备   | 具备普通本地文件读写能力，用于历史记录和 PNG/PDF 导出。                 |

## 4.2 软件环境
**表 4.2 软件环境要求**

| **软件项**      | **要求**                                                                  |
|-----------------|---------------------------------------------------------------------------|
| 操作系统        | 支持安装 Qt 的 Windows、macOS 或 Linux 桌面系统；课程验收以实际配置为准。 |
| GUI 运行库      | Qt Widgets；构建配置支持 Qt 6 或 Qt 5，并链接 Qt PrintSupport。           |
| 绘图库          | QCustomPlot。                                                             |
| 编程语言/标准库 | C++17 与 C++ 标准数学库。                                                 |
| 数据库/网络     | 不使用数据库、服务器或网络通信。                                          |

# 5 物理架构
SmartCalc-v2.0 采用单机桌面物理架构，所有功能在用户本机完成。用户通过键盘和鼠标操作主窗口和图像窗口；计算、绘图、历史记录和导出均在本地进程内执行；历史记录文件和导出文件保存在本地文件系统。

![图 5.1 系统总体物理结构图](SmartCalc-v2.0_系统概要设计说明书_AI_Agent_assets/fig5-1_physical_architecture.png)

> 图示说明：SmartCalc-v2.0 采用单机桌面部署：用户通过鼠标/键盘与 SmartCalc 可执行程序交互，显示主窗口/图像窗口；程序依赖 Qt Widgets 与 QCustomPlot；历史记录文件 history.jsonl 与导出 PNG/PDF 均写入本地文件系统，不涉及服务器、数据库或网络通信。

**图 5.1 系统总体物理结构图**

# 6 逻辑结构设计
## 6.1 软件包设计
![图 6.1 系统顶层软件包图](SmartCalc-v2.0_系统概要设计说明书_AI_Agent_assets/fig6-1_top_package_diagram.png)

> 图示说明：顶层 CSCI 包包括 ViewUI、Controller、ExpressionModel、GraphModel、History、GraphExport 与 CommonBuild。ViewUI 负责界面和输入，Controller 负责请求转发，ExpressionModel 负责表达式计算，GraphModel 负责采样坐标，History 负责最近 100 条有效计算记录，GraphExport 负责 PNG/PDF 导出，CommonBuild 负责资源、构建和测试。

**图 6.1 系统顶层软件包图**

系统由应用入口、视图层、控制层、模型层、绘图模型、资源文件、构建测试脚本以及新增设计包组成。新增设计包用于承载键盘输入、历史记录和图像导出功能，设计上复用现有 View、Graph、Controller 和 Model 接口。

**表 6.1 软件配置项模块划分及功能说明**

<table>
<colgroup>
<col style="width: 19%" />
<col style="width: 17%" />
<col style="width: 62%" />
</colgroup>
<thead>
<tr class="header">
<th><strong>模块名称、标识</strong></th>
<th><strong>模块类别</strong></th>
<th><strong>功能简述</strong></th>
</tr>
</thead>
<tbody>
<tr class="odd">
<td>CSCI-SC-01<br />
ViewUI</td>
<td>视图层包</td>
<td>主窗口、图像窗口、表达式输入、显示、按钮槽函数、键盘事件入口和 UI 状态维护。</td>
</tr>
<tr class="even">
<td>CSCI-SC-02<br />
Controller</td>
<td>控制层包</td>
<td>接收视图层计算/绘图请求，调用模型层并返回结果或坐标序列。</td>
</tr>
<tr class="odd">
<td>CSCI-SC-03<br />
ExpressionModel</td>
<td>表达式计算模型包</td>
<td>表达式格式化、词法单元、RPN 转换、栈式求值、数学函数计算。</td>
</tr>
<tr class="even">
<td>CSCI-SC-04<br />
GraphModel</td>
<td>绘图模型包</td>
<td>读取 x 范围、按固定步长采样、生成 x/y 坐标序列、交由 QCustomPlot 展示。</td>
</tr>
<tr class="odd">
<td>CSCI-SC-05<br />
History</td>
<td>历史记录包</td>
<td>保存、读取、清空最近 100 条有效计算历史记录。</td>
</tr>
<tr class="even">
<td>CSCI-SC-06<br />
GraphExport</td>
<td>图像导出包</td>
<td>导出当前有效函数图像为 PNG/PDF，并处理导出异常。</td>
</tr>
<tr class="odd">
<td>CSCI-SC-07<br />
CommonBuild</td>
<td>公共资源与构建包</td>
<td>资源文件、图标、CMake、Makefile、测试与覆盖率目标。</td>
</tr>
</tbody>
</table>

## 6.2 CSCI 模块设计
### 6.2.1 CSCI-SC-01 视图层包（ViewUI）
模块名称：视图层包（ViewUI）。对应现有代码 src/view/view.h、view.cc、view.ui、graph.h、graph.cc、graph.ui、format_string.h、format_string.cc。

功能描述：提供计算器主窗口和图像窗口，处理按钮点击、表达式显示、x 值输入、图像窗口创建和状态刷新。（如下图）新增键盘输入应在 View 层通过 keyPressEvent、QShortcut 或 QAction 统一映射到已有输入槽函数，避免绕过现有输入合法性检查。

**图 6.2 主要界面原型图**

![图 6.2 主窗口界面原型](SmartCalc-v2.0_系统概要设计说明书_AI_Agent_assets/fig6-2_main_ui_prototype.png)

> 图示说明：主窗口包含表达式显示区、x 值输入区、数字/函数/运算符按钮、清空、退格、等号和绘图入口，用于表达式输入、变量管理和计算触发。

![图 6.2 图像窗口界面原型](SmartCalc-v2.0_系统概要设计说明书_AI_Agent_assets/fig6-2_graph_ui_prototype.png)

> 图示说明：图像窗口包含 QCustomPlot 曲线显示区、表达式显示区和 x/y 坐标范围输入控件，用于函数图像绘制和刷新。

**表 6.2 视图层包接口设计说明**

| **接口名称**                      | **调用方**                              | **实现方**               | **输入参数**                                                           | **输出/返回**                                              | **前置条件**                                                             | **异常与错误处理**                                                                 | **说明**                                                                                                 |
|-----------------------------------|-----------------------------------------|--------------------------|------------------------------------------------------------------------|------------------------------------------------------------|--------------------------------------------------------------------------|------------------------------------------------------------------------------------|----------------------------------------------------------------------------------------------------------|
| NumberClicked(token)              | Qt 按钮事件、KeyboardAdapter            | ViewUI                   | token：数字字符 0—9、小数点“.”、科学计数法符号“e”                      | 更新后的显示表达式；内部表达式状态同步更新                 | 主窗口已初始化；display 可正常刷新                                       | 若小数点重复、e 位置非法或当前状态不允许继续追加，则拒绝本次输入并保持原表达式     | 负责维护 string_to_show\_、string_to_calculate\_、输入状态标志和光标/末尾 token 状态                     |
| OperatorClicked(token)            | Qt 按钮事件、KeyboardAdapter            | ViewUI                   | token：+、-、\*、/、mod、^ 等运算符                                    | 更新后的显示表达式；内部表达式追加对应模型 token           | 当前表达式非空，或 token 为允许的一元符号；上一 token 不为非法连续运算符 | 对连续二元运算符、表达式起始非法运算符、括号后非法运算符进行拒绝处理               | 运算符显示形式与模型内部 token 可不同，View 负责完成 UI token 到内部 token 的映射                        |
| FunctionClicked(functionName)     | Qt 按钮事件、KeyboardAdapter            | ViewUI                   | functionName：sin、cos、tan、asin、acos、atan、sqrt、ln、log 等        | 显示表达式追加函数名及左括号；内部表达式追加对应函数 token | 当前输入位置允许函数调用，例如表达式开始、运算符之后或左括号之后         | 若函数插入位置非法，则拒绝输入并保持当前状态                                       | 函数 token 需与 FormatString、ExpressionModel 约定一致，避免模型层无法识别                               |
| ParenthesisClicked(token)         | Qt 按钮事件、KeyboardAdapter            | ViewUI                   | token：“(” 或 “)”                                                      | 更新后的表达式显示和括号计数                               | 左括号可在表达式起始、运算符后、函数后出现；右括号必须存在未闭合左括号   | 右括号数量超过左括号数量、括号位置非法时拒绝输入                                   | View 维护括号平衡状态，为计算前校验提供依据                                                              |
| BackspaceClicked()                | Qt 按钮事件、KeyboardAdapter            | ViewUI                   | 无                                                                     | 删除后的显示表达式和内部表达式                             | 主窗口处于可编辑状态                                                     | 表达式为空时不执行操作；删除函数 token 时应按完整 token 删除，避免残留非法字符串   | 用于支持按钮退格和键盘 Backspace，保证显示字符串与内部字符串同步                                         |
| ClearClicked()                    | Qt 按钮事件、KeyboardAdapter            | ViewUI                   | 无                                                                     | 清空表达式、结果、错误提示和输入状态                       | 主窗口已初始化                                                           | 无                                                                                 | 对应 C、Esc、Delete 等清空操作，恢复到初始可输入状态                                                     |
| EqualButtonClicked()              | 用户点击“=”、KeyboardAdapter 处理 Enter | ViewUI                   | 当前显示表达式、内部表达式、x_value                                    | 有效计算结果或 calculation error                           | 表达式非空；括号匹配；不以非法二元运算符结尾；若包含 x，则 x_value 可用  | Controller 返回 NAN/INF 或模型异常时显示 calculation error；失败结果不写入历史记录 | 先调用 FormatString 生成模型表达式，再调用 Controller::Calculate；成功后触发 History 保存                |
| OpenGraphWindow()                 | 用户点击绘图入口                        | ViewUI                   | 当前表达式、当前 x 轴/y 轴默认范围或用户设置范围                       | Graph 窗口对象；当前曲线显示状态                           | 当前表达式可作为函数表达式传递；Graph 组件可创建                         | 表达式为空或 Graph 初始化失败时提示 invalid input 或窗口创建失败                   | 负责创建或激活 Graph 窗口，并将表达式传递给 Graph 继续处理                                               |
| RefreshDisplay(displayText)       | ViewUI 内部调用                         | ViewUI                   | displayText：待显示文本                                                | 主窗口 display 更新                                        | UI 控件已初始化                                                          | 若 displayText 为空则显示空白或默认状态                                            | 所有输入、计算、清空操作统一通过该接口刷新界面，避免多处分散更新                                         |
| KeyboardAdapter::HandleKey(event) | Qt keyPressEvent、QShortcut 或 QAction  | KeyboardAdapter / ViewUI | QKeyEvent：数字键、运算符键、函数快捷键、Enter、Backspace、Esc、Delete | 对应输入、计算、退格、清空动作                             | 主窗口拥有键盘焦点；快捷键映射表已初始化                                 | 未支持按键忽略；非法输入按 ViewUI 原有规则拒绝                                     | 键盘输入不得直接修改表达式字符串，必须复用 NumberClicked、OperatorClicked、EqualButtonClicked 等既有接口 |

### 6.2.2 CSCI-SC-02 控制层包（Controller）
模块名称：控制层包（Controller）。对应现有代码 src/controller/controller.h。

功能描述：作为视图层与模型层之间的中介，提供 Calculate 和 CalculateGraphCoordinates 两个核心接口。控制层不保存 UI 状态，不直接执行具体数学算法。

**表 6.3 控制层包接口设计说明**

| **接口名称**                                                                                                            | **调用方**    | **实现方**                   | **输入参数**                                                        | **输出/返回**                                                    | **前置条件**                                                          | **异常与错误处理**                                                                                             | **说明**                                                                  |
|-------------------------------------------------------------------------------------------------------------------------|---------------|------------------------------|---------------------------------------------------------------------|------------------------------------------------------------------|-----------------------------------------------------------------------|----------------------------------------------------------------------------------------------------------------|---------------------------------------------------------------------------|
| long double Calculate(const std::string& expression, long double x_value)                                               | ViewUI        | Controller                   | expression：FormatString 处理后的内部表达式；x_value：变量 x 的取值 | long double 计算结果                                             | expression 非空，且 token 格式符合 ExpressionModel 约定               | 若模型层解析失败、计算结果为 NAN/INF 或发生定义域错误，则返回 NAN 或约定错误值，由 View 显示 calculation error | Controller 不做具体数学运算，只负责创建/调用 Calculation 并返回 GetResult |
| GraphCoordinates CalculateGraphCoordinates(const std::string& expression, std::pair\<long double,long double\> x_range) | Graph         | Controller                   | expression：内部函数表达式；x_range：x_min、x_max                   | GraphCoordinates：x_values、y_values、valid_flags 或等价坐标集合 | x_min \< x_max；expression 可由模型层解析；绘图步长采用系统默认值 0.1 | 对单个采样点 NAN/INF 可跳过或标记为无效；若整体表达式非法，则返回空坐标集合                                    | Controller 调用 PlotGraph，统一向 Graph 屏蔽绘图模型细节                  |
| bool ValidateExpressionForCalculate(const std::string& expression)                                                      | ViewUI        | Controller / ExpressionModel | expression：内部表达式                                              | true/false                                                       | 表达式已完成界面层基本校验                                            | 表达式为空、括号不平衡、token 不合法时返回 false                                                               | 可选接口，用于计算前更早发现模型层不可识别的表达式                        |
| bool ValidateGraphRange(long double x_min, long double x_max, long double y_min, long double y_max)                     | Graph         | Controller                   | x_min、x_max、y_min、y_max                                          | true/false                                                       | Graph 窗口已读取用户输入范围                                          | 范围为空、非数字、x_min \>= x_max 或 y_min \>= y_max 时返回 false                                              | 用于统一绘图范围合法性判断，减少 Graph 内部重复判断                       |
| ErrorInfo GetLastError()                                                                                                | ViewUI、Graph | Controller                   | 无                                                                  | ErrorInfo：错误码、错误来源、错误消息                            | 前一次调用发生失败或需要查询状态                                      | 无错误时返回 NONE                                                                                              | 可选接口，用于详细设计阶段扩展更精确的错误提示；概要设计中可先约定错误码  |

### 6.2.3 CSCI-SC-03 表达式计算模型包（ExpressionModel）
模块名称：表达式计算模型包（ExpressionModel）。对应现有代码 src/model/calculation.\*、reverse_polish_notation.\*、lexeme.h，以及视图侧 format_string.\*。

功能描述：将界面表达式转换为内部 token，使用 Shunting-yard 算法转换为 RPN，基于栈完成加减乘除、取模、幂、平方根、三角函数、反三角函数、对数函数和变量 x 的求值。

**表 6.4 表达式计算模型包接口设计说明**

| **内部类/结构**       | **供给接口**                                                   | **调用方**                         | **输入参数**                                                                                            | **输出/关键数据**               | **前置条件**              | **异常与错误处理**                                                                    | **职责说明**                                                                         |
|-----------------------|----------------------------------------------------------------|------------------------------------|---------------------------------------------------------------------------------------------------------|---------------------------------|---------------------------|---------------------------------------------------------------------------------------|--------------------------------------------------------------------------------------|
| FormatString          | std::string ToModelExpression(const QString& q_str)            | ViewUI                             | q_str：界面显示表达式                                                                                   | basic_str\_：模型层可识别表达式 | q_str 来自 View 输入流程  | 遇到未知函数名或无法转换字符时返回空串或错误状态                                      | 将 sin、cos、tan、asin、acos、atan、ln、log、sqrt、mod、x 等显示内容转换为模型 token |
| FormatString          | QString ToDisplayExpression(const std::string& basic_str)      | ViewUI、History                    | basic_str：内部表达式                                                                                   | q_str\_：用户可读表达式         | basic_str token 合法      | token 不可识别时保留原文本或返回错误状态                                              | 用于历史记录回填、结果展示和调试显示                                                 |
| Lexeme                | Lexeme(value, priority, type)                                  | ReversePolishNotation、Calculation | value：数字、运算符或函数标识；priority：优先级；type：NUMBER、OPERATOR、FUNCTION、VARIABLE、BRACKET 等 | Lexeme 对象                     | 输入 token 已完成词法识别 | 非法 token 标记为 UNKNOWN 或触发解析失败                                              | 作为表达式解析和 RPN 转换的最小词法单元                                              |
| ReversePolishNotation | std::list Convert(const std::string& expression)               | Calculation                        | expression：模型内部表达式                                                                              | rpn_list\_：逆波兰表达式序列    | 表达式 token 基本合法     | 括号不匹配、运算符位置非法、函数参数缺失时返回空序列或错误状态                        | 使用 Shunting-yard 算法处理中缀表达式到 RPN 的转换                                   |
| ReversePolishNotation | ErrorInfo GetLastError()                                       | Calculation、Controller            | 无                                                                                                      | 错误码和错误位置                | Convert 已执行            | 无错误时返回 NONE                                                                     | 为上层判断 calculation error 提供依据                                                |
| Calculation           | void Parse(const std::string& expression, long double x_value) | Controller、PlotGraph              | expression：内部表达式；x_value：变量 x 取值                                                            | calculation_result\_            | expression 已完成格式转换 | 解析失败、除零、负数开方、对数定义域错误、asin/acos 越界、结果 NAN/INF 时设置错误状态 | 统一完成表达式解析、RPN 转换和栈式求值                                               |
| Calculation           | long double GetResult() const                                  | Controller、PlotGraph              | 无                                                                                                      | calculation_result\_            | Parse 已执行              | 若计算失败，返回 NAN 或约定错误值                                                     | 向 Controller 和 PlotGraph 提供计算结果                                              |
| Calculation           | bool IsValidResult() const                                     | Controller、PlotGraph              | 无                                                                                                      | true/false                      | Parse 已执行              | NAN/INF 或错误状态返回 false                                                          | 便于 View 判断是否显示 calculation error、History 是否保存记录                       |
| Calculation           | ErrorInfo GetLastError() const                                 | Controller                         | 无                                                                                                      | 错误码、错误类型                | Parse 已执行              | 无错误时返回 NONE                                                                     | 可支持后续将 calculation error 细化为更明确提示                                      |
| ExpressionResult      | 数据结构                                                       | Controller、History                | result、is_valid、error_code                                                                            | 计算结果对象                    | Calculation 完成计算      | 无                                                                                    | 可选结构，用于替代单一 long double 返回值，使并行开发时错误约定更清晰                |

### 6.2.4 CSCI-SC-04 绘图模型包（GraphModel）
模块名称：绘图模型包（GraphModel）。对应现有代码 src/model/plot_graph.h 与 src/view/graph.\*。

功能描述：Graph 窗口读取 x/y 轴范围，PlotGraph 按 x_min 到 x_max 以 0.1 步长采样，每个采样点调用计算模型得到 y 值，最终交给 QCustomPlot 绘制曲线。

### 6.2.5 CSCI-SC-05 历史记录包（History）
模块名称：历史记录包（History）。该模块对应需求文档中的新增正式功能，设置 HistoryManager、HistoryStore 或等价类。

功能描述：在一次有效计算完成后保存表达式、x 值、结果、时间；提供历史列表加载、记录选择复用、清空记录功能。历史记录默认保留最近 100 条。

**表 6.5 历史记录包接口设计说明**

| **接口名称**                                              | **调用方**              | **实现方**                           | **输入参数**                                                           | **输出/返回**                | **前置条件**                                             | **异常与错误处理**                                           | **说明**                                                                  |
|-----------------------------------------------------------|-------------------------|--------------------------------------|------------------------------------------------------------------------|------------------------------|----------------------------------------------------------|--------------------------------------------------------------|---------------------------------------------------------------------------|
| bool AppendHistory(const HistoryRecord& record)           | ViewUI 或 Controller    | HistoryManager                       | record：expression、display_expression、x_value、result、created_at    | true/false                   | record.result 为有效数值；表达式非空；历史文件路径已确定 | 文件不存在时自动创建；写入失败返回 false，不影响计算结果显示 | 仅保存有效计算结果，不保存 calculation error、NAN、INF                    |
| std::vector LoadRecent(int limit = 100)                   | ViewUI 或 Controller    | HistoryManager                       | limit：加载条数，默认 100                                              | 历史记录列表，按时间倒序排列 | 历史文件存在或允许为空                                   | 文件不存在返回空列表；读取失败返回空列表并设置错误状态       | 用于打开历史记录面板时加载最近计算记录                                    |
| bool ClearHistory()                                       | ViewUI                  | HistoryManager                       | 无                                                                     | true/false                   | 用户确认清空操作                                         | 删除或重写文件失败时返回 false，界面提示清空失败             | 清空本地历史文件，并通知 View 刷新为空状态                                |
| bool TrimToLimit(int limit = 100)                         | HistoryManager 内部调用 | HistoryStore                         | limit：最大保留条数                                                    | true/false                   | AppendHistory 写入后触发                                 | 截断失败时保留原文件并返回 false                             | 保证系统默认只保留最近 100 条有效记录                                     |
| std::optional GetRecordById(const std::string& record_id) | ViewUI                  | HistoryManager                       | record_id：历史记录唯一标识                                            | 对应历史记录                 | 历史列表已加载或文件可读取                               | id 不存在时返回空                                            | 用户点击某一条历史记录后，View 通过该接口获取详情并回填表达式、x 值和结果 |
| bool ImportFromFile(const std::string& path)              | 可选维护入口            | HistoryStore                         | path：历史文件路径                                                     | true/false                   | 文件格式符合约定                                         | 格式错误或读取失败时返回 false                               | 可选接口，便于测试和后续维护                                              |
| ErrorInfo GetLastError()                                  | ViewUI、Controller      | HistoryManager                       | 无                                                                     | 错误码、错误消息             | 历史接口调用后                                           | 无错误时返回 NONE                                            | 统一描述文件不存在、不可写、解析失败、权限不足等异常                      |
| HistoryRecord                                             | 数据结构                | ViewUI、HistoryManager、HistoryStore | expression、display_expression、x_value、result、created_at、record_id | 一条历史记录                 | 计算成功后构造                                           | 字段缺失时不得写入                                           | 建议使用 JSON Lines 保存，每行一条记录，便于追加和截断                    |

### 6.2.6 CSCI-SC-06 图像导出包（GraphExport）
模块名称：图像导出包（GraphExport）。该模块对应需求文档中的新增正式功能，可基于 QCustomPlot 的 savePng、savePdf 能力实现。

功能描述：在 Graph 窗口存在有效曲线时，用户选择保存路径和格式后导出 PNG 或 PDF 文件；导出失败不得关闭窗口、不得清除当前曲线。

**表 6.6 图像导出包接口设计说明**

| **接口名称**                                                          | **调用方**                 | **实现方**             | **输入参数**                                         | **输出/返回**    | **前置条件**                                              | **异常与错误处理**                                                       | **说明**                                                                      |
|-----------------------------------------------------------------------|----------------------------|------------------------|------------------------------------------------------|------------------|-----------------------------------------------------------|--------------------------------------------------------------------------|-------------------------------------------------------------------------------|
| bool HasValidGraph() const                                            | Graph                      | GraphExporter 或 Graph | 无                                                   | true/false       | Graph 窗口已完成至少一次绘图尝试                          | 当前无曲线、坐标为空或最近一次绘图失败时返回 false                       | 导出前必须调用；无有效图像时拒绝导出并提示用户                                |
| bool ExportCurrentGraph(const ExportInfo& info)                       | Graph                      | GraphExporter          | info：path、format、width、height、quality、title 等 | true/false       | HasValidGraph() 为 true；format 为 PNG 或 PDF；path 非空  | 路径不可写、格式不支持、底层保存失败时返回 false；不得关闭窗口或清除曲线 | 统一处理 PNG/PDF 导出逻辑                                                     |
| bool ExportPng(const std::string& path, int width, int height)        | GraphExporter 内部或 Graph | GraphExporter          | path：保存路径；width、height：导出尺寸              | true/false       | path 后缀为 .png 或 format 指定 PNG                       | QCustomPlot::savePng 失败时返回 false                                    | 通过 QCustomPlot 的 PNG 保存能力实现                                          |
| bool ExportPdf(const std::string& path)                               | GraphExporter 内部或 Graph | GraphExporter          | path：保存路径                                       | true/false       | path 后缀为 .pdf 或 format 指定 PDF；Qt PrintSupport 可用 | QCustomPlot::savePdf 失败或 PrintSupport 不可用时返回 false              | 通过 QCustomPlot/Qt PrintSupport 的 PDF 保存能力实现                          |
| bool ValidateExportPath(const std::string& path, ExportFormat format) | Graph                      | GraphExporter          | path：用户选择路径；format：PNG/PDF                  | true/false       | 用户已选择保存路径                                        | 路径为空、目录不存在、无写权限、后缀与格式不一致时返回 false             | 用于导出前检查，避免生成不完整文件                                            |
| ErrorInfo GetLastError() const                                        | Graph                      | GraphExporter          | 无                                                   | 错误码、错误消息 | 导出接口调用后                                            | 无错误时返回 NONE                                                        | 错误码建议包括 NO_VALID_GRAPH、INVALID_PATH、UNSUPPORTED_FORMAT、WRITE_FAILED |
| ExportInfo                                                            | 数据结构                   | Graph、GraphExporter   | path、format、width、height、expression、created_at  | 导出请求对象     | 用户选择导出路径和格式后构造                              | 字段缺失时拒绝导出                                                       | 记录一次导出请求的必要参数，便于测试和日志定位                                |
| ExportFormat                                                          | 枚举                       | Graph、GraphExporter   | PNG、PDF                                             | 格式类型         | 无                                                        | 其他格式统一视为 UNSUPPORTED_FORMAT                                      | 避免使用字符串散落判断，提高并行开发一致性                                    |

### 6.2.7 CSCI-SC-07 公共资源与构建包（CommonBuild）
模块名称：公共资源与构建包（CommonBuild）。对应 src/resources.qrc、pics、CMakeLists.txt、Makefile、model/tests。

功能描述：管理图标资源、构建目标、安装与卸载、单元测试、覆盖率报告、风格检查、文档和发布归档。

## 6.3 构件设计
![图 6.2 MVC 软件架构关系图](SmartCalc-v2.0_系统概要设计说明书_AI_Agent_assets/fig6-3_mvc_component_diagram.png)

> 图示说明：组件关系体现 MVC 与外部依赖：SmartCalc 可执行构件内部链接 ViewUI、Controller、ExpressionModel、GraphModel、HistoryManager/HistoryStore、GraphExporter；运行时依赖 Qt Runtime/Qt Widgets、QCustomPlot 和 Qt PrintSupport；读写 history.jsonl，并可导出 graph_result.png / graph_result.pdf。

**图 6.2 MVC 软件架构关系图**

下表展示了构建设计的详细说明。

**表 6.7 构件设计说明**

| **构件名**           | **文件/目标**                                                        | **供给接口**                                                                                                                         | **需求接口**                                                     | **输入数据**                                | **输出数据**                                                   | **异常处理约定**                                                 | **开发分工说明**                                                       |
|----------------------|----------------------------------------------------------------------|--------------------------------------------------------------------------------------------------------------------------------------|------------------------------------------------------------------|---------------------------------------------|----------------------------------------------------------------|------------------------------------------------------------------|------------------------------------------------------------------------|
| SmartCalc 可执行构件 | src/main.cc、CMake target SmartCalc                                  | 启动 Qt 应用；创建 Calculation、Controller、View；进入事件循环                                                                       | Qt Runtime、Qt Widgets、资源文件                                 | 命令行启动参数、Qt 运行环境                 | 主窗口实例、应用进程状态                                       | Qt 初始化失败时退出程序并返回错误状态                            | 由集成人员负责，保证各构件对象创建顺序和依赖注入一致                   |
| View 构件            | src/view/view.、view.ui、format_string.                              | NumberClicked、OperatorClicked、FunctionClicked、BackspaceClicked、ClearClicked、EqualButtonClicked、OpenGraphWindow、RefreshDisplay | Controller、FormatString、Graph、HistoryManager、KeyboardAdapter | 用户按钮输入、键盘输入、x_value、当前表达式 | 显示表达式、计算结果、错误提示、历史记录保存请求、绘图窗口请求 | 非法输入拒绝追加；计算失败显示 calculation error；不保存失败结果 | 由 UI 开发人员负责，不直接实现数学算法和文件持久化                     |
| KeyboardAdapter 构件 | view/key_adapter.\* 或 View 内部 keyPressEvent                       | HandleKey、快捷键映射表                                                                                                              | View 构件输入槽函数                                              | QKeyEvent、QShortcut、QAction               | 数字输入、运算符输入、计算、退格、清空等动作                   | 未支持按键忽略；非法按键不得改变表达式                           | 可由 UI 人员或独立人员实现，但必须复用 View 公开输入接口               |
| Graph 构件           | src/view/graph.、graph.ui、qcustomplot.                              | ReadGraphRange、DrawCurve、RefreshGraph、OpenExportDialog、ShowGraphError                                                            | Controller 坐标接口、GraphExporter、QCustomPlot                  | 表达式、x/y 轴范围、坐标序列、导出路径      | 曲线图像、invalid input 提示、导出请求                         | 范围非法显示 invalid input；导出失败不关闭窗口、不清除曲线       | 由绘图界面开发人员负责，重点处理坐标轴、曲线刷新和导出入口             |
| Controller 构件      | src/controller/controller.h/.cc                                      | Calculate、CalculateGraphCoordinates、ValidateExpressionForCalculate、ValidateGraphRange、GetLastError                               | Calculation、PlotGraph、ErrorInfo                                | 内部表达式、x_value、x_range、y_range       | 计算结果、坐标集合、错误状态                                   | 模型异常转为 NAN、空坐标集合或 ErrorInfo                         | 由控制层开发人员负责，保证 View/Graph 不直接依赖模型实现细节           |
| Model 构件           | src/model/calculation.、reverse_polish_notation.、lexeme.h           | Parse、GetResult、IsValidResult、Convert、GetLastError                                                                               | C++ 标准数学库                                                   | 内部表达式、Lexeme 序列、x_value            | long double 结果、RPN 序列、错误状态                           | 除零、定义域错误、括号不匹配、非法 token 返回错误状态或 NAN      | 由算法开发人员负责，接口必须与 Controller 约定 token、错误码和结果格式 |
| GraphModel 构件      | src/model/plot_graph.\*                                              | CalculateGraphCoordinates、SetStep、GetValidPoints                                                                                   | Calculation、C++ 标准库                                          | 表达式、x_min、x_max、step                  | x_values、y_values、valid_flags                                | 单点计算失败时跳过或标记；整体失败返回空集合                     | 由模型/绘图算法人员负责，默认 step=0.1，与 Graph 的显示范围约定一致    |
| History 构件         | src/history/history_manager.、history_store.                         | AppendHistory、LoadRecent、ClearHistory、GetRecordById、TrimToLimit、GetLastError                                                    | 本地文件系统、QStandardPaths 或等价路径接口                      | HistoryRecord、limit、record_id             | 历史记录列表、操作状态、错误信息                               | 文件不存在返回空列表或自动创建；读写失败不得影响计算功能         | 由持久化开发人员负责，固定 history.jsonl 字段和最近 100 条策略         |
| Export 构件          | src/export/graph_export.\*                                           | HasValidGraph、ExportCurrentGraph、ExportPng、ExportPdf、ValidateExportPath、GetLastError                                            | QCustomPlot、Qt PrintSupport、本地文件系统                       | ExportInfo、保存路径、格式、当前曲线对象    | PNG/PDF 文件、导出状态、错误信息                               | 无有效图像、路径不可写、格式不支持时返回失败并保留当前曲线       | 由导出功能开发人员负责，必须与 Graph 约定当前曲线对象和导出入口        |
| Resource 构件        | src/resources.qrc、src/pics                                          | 图标和静态资源加载                                                                                                                   | Qt Resource System                                               | qrc 路径、图片资源                          | UI 图标、按钮图片、静态资源对象                                | 资源缺失时使用默认显示或提示加载失败                             | 由资源/集成人员维护，保证构建目标能正确打包资源                        |
| Test 构件            | src/model/tests/calculation_test.cc、history_test.cc、export_test.cc | 模型层单元测试、历史记录测试、导出接口测试                                                                                           | GTest、被测构件接口                                              | 测试表达式、测试历史文件、测试导出路径      | 测试报告、覆盖率结果                                           | 测试失败阻断提交或验收构建                                       | 由测试人员负责，至少覆盖计算成功/失败、历史读写、导出失败路径          |

## 6.4 软件包与构件映射关系
**表 6.8 软件包与构件映射关系**

| **软件包**               | **所属构件**            | **说明**                                                     |
|--------------------------|-------------------------|--------------------------------------------------------------|
| src/view                 | View / Graph 构件       | GUI 事件、主窗口、图像窗口、表达式格式化、QCustomPlot 集成。 |
| src/controller           | Controller 构件         | 视图层与模型层之间的请求转发。                               |
| src/model                | Model / GraphModel 构件 | 表达式词法、RPN 转换、栈式求值、绘图坐标生成。               |
| src/model/tests          | Test 构件               | 计算模块 GTest 测试。                                        |
| src/pics、resources.qrc  | Resource 构件           | 图标和 Qt 资源。                                             |
| 新增 src/history         | History 构件            | 历史记录本地持久化。                                         |
| 新增 src/export          | Export 构件             | 图像导出实现。                                               |
| CMakeLists.txt、Makefile | Build 构件              | 构建、安装、测试、覆盖率、文档和发布归档。                   |

# 7 运行与流程设计
## 7.1 功能流程设计
### 7.1.1 表达式输入与计算功能流程
![图 7.1 表达式计算功能流程图](SmartCalc-v2.0_系统概要设计说明书_AI_Agent_assets/fig7-1_expression_calculation_flow.png)

> 图示说明：计算流程包括：用户按钮或键盘输入；View 校验输入可追加性并维护显示/内部表达式；等号或 Enter 触发计算前检查；FormatString 转换为模型 token；Controller 调用 ExpressionModel；模型完成 RPN 转换和栈式求值；View 对结果格式化，成功则保存历史，失败显示 calculation error。

**图 7.1 表达式计算功能流程图**

表达式输入与计算流程从用户输入开始。用户可通过主窗口按钮或键盘输入数字、运算符、函数、括号、变量 x 等内容。主窗口 View 接收输入后，首先根据当前表达式状态判断该输入是否允许追加，例如小数点是否重复、运算符是否连续、括号是否匹配、函数或变量是否处于合法位置等。对于不可追加的输入，View 保持原有显示内容或给出非法输入提示；对于可追加输入，View 同步维护面向用户显示的表达式字符串和用于模型层计算的内部表达式字符串，并立即刷新 display。

当用户点击“=”按钮或按下 Enter 键时，View 进入计算前检查阶段。该阶段主要检查表达式是否为空、是否以非法运算符结尾、括号数量是否匹配、变量 x 是否具有可用取值等。若检查不通过，界面显示 calculation error 或保持可恢复状态，不再向控制层提交计算请求。若检查通过，View 将表达式交由格式化处理过程，把界面层函数名和显示符号转换为模型层可识别的 token 序列，然后通过 Controller 发起计算请求。

Controller 接收计算请求后，不直接执行数学运算，而是调用表达式计算模型完成解析和求值。模型层首先将中缀表达式转换为逆波兰表达式（RPN），再按照 RPN 序列进行栈式求值，依次处理数字、变量、运算符和数学函数。求值完成后，模型层将计算结果返回 Controller，再由 Controller 返回 View。View 对返回结果进行统一格式化：若结果为 NAN、INF 或其他无效数值，则显示 calculation error，且不写入历史记录；若结果为有效数值，则显示格式化结果，并可将原表达式、x 值、结果和时间交给历史记录模块保存。该流程保证了输入校验、表达式转换、计算执行、结果显示和历史记录保存之间的职责边界清晰。

### 7.1.2 函数图像绘制与导出流程

![图 7.2 函数图像绘制与导出流程图](SmartCalc-v2.0_系统概要设计说明书_AI_Agent_assets/fig7-2_graph_export_flow.png)

> 图示说明：绘图与导出流程包括：View 打开或激活 Graph；Graph 校验表达式和坐标范围；Controller 调用 GraphModel；GraphModel 以 step=0.1 遍历 x 范围并计算 y 值；QCustomPlot 刷新曲线；导出前 GraphExporter 检查有效曲线、格式和路径，再保存 PNG/PDF，失败时保留窗口和当前曲线。

**图 7.2 函数图像绘制与导出流程图**

函数图像绘制流程由用户在主窗口点击 plot the graph 或等价绘图入口触发。View 接收绘图请求后，创建或激活图像窗口 Graph，并将当前表达式传递给图像窗口。Graph 窗口负责读取用户设置的 x 轴与 y 轴显示范围，并对表达式和绘图范围进行基本合法性检查，例如表达式是否为空、是否包含可计算变量 x、范围上下界是否有效等。若检查不通过，Graph 显示 invalid input，并保持图像窗口处于可继续修改输入的状态。

在输入合法的情况下，Graph 将格式化后的表达式和 x 轴采样范围提交给 Controller。Controller 调用绘图模型生成坐标数据。绘图模型按照固定步长从 x_min 遍历到 x_max，对每一个采样点代入变量 x，并调用表达式计算模型求得对应的 y 值。对于可计算点，绘图模型将其加入坐标序列；对于计算结果为 NAN、INF 或超出可显示范围的点，模型可跳过该点或按约定进行异常点处理。所有采样完成后，绘图模型将坐标数据返回 Controller，再由 Controller 返回 Graph。

Graph 接收到坐标数据后，调用绘图控件完成曲线刷新：清除旧曲线，设置坐标轴范围，加载新的 x/y 坐标序列，并刷新图像显示。若用户继续触发图像导出，Graph 首先检查当前是否存在有效曲线，再调用 GraphExporter 处理导出请求。GraphExporter 检查导出格式是否为 PNG 或 PDF、目标路径是否有效且可写；检查通过后调用底层绘图控件或文件写入接口将当前图像保存到本地文件系统。若导出失败，系统提示导出错误，但不关闭图像窗口，也不清除当前已绘制曲线，从而保证用户可以修改路径或格式后重新导出。

### 7.1.3 历史记录管理流程
![图 7.3 历史记录管理流程图](SmartCalc-v2.0_系统概要设计说明书_AI_Agent_assets/fig7-3_history_management_flow.png)

> 图示说明：历史记录流程包括：有效计算后保存表达式、x 值、结果和时间；History 模块追加本地记录并截断到最近 100 条；打开历史面板时加载并倒序显示记录；选择历史项时回填表达式和 x 值；清空历史时重写或删除历史文件；读写失败不影响核心计算和绘图。

**图 7.3 历史记录管理流程图**（原文页内标题为“函数图像绘制与导出流程图”，此处按章节内容修正为历史记录管理流程图。）

历史记录管理流程覆盖有效计算结果保存、历史记录查看、历史项加载、清空历史记录和异常处理五类场景。一次表达式计算完成后，只有当结果为有效数值时，View 才会组织历史记录数据，包括原始表达式、变量 x 的取值、格式化计算结果和生成时间。随后 View 通过 Controller 或等价接口向 History 模块提交保存请求。History 模块负责将记录追加写入本地历史记录文件，并在记录数量超过上限时删除最早记录，以保证系统默认只保留最近 100 条有效计算历史。保存完成后，History 模块将状态返回给上层，View 可据此更新历史入口状态或进行轻量提示。若写入失败，系统不影响本次计算结果显示。

当用户打开历史记录面板时，View 向 Controller 发起加载最近历史记录的请求，Controller 将请求转发给 History 模块。History 模块读取本地历史记录文件，对记录进行解析、过滤和按时间倒序排列，然后返回历史记录列表。View 接收列表后在界面中展示表达式、x 值、结果和时间等信息，便于用户查阅最近计算过程。

当用户选择某一条历史记录时，View 请求 History 模块提供该记录的详细数据。History 模块返回对应的表达式、x 值、结果和时间后，View 将表达式和 x 值回填到主窗口输入区域，结果可作为只读参考显示。用户可在加载后的表达式基础上继续编辑、重新计算或绘制图像。若用户执行清空历史记录操作，View 向 History 模块发起清空请求，History 模块清空本地历史文件并返回操作状态，View 随后将历史记录界面刷新为空状态。

历史记录相关异常主要包括历史文件不存在、读取失败、写入失败或文件不可写等。对于这些异常，History 模块应返回明确错误状态，View 在界面中提示错误信息，同时保持主计算、绘图和已有显示结果可继续使用。该设计将历史记录作为辅助功能处理，避免本地文件异常影响计算器的核心计算流程。

## 7.2 运行设计
系统运行时仅包含一个 Qt GUI 进程。main.cc 创建 Calculation、Controller 和 View 对象，显示主窗口并进入 Qt 事件循环。用户交互通过 Qt signal/slot 或键盘事件驱动，不需要独立后台服务。

计算和默认绘图在 GUI 线程内同步执行。由于需求限定表达式长度和默认采样规模较小，现阶段可满足响应时间要求；若后续支持大范围高密度绘图，应将坐标计算移入工作线程并提供取消机制。

# 8 数据（库）结构设计
本系统不使用数据库。运行期表达式、结果、状态标志、绘图坐标保存在内存中；历史记录和图像导出文件保存在本地文件系统。

![图 8.1 主要数据结构关系图](SmartCalc-v2.0_系统概要设计说明书_AI_Agent_assets/fig8-1_data_structure_relation.png)

> 图示说明：数据结构关系为：界面表达式 QString 经格式化生成内部 std::string token；词法分析得到 Lexeme；中缀表达式转换为 RPN 列表；计算结果为 long double；绘图坐标为 vector<double> x/y 且 step=0.1；HistoryRecord 保存表达式、x 值、结果和时间；ExportInfo 保存路径、格式、表达式和状态。

**图 8.1 主要数据结构关系图**

## 8.1 逻辑结构设计要点
**表 8.1 逻辑结构设计要点说明**

| **数据项**         | **类型**             | **范围/取值**                           | **说明**                       |
|--------------------|----------------------|-----------------------------------------|--------------------------------|
| 内部表达式         | std::string          | ASCII token                             | 模型层用于解析和计算的表达式。 |
| Lexeme             | class                | value、priority、type                   | 表达式词法单元。               |
| RPN 序列           | std::list\<Lexeme\>  | 由中缀表达式转换得到                    | 栈式求值输入。                 |
| x_value            | double / long double | 约 ±100000000000，最多 6 位小数         | 变量 x 的实际取值。            |
| calculation_result | long double          | 有效数值、NAN、INF                      | 最近一次计算结果。             |
| 绘图坐标           | vector\<double\> x/y | \[x_min,x_max)，step=0.1                | 函数曲线离散点。               |
| HistoryRecord      | 结构化文本/JSON 行   | expression、x_value、result、created_at | 最近 100 条有效计算记录。      |
| ExportInfo         | 结构体/临时对象      | path、format、expression、status        | 图像导出过程信息。             |

## 8.2 物理结构设计要点
历史记录建议采用 JSON Lines 或 CSV 等轻量文本格式，默认文件名为 history.jsonl，可保存在 Qt 标准应用数据目录或程序运行目录下的 data 子目录。每行保存一条记录，便于追加和按行截断。

**表 8.2 物理结构设计要点说明**

| **文件/目录**   | **存放位置**                                          | **访问方式**     | **保密与一致性设计**                         |
|-----------------|-------------------------------------------------------|------------------|----------------------------------------------|
| history.jsonl   | data/history.jsonl 或 QStandardPaths::AppDataLocation | 追加、读取、重写 | 无敏感账号数据；写入失败不影响计算结果显示。 |
| 导出 PNG/PDF    | 用户选择路径                                          | 按需写入         | 导出前检查路径与格式；失败不生成不完整文件。 |
| 资源文件        | Qt qrc 资源                                           | 只读加载         | 随可执行程序打包。                           |
| 测试/覆盖率输出 | gcov_report、临时文件                                 | 开发期生成       | 不随最终应用交付。                           |

# 9 部署与运行目录设计

本章描述 SmartCalc-v2.0 在用户桌面计算机上的部署方式、运行期文件组成以及运行过程中产生的数据文件位置。本章只描述系统交付和运行后的目录结构，不描述源代码目录、构建目录和测试目录；源代码目录与开发环境配置在第 12 章中说明。

## 9.1 部署设计

SmartCalc-v2.0 采用单机桌面应用部署方式。系统部署目标为用户桌面计算机，所有计算、绘图、历史记录管理和图像导出功能均在本地进程内完成，不部署服务器、数据库或网络通信组件。

系统交付内容主要包括 SmartCalc 可执行程序或应用包、Qt 运行库及必要插件、应用资源文件和用户说明文档。历史记录文件和导出图像文件不作为安装包预置内容，而是在用户运行系统并执行相应操作后由系统在本地生成。

**表 9.1 部署设计说明**

| **部署位置**       | **部署内容**                     | **说明**                                                         |
|--------------------|----------------------------------|------------------------------------------------------------------|
| 用户桌面计算机     | SmartCalc 可执行程序或应用包     | 启动主窗口，提供表达式计算、函数绘图、历史记录和图像导出等功能。 |
| 应用安装目录       | Qt 运行库、Qt 插件、应用资源文件 | 支撑 Qt Widgets 界面显示、图像绘制和 PNG/PDF 导出等运行能力。    |
| 用户数据目录       | history.jsonl 等运行期数据文件   | 保存最近有效计算历史记录，由系统运行后按需创建。                 |
| 用户选择的本地路径 | 导出的 PNG/PDF 图像文件          | 用户触发图像导出时生成，不固定存放在安装目录中。                 |

## 9.2 运行目录结构

系统运行目录分为应用安装目录、用户数据目录和用户选择的导出目录三类。应用安装目录用于保存只读的程序和运行依赖；用户数据目录用于保存系统自动生成的历史记录文件；导出目录由用户在导出图像时自行选择。

应用安装目录结构如下：

```text
SmartCalc-v2.0/
├── SmartCalc.exe / SmartCalc.app / SmartCalc  # SmartCalc 主程序或平台应用包
├── QtRuntime/                                 # Qt 运行库，具体形式随平台变化
├── plugins/                                   # Qt 插件目录
│   ├── platforms/                             # 窗口系统适配插件
│   └── imageformats/                          # 图像格式插件
├── resources/                                 # 图标、界面资源等静态文件
├── docs/                                      # 用户说明文档、部署说明等可选文档
└── README.txt                                 # 可选运行说明文件
```

运行期用户数据目录如下：

```text
<QStandardPaths::AppDataLocation>/SmartCalc-v2.0/
└── history.jsonl  # 最近有效计算历史记录，首次保存时自动创建
```

图像导出文件由用户选择保存位置，目录结构不固定。

```text
<用户选择的导出目录>/
├── graph_result.png
└── graph_result.pdf
```

**表 9.2 运行期文件说明**

| **文件或目录**               | **所属位置**               | **访问方式**     | **说明**                                             |
|------------------------------|----------------------------|------------------|------------------------------------------------------|
| SmartCalc 可执行程序或应用包 | 应用安装目录               | 只读加载         | 系统启动入口，负责创建主窗口并进入 Qt 事件循环。     |
| Qt 运行库和插件              | 应用安装目录               | 只读加载         | 支撑 Qt Widgets、图像显示和导出功能。                |
| 应用资源文件                 | 应用安装目录或程序内嵌资源 | 只读加载         | 保存图标、界面资源等静态内容。                       |
| history.jsonl                | 用户数据目录               | 追加、读取、重写 | 保存最近 100 条有效计算历史记录。                    |
| 导出 PNG/PDF 文件            | 用户选择路径               | 按需写入         | 保存当前有效函数图像；导出失败时不影响当前图像显示。 |

该设计将程序文件和运行期数据分离，避免在应用安装目录中写入用户数据，也避免将源代码、构建输出和测试文件混入运行目录。源代码目录、构建脚本和测试目录统一放在第 12 章“开发环境”中说明。

# 10 性能设计
系统以轻量桌面应用为目标。计算复杂度主要与表达式 token 数量线性相关；绘图复杂度与采样点数线性相关，采样点数约为 (x_max - x_min) / 0.1。

**表 10.1 性能设计说明**

| **性能项** | **设计目标**                  | **设计措施**                                       |
|------------|-------------------------------|----------------------------------------------------|
| 表达式计算 | 验收规模内 0.5 秒内显示结果   | RPN 转换与栈式求值线性处理；避免重复解析无关数据。 |
| 键盘输入   | 按键后 0.1 秒内更新显示       | 键盘事件复用按钮输入槽函数，避免额外复杂逻辑。     |
| 默认绘图   | 默认范围约 200 点，1 秒内刷新 | 固定步长 0.1；QCustomPlot 只刷新当前曲线。         |
| 历史记录   | 加载 100 条 0.5 秒内完成      | 使用轻量文本文件，限制记录上限。                   |
| 图像导出   | 默认规模 PNG/PDF 2 秒内完成   | 仅用户触发时写文件，普通绘图不写磁盘。             |
| 资源占用   | 无网络和数据库占用            | 单进程、轻量内存数据结构。                         |

# 11 系统出错处理设计
## 11.1 出错信息
**表 11.1 出错信息设计说明**

| **出错场景**              | **界面提示**           | **处理原则**                                      |
|---------------------------|------------------------|---------------------------------------------------|
| 非法表达式输入            | 保持原表达式或拒绝输入 | 通过 View 层规则防止明显非法 token 进入计算流程。 |
| 计算结果 NAN/INF          | calculation error      | 不保存历史记录，用户可清空或继续输入。            |
| 绘图表达式非法            | invalid input          | 清除无效曲线，保留图像窗口。                      |
| 历史记录写入失败          | 提示保存失败或静默跳过 | 不得影响本次计算结果显示。                        |
| 历史记录读取失败          | 提示读取失败           | 主计算功能继续可用。                              |
| 导出无有效图像            | 导出失败/无有效图像    | 拒绝导出，不生成文件。                            |
| 导出路径不可写/格式不支持 | 导出失败               | 不关闭图像窗口，不清除曲线。                      |

## 11.2 补救措施
用户可通过 C 按钮、Esc 或 Delete 快捷键恢复初始状态。

退格操作删除最近输入的字符或完整 token，并重新计算输入状态。

除零、负数开方、对数定义域错误、取模零等异常统一转换为 calculation error。

文件写入失败时，系统保留当前计算或图像结果，并允许用户重新选择路径。

## 11.3 系统维护设计
核心模型层保持与 GUI 解耦，便于通过 GTest 进行单元测试。

Makefile 提供 check_style、test、gcov_report、dvi、dist 等维护目标。

历史记录和导出模块应独立封装，避免文件逻辑散落在 UI 事件处理函数中。

新增功能需同步补充单元测试或系统测试用例。

## 11.4 错误处理设计
错误处理采用“输入阶段限制 + 计算阶段兜底 + 界面阶段反馈”的三层策略。输入阶段避免明显非法表达式；计算阶段使用 NAN/INF 表示无效结果；界面阶段将无效结果转换为用户可理解的固定提示。

# 12 开发环境
## 12.1 编辑器
可使用 Qt Creator、Visual Studio Code、CLion 或支持 C++17/Qt 的等价编辑器。

## 12.2 编译环境
**表 12.1 编译环境要求**

| **环境项** | **要求**                                              |
|------------|-------------------------------------------------------|
| 语言标准   | C++17                                                 |
| 构建工具   | CMake 3.5+、Makefile                                  |
| 编译器     | g++ 或 clang++，支持 C++17                            |
| GUI 框架   | Qt 6 优先，保留 Qt 5 兼容配置；Widgets + PrintSupport |
| 测试框架   | GTest                                                 |
| 覆盖率工具 | gcov、lcov、genhtml                                   |
| 风格检查   | clang-format，Google 代码风格                         |

## 12.3 配置管理工具
源代码包中包含 Git 仓库信息，采用 Git 进行版本控制。文档、源代码、构建脚本和测试用例应按版本统一归档。

## 12.4 源代码目录
**表 12.1 源代码目录与文件说明**

| **目录/文件**           | **说明**                                            |
|-------------------------|-----------------------------------------------------|
| src/main.cc             | 应用入口，创建 Calculation、Controller 和 View。    |
| src/view                | 主窗口、图像窗口、表达式格式化和 QCustomPlot 集成。 |
| src/controller          | 控制层接口。                                        |
| src/model               | 计算模型、RPN 转换、绘图坐标生成。                  |
| src/model/tests         | 计算模块 GTest 测试用例。                           |
| src/pics、resources.qrc | 图标和 Qt 资源文件。                                |
| src/CMakeLists.txt      | Qt/C++17 构建配置。                                 |
| src/Makefile            | 安装、卸载、测试、覆盖率、文档和发布目标。          |

# 13 尚未解决的问题
**表 13.1 尚未解决的问题说明**

| **序号** | **问题描述**                                                                                     | **影响**                           | **处理方案**                                                                                         |
|----------|--------------------------------------------------------------------------------------------------|------------------------------------|------------------------------------------------------------------------------------------------------|
| 1        | 当前代码包可见实现以按钮输入、计算和绘图为主；键盘输入、历史记录、图像导出需按需求补充编码实现。 | 影响新增正式需求验收。             | 在 View/Graph 层新增键盘事件、历史入口和导出入口；新增 HistoryManager 与 GraphExporter，并补充测试。 |
| 2        | 绘图采样步长固定为 0.1，不适合高精度或大范围曲线分析。                                           | 大范围绘图时性能和曲线平滑度受限。 | 后续引入自适应采样、异常点跳过和后台线程。                                                           |
| 3        | 错误提示目前以固定英文提示为主。                                                                 | 中文用户可理解性有限。             | 后续引入多语言资源和更细分错误提示。                                                                 |
| 4        | 历史记录文件位置和格式需在详细设计阶段固化。                                                     | 影响跨平台路径和测试用例。         | 优先使用 QStandardPaths::AppDataLocation，采用 JSON Lines。                                          |

# 14 用户需求与系统需求规格对照表
**表 14.1 用户需求与系统需求规格对照表**

| **序号** | **系统需求规格**      | **名称/内容概述**      | **软件模块名**                              | **备注**                                                |
|----------|-----------------------|------------------------|---------------------------------------------|---------------------------------------------------------|
| 1        | SC-SRS-1.1.0 ~ 1.11.0 | 表达式输入与编辑       | ViewUI / KeyboardAdapter                    | 覆盖数字、小数、e、运算符、函数、括号、清空、退格。     |
| 2        | SC-SRS-2.1.0 ~ 2.6.0  | 表达式格式化与数学计算 | FormatString / ExpressionModel / Controller | 覆盖格式化、RPN、栈式求值和结果显示。                   |
| 3        | SC-SRS-3.1.0 ~ 3.3.0  | 变量 x 管理            | ViewUI / Controller / Calculation           | 覆盖 x 输入、插入和求值。                               |
| 4        | SC-SRS-4.1.0 ~ 4.4.0  | 函数图像绘制           | Graph / PlotGraph / Controller              | 覆盖打开窗口、范围设置、坐标生成和曲线绘制。            |
| 5        | SC-SRS-5.1.0 ~ 5.2.0  | 界面显示与错误处理     | ViewUI / Graph / ErrorHandler               | 覆盖 display、calculation error、invalid input 和恢复。 |
| 6        | SC-SRS-6.1.0 ~ 6.2.0  | 键盘输入支持           | KeyboardAdapter / ViewUI                    | 复用按钮槽函数。                                        |
| 7        | SC-SRS-7.1.0 ~ 7.3.0  | 计算历史记录管理       | HistoryManager / HistoryStore               | 默认保留最近 100 条。                                   |
| 8        | SC-SRS-8.1.0 ~ 8.2.0  | 函数图像导出           | GraphExporter / Graph                       | 支持 PNG/PDF。                                          |
