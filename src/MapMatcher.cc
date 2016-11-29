#include "MapMatcher.hh"

using namespace clang;
using namespace clang::ast_matchers;

MapHandler::MapHandler() {}

void MapHandler::run(const MatchFinder::MatchResult &Result) {
  if(const ForStmt *forS = Result.Nodes.getNodeAs<ForStmt>("for")) {
    forS->dump();
  }
}

StatementMatcher MapHandler::matcher() {
  return(
    forStmt(
      hasLoopInit(loopInitMatcher())
    ).bind("for")
  );
}

StatementMatcher MapHandler::loopInitMatcher() {
  return (
    declStmt(hasSingleDecl(varDecl(
      hasInitializer(ignoringParenImpCasts(integerLiteral(equals(0)))
    ))))
  );
}
