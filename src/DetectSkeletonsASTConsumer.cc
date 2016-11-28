#include "DetectSkeletonsASTConsumer.hh"
#include "MapVisitor.hh"

void DetectSkeletonsASTConsumer::HandleTranslationUnit(ASTContext &context) {
  MapVisitor().TraverseDecl(context.getTranslationUnitDecl());
}
