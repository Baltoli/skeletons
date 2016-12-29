#include "Common.hh"

#include <algorithm>
#include <string>

using std::vector;
using std::string;

bool allEqual(vector<string> ss) {
  if(ss.empty()) {
    return true;
  }

  auto first = *ss.begin();
  return std::all_of(
    ss.begin() + 1, ss.end(), 
    [&](string s) { 
      return s == first; 
    }
  );
}

string getDeclName(const MatchFinder::MatchResult &Result, string binding) {
  auto decl = Result.Nodes.getNodeAs<VarDecl>(binding);
  if(decl) {
    return decl->getNameAsString();
  } else {
    return string("");
  }
}
