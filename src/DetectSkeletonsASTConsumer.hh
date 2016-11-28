#ifndef DETECT_SKELETONS_AST_CONSUMER_H
#define DETECT_SKELETONS_AST_CONSUMER_H

#include <clang/AST/ASTConsumer.h>
#include <clang/ASTMatchers/ASTMatchers.h>
#include <clang/ASTMatchers/ASTMatchFinder.h>

using namespace clang;
using namespace clang::ast_matchers;

class ForLoopHandler : public MatchFinder::MatchCallback {
  public:
    ForLoopHandler();
    virtual void run(const MatchFinder::MatchResult &Result);
};
  
class DetectSkeletonsASTConsumer : public ASTConsumer {
  public:
    DetectSkeletonsASTConsumer();
    void HandleTranslationUnit(ASTContext &context);

  private:
    MatchFinder Matcher;
    ForLoopHandler ForHandler;
};

#endif
