#ifndef DETECT_SKELETONS_AST_CONSUMER_H
#define DETECT_SKELETONS_AST_CONSUMER_H

#include <clang/AST/ASTConsumer.h>

#include "MapMatcher.hh"

using namespace clang;
class DetectSkeletonsASTConsumer : public ASTConsumer {
  public:
    DetectSkeletonsASTConsumer(bool overwrite);
    void HandleTranslationUnit(ASTContext &context);

  private:
    MatchFinder Matcher;
    MapHandler mapHandler;
};

#endif
