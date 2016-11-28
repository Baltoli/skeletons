#ifndef MAP_VISITOR_H
#define MAP_VISITOR_H

#include <clang/AST/RecursiveASTVisitor.h>

using namespace clang;

class MapVisitor : public RecursiveASTVisitor<MapVisitor> {
  public:
    bool VisitDecl(Decl *d);
};

#endif
