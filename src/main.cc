#include <iostream>

#include <clang/Tooling/Tooling.h>
#include <clang/Tooling/CommonOptionsParser.h>
#include <llvm/Support/CommandLine.h>

#include "DetectSkeletonsAction.hh"
#include "AnnotateSkeletonsAction.hh"
#include "Log.hh"

using namespace llvm;
using namespace clang;
using namespace clang::tooling;
using std::string;

static cl::OptionCategory OptionsCategory("skel options");
static cl::opt<bool> Overwrite("overwrite", cl::desc("Overwrite source files"), 
                               cl::cat(OptionsCategory));
static cl::alias OverwriteA("w", cl::desc("Alias for -overwrite"), cl::aliasopt(Overwrite));

int main(int argc, const char **argv) {
  CommonOptionsParser op(argc, argv, OptionsCategory);
  ClangTool Tool(op.getCompilations(), op.getSourcePathList());
  
  if(Overwrite) {
    return Tool.run(newFrontendActionFactory<AnnotateSkeletonsAction>().get());
  } else {
    return Tool.run(newFrontendActionFactory<DetectSkeletonsAction>().get());
  }
}
