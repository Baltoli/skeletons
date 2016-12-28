#include "Common.hh"
#include "DynamicHandler.hh"
#include "Log.hh"

#include <clang/Rewrite/Core/Rewriter.h>

using namespace clang;
using namespace clang::ast_matchers;

void DynamicHandler::run(const MatchFinder::MatchResult &Result) {
  log(Debug, "Handling possible reorderable loop");

  if(const ForStmt *forS = Result.Nodes.getNodeAs<ForStmt>("for")) {
    vector<string> bindings = { "initVar", "condVar", "incVar" };

    std::transform(
      bindings.begin(), bindings.end(), bindings.begin(), 
      [&](string s) { 
        return getDeclName(Result, s); 
      }
    );

    auto all_equal = allEqual(bindings);
    if(!all_equal) {
      log(Debug, "Near miss for Reorderable Loop: "
                 "For loop referencing different variables");
      return;
    }

    log(Debug, "Found reorderable loop");

    Rewriter r(*Result.SourceManager, LangOptions());
    //r.ReplaceText(forS->getSourceRange(), "");
    r.overwriteChangedFiles();
  }
}

StatementMatcher DynamicHandler::matcher() {
  return forLoopMatcher();
}

StatementMatcher DynamicHandler::forLoopMatcher() {
  return forStmt(
    hasLoopInit(initMatcher()),
    hasCondition(conditionMatcher()),
    hasIncrement(incrementMatcher())
  ).bind("for");
}

StatementMatcher DynamicHandler::initMatcher() {
  return (
    anyOf(
      declStmt(hasSingleDecl(
        varDecl(
          hasInitializer(ignoringParenImpCasts(integerLiteral()))
        ).bind("initVar")
      )),
      binaryOperator(
        hasOperatorName("="),
        hasLHS(ignoringParenImpCasts(
          declRefExpr(to(varDecl(hasType(isInteger())).bind("initVar")))
        )),
        hasRHS(ignoringParenImpCasts(integerLiteral()))
      )
    )
  );
}

StatementMatcher DynamicHandler::conditionMatcher() {
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

StatementMatcher DynamicHandler::incrementMatcher() {
  return (
    unaryOperator(
      hasOperatorName("++"),
      hasUnaryOperand(ignoringParenImpCasts(
        declRefExpr(to(varDecl(hasType(isInteger())).bind("incVar")))
      ))
    )
  );
}
