#include "DynamicHandler.hh"

void DynamicHandler::run(const MatchFinder::MatchResult &Result) {
}

StatementMatcher DynamicHandler::matcher() {
  return stmt();
}
