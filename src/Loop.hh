#ifndef LOOP_H
#define LOOP_H

#include <clang/AST/AST.h>

#include <string>

/**
 * Plain data object describing a 'for' loop to be reordered by the dynamic
 * optimisation process.
 */
struct Loop {
  /**
   * The clang statement with the loop AST fragment
   */
  const clang::ForStmt *stmt;

  /**
   * The loop induction variable (for example, i in:
   *
   *     for (int i = 0; i < N; i++) {}
   */
  std::string loopVariable;

  /**
   * The loop upper bound (for example, N in:
   *
   *     for (int i = 0; i < N; i++) {}
   *
   * This could be a variable, constant or some other type
   */
  std::string bound;

  Loop(const clang::ForStmt *fs, std::string lv, std::string bo) :
    stmt(fs), loopVariable(lv), bound(bo) {}
};

#endif
