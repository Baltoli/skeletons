#include <iostream>

#include <clang/Tooling/Tooling.h>
#include <clang/Tooling/CommonOptionsParser.h>
#include <llvm/Support/CommandLine.h>

#include "AnnotateSkeletonsAction.hh"
#include "DetectSkeletonsAction.hh"
#include "DetectDynamicAction.hh"
#include "Flags.hh"
#include "Log.hh"
#include "LoopReorderer.hh"

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

static cl::opt<ProgramMode> Mode(cl::desc("Application mode"),
    cl::values(
      clEnumValN(Static, "static", "Statically analyse the program"),
      clEnumValN(FindReorderable, "reorder", "Dynamically find reorderable loops"),
      nullptr
    ), cl::cat(OptionsCategory), cl::Required);

Strategy::Type StrategyFlag;
static cl::opt<Strategy::Type, true> StrategyType(cl::desc("Reordering strategy"),
    cl::values(
      clEnumValN(Strategy::Reverse, "reverse", "Reverse the order of loop iteration"),
      nullptr
    ), 
    cl::location(StrategyFlag), cl::init(Strategy::Identity), cl::cat(OptionsCategory));

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
