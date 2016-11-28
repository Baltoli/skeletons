#include <iostream>
#include <string>

#include <clang/Tooling/Tooling.h>
#include <clang/Tooling/CommonOptionsParser.h>
#include <clang/Frontend/FrontendActions.h>
#include <clang/AST/ASTConsumer.h>
#include <clang/AST/RecursiveASTVisitor.h>
#include <llvm/Support/CommandLine.h>

using namespace llvm;
using namespace clang;
using namespace clang::tooling;
using std::string;

class TestClassVisitor : public RecursiveASTVisitor<TestClassVisitor> {
  public:
    bool VisitDecl(Decl *d) {
      d->dump();
      return true;
    }
};

class MyASTConsumer : public ASTConsumer {
  public:
    void HandleTranslationUnit(ASTContext &context) {
      std::cout << "fiw" << std::endl;
      TestClassVisitor().TraverseDecl(context.getTranslationUnitDecl());
    }
};

class MyFrontendAction : public ASTFrontendAction {
  public:
    MyFrontendAction() {}

    std::unique_ptr<ASTConsumer> CreateASTConsumer(CompilerInstance &CI, StringRef InFile) {
      std::cout << InFile.str() << std::endl;
      return std::unique_ptr<ASTConsumer>(new MyASTConsumer());
    }
};

static cl::OptionCategory OptionsCategory("skel options");

int main(int argc, const char **argv) {
  CommonOptionsParser op(argc, argv, OptionsCategory);
  ClangTool Tool(op.getCompilations(), op.getSourcePathList());
  return Tool.run(newFrontendActionFactory<MyFrontendAction>().get());
}
