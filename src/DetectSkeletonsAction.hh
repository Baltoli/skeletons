#ifndef DETECT_SKELETONS_HH
#define DETECT_SKELETONS_HH

#include <string>

#include <clang/Frontend/FrontendActions.h>
#include <clang/AST/ASTConsumer.h>

using namespace llvm;
using namespace clang;
using std::unique_ptr;

class DetectSkeletonsAction : public ASTFrontendAction {
  public:
    DetectSkeletonsAction();
    unique_ptr<ASTConsumer> CreateASTConsumer(CompilerInstance &CI, StringRef InFile);
};

#endif
