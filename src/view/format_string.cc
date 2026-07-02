/**
 * @file format_string.cc
 * @brief Implements conversion of UI expressions into parser-friendly strings.
 */

#include "format_string.h"

namespace s21 {

/**
 * @brief Converts the UI expression into a compact internal representation.
 * @details Replaces multi-character function names with single-character
 * markers expected by the parser while preserving numbers, operators, and the
 * variable `x`. In particular, `ln` becomes `l`, `log` becomes `L`,
 * `sin/cos/tan` remain lowercase single-letter markers, and
 * `asin/acos/atan` are converted to uppercase `S/C/T`.
 */
void FormatString::Convert() {
  QString::iterator it = q_str_.begin();
  size_t iter_move = 0;

  while (it != q_str_.end()) {
    iter_move = 1;

    if (*it == 'l') {
      // Distinguish `ln` from `log`: both start with `l`, but the parser uses
      // different one-letter tokens for them.
      it++;

      if (*it == 'n') {
        basic_str_ += "l";

      } else {
        basic_str_ += "L";
        iter_move = 2;
      }

    } else if (*it == 'a') {
      // `asin`, `acos`, and `atan` are encoded as uppercase one-letter parser
      // tokens `S`, `C`, and `T`.
      it++;

      basic_str_ += static_cast<char>(it->unicode() - 32);
      iter_move = 3;

    } else if (*it == 's' || *it == 'c' || *it == 't') {
      // `sin`, `cos`, and `tan` keep their lowercase initial as the compact
      // parser token.
      basic_str_ += static_cast<char>(it->unicode());
      iter_move = 3;

    } else {
      // Numbers, operators, parentheses, and `x` are copied unchanged.
      basic_str_ += static_cast<char>(it->unicode());
    }

    it += iter_move;
  }
}

}  // namespace s21
