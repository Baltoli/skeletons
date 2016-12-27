#ifndef DETECT_DYNAMIC_HH
#define DETECT_DYNAMIC_HH

#include <string>

#include <clang/Frontend/FrontendActions.h>
#include <clang/AST/ASTConsumer.h>

using namespace llvm;
using namespace clang;
using std::unique_ptr;

class DetectDynamicAction : public ASTFrontendAction {
  public:
    DetectDynamicAction();
    unique_ptr<ASTConsumer> CreateASTConsumer(CompilerInstance &CI, StringRef InFile);
};

#endif
