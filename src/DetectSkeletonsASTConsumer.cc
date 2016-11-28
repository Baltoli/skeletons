#include <iostream>

#include "DetectSkeletonsASTConsumer.hh"
#include "MapVisitor.hh"

using namespace clang::ast_matchers;

ForLoopHandler::ForLoopHandler() {}

void ForLoopHandler::run(const MatchFinder::MatchResult &Result) {
}

DetectSkeletonsASTConsumer::DetectSkeletonsASTConsumer() {
  Matcher.addMatcher(forStmt(), &ForHandler);
}

void DetectSkeletonsASTConsumer::HandleTranslationUnit(ASTContext &context) {
  Matcher.matchAST(context);
}
