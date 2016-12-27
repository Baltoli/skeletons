#include "DynamicHandler.hh"

void DynamicHandler::run(const MatchFinder::MatchResult &Result) {
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
  return stmt();
}

StatementMatcher DynamicHandler::conditionMatcher() {
  return expr();
}

StatementMatcher DynamicHandler::incrementMatcher() {
  return stmt();
}
