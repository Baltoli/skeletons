#include "LoopReorderer.hh"

#include <sstream>

using std::string;
using std::stringstream;
using namespace clang;
using namespace llvm;

string LoopReorderer::code(const Stmt *stmt) {
  string out;
  raw_string_ostream stream(out);
  stmt->printPretty(stream, nullptr, PrintingPolicy(context.getLangOpts()));
  return out;
}

string LoopReorderer::transform(const ForStmt *stmt) {
  switch(strategy) {
    case Strategy::Reverse:
      return reverse(stmt);
    default:
      return identity(stmt);
  }
}

string LoopReorderer::identity(const ForStmt *stmt) {
  stringstream st;
  st << "for ("
     << code(stmt->getInit())
     << code(stmt->getCond()) << "; "
     << code(stmt->getInc()) << ") "
     << code(stmt->getBody());
  return st.str();
}

string LoopReorderer::reverse(const ForStmt *stmt) {
  return "";
}
