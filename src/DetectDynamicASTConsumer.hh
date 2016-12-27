#ifndef DETECT_DYNAMIC_AST_CONSUMER_H
#define DETECT_DYNAMIC_AST_CONSUMER_H

#include <clang/AST/ASTConsumer.h>
#include <clang/ASTMatchers/ASTMatchers.h>
#include <clang/ASTMatchers/ASTMatchFinder.h>

#include "DynamicHandler.hh"

using namespace clang;
using namespace clang::ast_matchers;

class DetectDynamicASTConsumer : public ASTConsumer {
  public:
    DetectDynamicASTConsumer();
    void HandleTranslationUnit(ASTContext &context);
  private:
    DynamicHandler handler;
    MatchFinder Matcher;
};

#endif
