#ifndef DETECT_DYNAMIC_HH
#define DETECT_DYNAMIC_HH

#include <string>

#include <clang/AST/ASTConsumer.h>
#include <clang/Frontend/FrontendActions.h>

using namespace llvm;
using namespace clang;
using std::unique_ptr;

class DetectDynamicAction : public ASTFrontendAction {
  public:
    DetectDynamicAction();
    unique_ptr<ASTConsumer> CreateASTConsumer(CompilerInstance &CI, StringRef InFile);
};

#endif
