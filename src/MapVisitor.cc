#include "MapVisitor.hh"

bool MapVisitor::VisitDecl(Decl *d) {
  d->dump();
  return true;
}
