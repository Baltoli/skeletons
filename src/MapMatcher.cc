#include "MapMatcher.hh"

#include <llvm/Support/raw_ostream.h>

using namespace clang;
using namespace clang::ast_matchers;

MapHandler::MapHandler() {}

void MapHandler::run(const MatchFinder::MatchResult &Result) {
  if(const ForStmt *forS = Result.Nodes.getNodeAs<ForStmt>("for")) {
    forS->dumpPretty(*Result.Context);
    forS->dumpColor();
    llvm::errs() << '\n';
  }
}

StatementMatcher MapHandler::matcher() {
  return(
    forStmt(
      hasLoopInit(loopInitMatcher()),
      hasCondition(loopConditionMatcher()),
      hasIncrement(loopIncrementMatcher())
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

StatementMatcher MapHandler::loopConditionMatcher() {
  return expr();
}

StatementMatcher MapHandler::loopIncrementMatcher() {
  return stmt();
}
