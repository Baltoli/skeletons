#include <iostream>

#include "DetectSkeletonsASTConsumer.hh"

DetectSkeletonsASTConsumer::DetectSkeletonsASTConsumer() {
  Matcher.addMatcher(mapHandler.matcher(), &mapHandler);
}

void DetectSkeletonsASTConsumer::HandleTranslationUnit(ASTContext &context) {
  Matcher.matchAST(context);
}
