#ifndef MAP_MATCHER_H
#define MAP_MATCHER_H

#include <clang/ASTMatchers/ASTMatchers.h>
#include <clang/ASTMatchers/ASTMatchFinder.h>

using namespace clang::ast_matchers;

class MapHandler : public MatchFinder::MatchCallback {
  public:
    MapHandler(bool overwrite);

    /**
     * Callback method that is run when we match a result in the AST.
     */
    virtual void run(const MatchFinder::MatchResult &Result);

    /**
     * Clang AST matcher that matches any for-loop with structure recognisable
     * as being a map over a collection.
     *
     * The matching properties of this matcher are currently defined entirely in
     * terms of its child matchers.
     */
    StatementMatcher matcher();

    /**
     * Clang AST matcher that matches for-loop initialization statements that
     * could be part of a map operation.
     *
     * This will match any loop initialisation that:
     *  - Declares a single variable
     *  - Is of an integer type after implicit / parenthesized casts
     *  - Initialises the variable to constant 0
     */
    StatementMatcher loopInitMatcher();

    /**
     * Clang AST matcher that matches for-loop conditions that could be part of
     * a map operation.
     *
     * This will match conditions that:
     *  - Compare using the < operator
     *  - Compare to a single variable
     */
    StatementMatcher loopConditionMatcher();
    
    /**
     * Clang AST matcher that matches for-loop increments that could be part of
     * a map operation.
     *
     * This will match increment statements that:
     *  - Pre or post increment the variable defined in the initializer
     */
    StatementMatcher loopIncrementMatcher();

    /**
     * Clang AST matcher that matches the body of a for-loop that could be a
     * mappable operation.
     *
     * This will match bodies that:
     *  - TODO
     */
    StatementMatcher bodyMatcher();
  private:
    void addParallelAnnotation(clang::SourceLocation loc, 
                               const clang::ast_matchers::MatchFinder::MatchResult &Result);
    bool isValidMapBody(const clang::Stmt *body, const clang::BinaryOperator *op);
    bool assignsToArray(const clang::Stmt *stmt, 
                        std::string arr, std::string index, const clang::BinaryOperator *op);
    std::string successOutputMessage(clang::FullSourceLoc loc);
    bool overwrite;
};
  

#endif
