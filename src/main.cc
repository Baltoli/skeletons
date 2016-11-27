#include <clang/Tooling/Tooling.h>
#include <clang/Tooling/CommonOptionsParser.h>

using namespace clang;
using namespace clang::driver;
using namespace clang::tooling;

static llvm::cl::OptionCategory ToolingSampleCategory("Tooling Sample");

int main(int argc, const char **argv) {
  CommonOptionsParser op(argc, argv, ToolingSampleCategory);
  return 0;
}
