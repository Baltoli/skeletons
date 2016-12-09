#include "MapMatcher.hh"

#include <vector>
#include <algorithm>
#include <sstream>

#include <llvm/Support/raw_ostream.h>

#include "Log.hh"

using std::vector;
using std::string;
using namespace clang;
using namespace clang::ast_matchers;

namespace {

static string getDeclName(const MatchFinder::MatchResult &Result, string binding) {
  auto decl = Result.Nodes.getNodeAs<VarDecl>(binding);
  if(decl) {
    return decl->getNameAsString();
  } else {
    return string("");
  }
}

static bool allEqual(vector<string> ss) {
  if(ss.size() < 1) {
    return true;
  }

  auto first = *ss.begin();
  return std::all_of(ss.begin() + 1, ss.end(), [&](string s) { return s == first; });
}

}

MapHandler::MapHandler() {}

void MapHandler::run(const MatchFinder::MatchResult &Result) {
  log(Debug, "Handling Map match result");

  if(const ForStmt *forS = Result.Nodes.getNodeAs<ForStmt>("for")) {
    vector<string> bindings = { "initVar", "condVar", "incVar" };

    std::transform(
      bindings.begin(), bindings.end(), bindings.begin(), 
      [&](string s) { return getDeclName(Result, s); }
    );

    auto all_equal = allEqual(bindings);
    if(!all_equal) {
      log(Debug, "Near miss for Map: "
                 "For loop referencing different variables");
      return;
    }

    auto loc = Result.Context->getFullLoc(forS->getLocStart());
    log(Output, successOutputMessage(loc));
  }
}

StatementMatcher MapHandler::matcher() {
  return (
    forStmt(
      hasLoopInit(loopInitMatcher()),
      hasCondition(loopConditionMatcher()),
      hasIncrement(loopIncrementMatcher())
    ).bind("for")
  );
}

StatementMatcher MapHandler::loopInitMatcher() {
  return (
    declStmt(hasSingleDecl(
      varDecl(
        hasInitializer(ignoringParenImpCasts(integerLiteral(equals(0))))
      ).bind("initVar")
    ))
  );
}

StatementMatcher MapHandler::loopConditionMatcher() {
  return (
    binaryOperator(
      hasOperatorName("<"),
      hasLHS(ignoringParenImpCasts(
        declRefExpr(to(varDecl(hasType(isInteger())).bind("condVar")))
      )),
      hasRHS(ignoringParenImpCasts(
        expr(hasType(isInteger()))
      ))
    )
  );
}

StatementMatcher MapHandler::loopIncrementMatcher() {
  return (
    unaryOperator(
      hasOperatorName("++"),
      hasUnaryOperand(ignoringParenImpCasts(
        declRefExpr(to(varDecl(hasType(isInteger())).bind("incVar")))
      ))
    )
  );
}

string MapHandler::successOutputMessage(FullSourceLoc loc) {
  std::stringstream out;
  out << "Found Map at: " 
      << "line "
      << loc.getExpansionLineNumber() 
      << ", column "
      << loc.getExpansionColumnNumber();
  return out.str();
}
