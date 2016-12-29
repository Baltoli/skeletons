#include "LoopReorderer.hh"

using std::string;
using namespace clang;
using namespace llvm;

string LoopReorderer::code(const Stmt *stmt) {
  string out;
  raw_string_ostream stream(out);
  stmt->printPretty(stream, nullptr, PrintingPolicy(context.getLangOpts()));
  return out;
}

string LoopReorderer::transform(const ForStmt *stmt) {
  return code(stmt);
}

string LoopReorderer::reverse(const ForStmt *stmt) {
  return "";
}
