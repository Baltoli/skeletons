#include <iostream>

#include <clang/Tooling/Tooling.h>
#include <clang/Tooling/CommonOptionsParser.h>
#include <llvm/Support/CommandLine.h>

#include "DetectSkeletonsAction.hh"
#include "Log.hh"

using namespace llvm;
using namespace clang;
using namespace clang::tooling;
using std::string;

static cl::OptionCategory OptionsCategory("skel options");

int main(int argc, const char **argv) {
  CommonOptionsParser op(argc, argv, OptionsCategory);
  ClangTool Tool(op.getCompilations(), op.getSourcePathList());
  return Tool.run(newFrontendActionFactory<DetectSkeletonsAction>().get());
}
