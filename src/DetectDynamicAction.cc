#include "DetectDynamicAction.hh"
#include "DetectSkeletonsASTConsumer.hh"

using std::unique_ptr;

DetectDynamicAction::DetectDynamicAction() {}

unique_ptr<ASTConsumer> DetectDynamicAction::CreateASTConsumer(CompilerInstance &CI, 
                                                                 StringRef InFile) 
{
  return unique_ptr<ASTConsumer>(new DetectSkeletonsASTConsumer(false));
}
