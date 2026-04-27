/**
 * @file main.cc
 * @brief Application entry point for the SmartCalc desktop program.
 */

#include <QApplication>

#include "controller/controller.h"
#include "view/view.h"

/**
 * @brief Creates the MVC components, configures the main window, and starts
 * the Qt event loop.
 * @param argc Number of command-line arguments.
 * @param argv Array of command-line argument strings.
 * @return Exit code returned by the Qt application event loop.
 */
int main(int argc, char *argv[]) {
  s21::Calculation calculation;
  s21::Controller controller(&calculation);

  QApplication a(argc, argv);
  a.setWindowIcon(QIcon(":/pics/calculator_icon.png"));
  View view(nullptr, &controller);
  view.show();
  view.setWindowTitle("SmartCalc");

  return a.exec();
}
