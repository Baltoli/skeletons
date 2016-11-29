#ifndef MAP_MATCHER_H
#define MAP_MATCHER_H

#include <clang/ASTMatchers/ASTMatchers.h>
#include <clang/ASTMatchers/ASTMatchFinder.h>

using namespace clang::ast_matchers;

class MapHandler : public MatchFinder::MatchCallback {
  public:
    MapHandler();
    virtual void run(const MatchFinder::MatchResult &Result);

    StatementMatcher matcher();

  private:
    StatementMatcher loopInitMatcher();
};
  

#endif
