#include "Common.hh"
#include "DynamicHandler.hh"
#include "Flags.hh"
#include "Log.hh"
#include "Loop.hh"
#include "LoopReorderer.hh"

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

    const Stmt *init = Result.Nodes.getNodeAs<Stmt>("init");
    if(!init) {
      log(Debug, "No initialiser for reorderable loop");
      return;
    }

    const Stmt *bound = Result.Nodes.getNodeAs<Stmt>("bound");
    if(!bound) {
      log(Debug, "No bound for reorderable loop");
      return;
    }

    const DeclStmt *declared = Result.Nodes.getNodeAs<DeclStmt>("declared");

    log(Debug, "Found reorderable loop");

    Rewriter r(*Result.SourceManager, LangOptions());
    LoopReorderer lro(StrategyFlag, *Result.Context);
    Loop loop(forS, bindings[0], declared != nullptr, init, bound);
    string newSource = lro.transform(loop);

    r.ReplaceText(forS->getSourceRange(), newSource);
    r.overwriteChangedFiles();

    llvm::outs() 
      << Result.Context->getFullLoc(forS->getLocStart()).getSpellingLineNumber() 
      << '\n';

    exit(0);
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
          hasInitializer(ignoringParenImpCasts(integerLiteral().bind("init")))
        ).bind("initVar")
      )).bind("declared"),
      binaryOperator(
        hasOperatorName("="),
        hasLHS(ignoringParenImpCasts(
          declRefExpr(to(varDecl(hasType(isInteger())).bind("initVar")))
        )),
        hasRHS(ignoringParenImpCasts(integerLiteral().bind("init")))
      )
    )
  );
}

StatementMatcher DynamicHandler::conditionMatcher() {
  return (
    binaryOperator(
      anyOf(
        hasOperatorName("<"),
        hasOperatorName("<=")
      ),
      hasLHS(ignoringParenImpCasts(
        declRefExpr(to(varDecl(hasType(isInteger())).bind("condVar")))
      )),
      hasRHS(ignoringParenImpCasts(
        expr(hasType(isInteger())).bind("bound")
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
