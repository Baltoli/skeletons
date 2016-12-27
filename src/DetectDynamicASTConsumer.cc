#include "DetectDynamicASTConsumer.hh"

DetectDynamicASTConsumer::DetectDynamicASTConsumer()
{
  Matcher.addMatcher(handler.matcher(), &handler);
}

void DetectDynamicASTConsumer::HandleTranslationUnit(ASTContext &context) {
  Matcher.matchAST(context);
}
