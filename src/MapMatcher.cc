#include "MapMatcher.hh"

#include <vector>
#include <algorithm>
#include <sstream>

#include <llvm/Support/raw_ostream.h>
#include <clang/Rewrite/Core/Rewriter.h>
#include <clang/Basic/LangOptions.h>

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
    
    addParallelAnnotation(forS->getLocStart(), Result);
  }
}

void MapHandler::addParallelAnnotation(SourceLocation loc, 
                                       const MatchFinder::MatchResult &Result)
{
  // TODO: Is an empty LangOptions OK here? My instinct is that it probably is
  // as long as we are *only* doing simple insertion of text into the source
  // file.
  Rewriter r(*Result.SourceManager, LangOptions());
  r.InsertText(loc, "// Hello there\n", false, true);
  r.overwriteChangedFiles();
}

StatementMatcher MapHandler::matcher() {
  return (
    forStmt(
      hasLoopInit(loopInitMatcher()),
      hasCondition(loopConditionMatcher()),
      hasIncrement(loopIncrementMatcher()),
      hasBody(bodyMatcher())
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

StatementMatcher MapHandler::bodyMatcher() {
  // TODO: what do we want to match here? and will it need further verification?
  // Things to think about:
  //  - Easiest case is DAXPY style array motion calling a single function.
  //  - From there need to check that the output array is not modified after the
  //    assignment is made (as otherwise the map properties might not be quite
  //    right.
  //  - Extension: how could it be possible to generalise from a direct function
  //    call to allowing previous work done to be used? Halfway step could be to
  //    allow anything that doesn't mutate the input or output array (same
  //    afterwards?)
  return (
    stmt()
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
