#ifndef LOOP_REORDERER_H
#define LOOP_REORDERER_H

#include <string>

#include <clang/AST/AST.h>
#include <llvm/Support/raw_ostream.h>

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
    std::string transform(const clang::ForStmt *stmt);
  private:
    Strategy strategy;
    clang::ASTContext &context;
    std::string code(const clang::Stmt *stmt);
    std::string identity(const clang::ForStmt *stmt);
    std::string reverse(const clang::ForStmt *stmt);
};

#endif
