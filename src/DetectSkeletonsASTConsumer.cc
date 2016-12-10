#include <iostream>

#include "DetectSkeletonsASTConsumer.hh"

DetectSkeletonsASTConsumer::DetectSkeletonsASTConsumer(bool overwrite) :
  mapHandler(overwrite)
{
  Matcher.addMatcher(mapHandler.matcher(), &mapHandler);
}

void DetectSkeletonsASTConsumer::HandleTranslationUnit(ASTContext &context) {
  Matcher.matchAST(context);
}
