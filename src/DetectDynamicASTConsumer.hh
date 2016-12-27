#ifndef DETECT_DYNAMIC_AST_CONSUMER_H
#define DETECT_DYNAMIC_AST_CONSUMER_H

#include <clang/AST/ASTConsumer.h>

#include "MapMatcher.hh"

using namespace clang;
class DetectDynamicASTConsumer : public ASTConsumer {
  public:
    DetectDynamicASTConsumer();
    void HandleTranslationUnit(ASTContext &context);
};

#endif
