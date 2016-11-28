#include "DetectSkeletonsAction.hh"
#include "DetectSkeletonsASTConsumer.hh"

using std::unique_ptr;

DetectSkeletonsAction::DetectSkeletonsAction() {}

unique_ptr<ASTConsumer> DetectSkeletonsAction::CreateASTConsumer(CompilerInstance &CI, 
                                                                 StringRef InFile) 
{
  return unique_ptr<ASTConsumer>(new DetectSkeletonsASTConsumer);
}
