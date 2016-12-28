#ifndef LOOP_REORDERER_H
#define LOOP_REORDERER_H

#include <string>

#include <clang/AST/AST.h>

struct Strategy {
  public:
    enum Type {
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
    std::string transform(const clang::ForStmt *stmt);
  private:
    Strategy strategy;
    clang::ASTContext &context;
};

#endif
