#include "DynamicHandler.hh"
#include "Log.hh"

void DynamicHandler::run(const MatchFinder::MatchResult &Result) {
  log(Debug, "Handling possible reorderable loop");
}

StatementMatcher DynamicHandler::matcher() {
  return forLoopMatcher();
}

StatementMatcher DynamicHandler::forLoopMatcher() {
  return forStmt(
    hasLoopInit(initMatcher()),
    hasCondition(conditionMatcher()),
    hasIncrement(incrementMatcher())
  ).bind("for");
}

StatementMatcher DynamicHandler::initMatcher() {
  return (
    declStmt(hasSingleDecl(
      varDecl(
        hasInitializer(ignoringParenImpCasts(integerLiteral()))
      ).bind("initVar")
    ))
  );
}

StatementMatcher DynamicHandler::conditionMatcher() {
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

StatementMatcher DynamicHandler::incrementMatcher() {
  return (
    unaryOperator(
      hasOperatorName("++"),
      hasUnaryOperand(ignoringParenImpCasts(
        declRefExpr(to(varDecl(hasType(isInteger())).bind("incVar")))
      ))
    )
  );
}
