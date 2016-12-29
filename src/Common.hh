#ifndef COMMON_HH
#define COMMON_HH

#include <vector>

#include <clang/ASTMatchers/ASTMatchers.h>
#include <clang/ASTMatchers/ASTMatchFinder.h>

using std::vector;
using std::string;
using namespace clang;
using namespace clang::ast_matchers;

bool allEqual(vector<string> ss);
string getDeclName(const MatchFinder::MatchResult &Result, string binding);

#endif
