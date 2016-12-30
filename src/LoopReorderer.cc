#include "Log.hh"
#include "LoopReorderer.hh"

#include <sstream>

using std::string;
using std::stringstream;
using namespace clang;
using namespace llvm;

string LoopReorderer::code(const Stmt *stmt) {
  string out;
  raw_string_ostream stream(out);

  if(stmt) {
    stmt->printPretty(stream, nullptr, PrintingPolicy(context.getLangOpts()));
  }

  return out;
}

string LoopReorderer::transform(Loop loop) {
  switch(strategy) {
    case Strategy::Reverse:
      return reverse(loop);
    default:
      return identity(loop);
  }
}

string LoopReorderer::identity(Loop loop) {
  auto stmt = loop.stmt;
  stringstream st;

  st << "for ("
     << code(stmt->getInit()) << "; "
     << code(stmt->getCond()) << "; "
     << code(stmt->getInc()) << ") "
     << code(stmt->getBody());
  return st.str();
}

string LoopReorderer::reverse(Loop loop) {
  stringstream st;
  st << "for ("
     << (loop.declared ? "int " : "")
     << loop.var << " = "
     << code(loop.bound) << " - " 
     << reverseBound(comparisonOp(loop.stmt->getCond())) << "; "
     << loop.var << reverseComparison(comparisonOp(loop.stmt->getCond())) 
     << code(loop.init) << "; "
     << loop.var << "--)"
     << code(loop.stmt->getBody());
  return st.str();
}

string LoopReorderer::comparisonOp(const Expr *e) {
  auto binop = dyn_cast<BinaryOperator>(e);
  return BinaryOperator::getOpcodeStr(binop->getOpcode());
}

int LoopReorderer::reverseBound(string cmp) {
  if(cmp == "<") return 1;
  if(cmp == "<=") return 0;

  log(Debug, "Unknown comparison operator when reversing loop: " + cmp);

  return 0;
}

string LoopReorderer::reverseComparison(string cmp) {
  // TODO: implement other direction
  return ">=";
}
