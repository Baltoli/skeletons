#include <iostream>

#include <clang/Tooling/Tooling.h>
#include <clang/Tooling/CommonOptionsParser.h>
#include <llvm/Support/CommandLine.h>

#include "DetectSkeletonsAction.hh"
#include "DetectDynamicAction.hh"
#include "AnnotateSkeletonsAction.hh"
#include "Log.hh"

using namespace llvm;
using namespace clang;
using namespace clang::tooling;
using std::string;

enum ProgramMode {
  Static,
  FindReorderable
};

static cl::OptionCategory OptionsCategory("skel options");
static cl::opt<bool> Overwrite("overwrite", cl::desc("Overwrite source files"), 
                               cl::cat(OptionsCategory));
static cl::alias OverwriteA("w", cl::desc("Alias for -overwrite"), cl::aliasopt(Overwrite));

static cl::opt<ProgramMode> Mode(cl::desc("choose application mode"),
    cl::values(
      clEnumVal(Static, "Statically analyse the program"),
      clEnumVal(FindReorderable, "Dynamically find reorderable loops"),
      nullptr
    ), cl::cat(OptionsCategory), cl::Required);

int main(int argc, const char **argv) {
  CommonOptionsParser op(argc, argv, OptionsCategory);
  ClangTool Tool(op.getCompilations(), op.getSourcePathList());
  
  if(Mode == Static) {
    if(Overwrite) {
      return Tool.run(
          newFrontendActionFactory<AnnotateSkeletonsAction>().get());
    } else {
      return Tool.run(
          newFrontendActionFactory<DetectSkeletonsAction>().get());
    }
  }

  return Tool.run(newFrontendActionFactory<DetectDynamicAction>().get());
}
