#ifndef DETECT_SKELETONS_AST_CONSUMER_H
#define DETECT_SKELETONS_AST_CONSUMER_H

#include <clang/AST/ASTConsumer.h>

using namespace clang;
  
class DetectSkeletonsASTConsumer : public ASTConsumer {
  public:
    void HandleTranslationUnit(ASTContext &context);
};

#endif
