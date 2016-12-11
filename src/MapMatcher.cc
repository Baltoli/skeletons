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

MapHandler::MapHandler(bool o) : overwrite(o) {}

void MapHandler::run(const MatchFinder::MatchResult &Result) {
  log(Debug, "Handling Map possible match result");

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

    const BinaryOperator *op = Result.Nodes.getNodeAs<BinaryOperator>("op");
    if(!op || !op->isAssignmentOp()) {
      log(Error, "Weird non-assignment binary op");
      return;
    }

    auto target = Result.Nodes.getNodeAs<DeclRefExpr>("target")->getNameInfo().getAsString();
    if(!isValidMapBody(forS->getBody(), op, target)) {
      log(Debug, "Near miss for Map: "
                 "For loop doesn't have a mappable body");
      return;
    }

    auto loc = Result.Context->getFullLoc(forS->getLocStart());
    log(Output, successOutputMessage(loc));
    
    addParallelAnnotation(forS->getLocStart(), Result);
  }
}

bool MapHandler::isValidMapBody(const Stmt *body, 
                                const BinaryOperator *op, string target) {
  // If the loop body is empty, then no harm done in marking it as parallel,
  // though there *should* be no way to actually trigger this behaviour in real
  // execution because of AST matching.
  if(!body) {
    return true;
  }

  auto compound = dyn_cast<CompoundStmt>(body);
  if(!compound) {
    log(Error, "For loop without compound body - why?");
    return false;
  }

  for(auto stmt : compound->body()) {
    // TODO: work out what we need to check here to rule out a particular loop
    // body as being mappable:
    //  - Assigning to anything in the source, at any point
    //  - Assigning to the target at an offset that *isn't* the loop index
    if(assignsToArray(stmt, target, op)) {
      return false;
    }
  }

  return true;
}

bool MapHandler::assignsToArray(const Stmt *stmt, string target, const BinaryOperator *op) {
  if(!stmt) {
    return false;
  }

  auto s = const_cast<Stmt *>(stmt)->IgnoreImplicit();
  auto found_op = dyn_cast<BinaryOperator>(s);
  if(found_op && found_op != op) {
    bool assign = found_op->isAssignmentOp();
    Expr *lhs = found_op->getLHS();
    
    auto arrLHS = dyn_cast<ArraySubscriptExpr>(lhs);
    if(arrLHS) {
      auto base = dyn_cast<DeclRefExpr>(arrLHS->getBase()->IgnoreImplicit());

      if(assign && base && base->getNameInfo().getAsString() == target) {
        log(Debug, "Found an assignment to the target array that isn't the original match");
        return true;
      }
    }
  }

  return std::any_of(stmt->child_begin(), stmt->child_end(), 
    [&](const Stmt *c) { 
      return assignsToArray(c, target, op);
    }
  );
}

void MapHandler::addParallelAnnotation(SourceLocation loc, 
                                       const MatchFinder::MatchResult &Result)
{
  // TODO: Is an empty LangOptions OK here? My instinct is that it probably is
  // as long as we are *only* doing simple insertion of text into the source
  // file.
  if(overwrite) {
    log(Debug, "Adding OpenMP annotation for discovered Map");

    Rewriter r(*Result.SourceManager, LangOptions());
    r.InsertText(loc, "#pragma omp parallel for\n", false, true);
    r.overwriteChangedFiles();
  }
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
  return (
    compoundStmt(hasAnySubstatement(binaryOperator(
      hasLHS(ignoringParenImpCasts(
        arraySubscriptExpr(
          hasBase(ignoringParenImpCasts(
              declRefExpr().bind("target"))),
          hasIndex(ignoringParenImpCasts(
              declRefExpr(hasDeclaration(equalsBoundNode("incVar")))))
        )
      ))
    ).bind("op")))
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
