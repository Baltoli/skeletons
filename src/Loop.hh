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
   * The loop induction variable. For example, i in:
   *
   *     for (int i = 0; i < N; i++) {}
   */
  std::string var;

  /**
   * The initializer for the loop induction variable. For example, 0 in:
   * 
   *     for (int i = 0; i < N; i++) {}
   */
  const clang::Stmt *init;

  /**
   * The loop upper bound. For example, N in:
   *
   *     for (int i = 0; i < N; i++) {}
   *
   * This could be a variable, constant or some other type
   */
  const clang::Stmt *bound;

  Loop(const clang::ForStmt *fs, 
       std::string v, 
       const clang::Stmt *i, 
       const clang::Stmt *b) :
    stmt(fs), var(v), init(i), bound(b) {}
};

#endif
