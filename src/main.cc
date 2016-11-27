#include <iostream>
#include <string>

#include <clang/Tooling/Tooling.h>
#include <clang/Tooling/CommonOptionsParser.h>
#include <llvm/Support/CommandLine.h>

using namespace llvm;
using namespace clang::tooling;
using std::string;

static cl::OptionCategory OptionsCategory("Options");

int main(int argc, const char **argv) {
  CommonOptionsParser OptionsParser(argc, argv, OptionsCategory);
  for(auto s : OptionsParser.getSourcePathList()) {
    std::cout << s << std::endl;
  }
  return 0;
}
