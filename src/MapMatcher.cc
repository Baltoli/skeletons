#include "MapMatcher.hh"

#include <llvm/Support/raw_ostream.h>

using namespace clang;
using namespace clang::ast_matchers;

MapHandler::MapHandler() {}

void MapHandler::run(const MatchFinder::MatchResult &Result) {
  if(const ForStmt *forS = Result.Nodes.getNodeAs<ForStmt>("for")) {
    auto loc = Result.Context->getFullLoc(forS->getLocStart());

    llvm::errs() << "[map] " 
                 << "line "
                 << loc.getExpansionLineNumber() 
                 << ", column "
                 << loc.getExpansionColumnNumber()
                 << '\n';
  }
}

StatementMatcher MapHandler::matcher() {
  return (
    forStmt(
      hasLoopInit(loopInitMatcher()),
      hasCondition(loopConditionMatcher()),
      hasIncrement(loopIncrementMatcher())
    ).bind("for")
  );
}

StatementMatcher MapHandler::loopInitMatcher() {
  return (
    declStmt(hasSingleDecl(
      varDecl(
        hasInitializer(ignoringParenImpCasts(integerLiteral(equals(0))))
      ).bind("initVar")
    ))
  );
}

StatementMatcher MapHandler::loopConditionMatcher() {
  return (
    binaryOperator(
      hasOperatorName("<"),
      hasLHS(ignoringParenImpCasts(
        declRefExpr(to(varDecl(hasType(isInteger())).bind("condVar")))
      )),
      hasRHS(ignoringParenImpCasts(
        expr(hasType(isInteger()))
      ))
    )
  );
}

StatementMatcher MapHandler::loopIncrementMatcher() {
  return (
    unaryOperator(
      hasOperatorName("++"),
      hasUnaryOperand(ignoringParenImpCasts(
        declRefExpr(to(varDecl(hasType(isInteger())).bind("incVar")))
      ))
    )
  );
}
