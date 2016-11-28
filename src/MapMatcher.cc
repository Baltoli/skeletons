#include "MapMatcher.hh"

using namespace clang::ast_matchers;

MapHandler::MapHandler() {}

void MapHandler::run(const MatchFinder::MatchResult &Result) {
}

StatementMatcher MapHandler::matcher() {
  return forStmt().bind("for");
}
