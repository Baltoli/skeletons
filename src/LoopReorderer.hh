#ifndef LOOP_REORDERER_H
#define LOOP_REORDERER_H

#include <string>

#include <clang/AST/AST.h>
#include <llvm/Support/raw_ostream.h>

#include "Loop.hh"

struct Strategy {
  public:
    enum Type {
      Identity,
      Reverse
    };

    Type t_;
    Strategy(Type t) : t_(t) {}
    operator Type () const {return t_;}
  private:
    template <typename T>
      operator T () const;
};

class LoopReorderer {
  public:
    LoopReorderer(Strategy s, clang::ASTContext &c) : 
      strategy(s), context(c) {}
    std::string transform(Loop loop);
  private:
    Strategy strategy;
    clang::ASTContext &context;
    std::string code(const clang::Stmt *stmt);
    std::string identity(Loop loop);
    std::string reverse(Loop loop);
    std::string comparisonOp(const clang::Expr *e);
    int reverseBound(std::string cmp);
    std::string reverseComparison(std::string cmp);
};

#endif
