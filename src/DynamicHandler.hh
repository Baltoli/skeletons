#ifndef DYNAMIC_HANDLER_H
#define DYNAMIC_HANDLER_H

#include <clang/ASTMatchers/ASTMatchers.h>
#include <clang/ASTMatchers/ASTMatchFinder.h>

using namespace clang::ast_matchers;

class DynamicHandler : public MatchFinder::MatchCallback {
  public:
    virtual void run(const MatchFinder::MatchResult &Result);
    StatementMatcher matcher();
};

#endif
