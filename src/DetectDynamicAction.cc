#include "DetectDynamicAction.hh"
#include "DetectDynamicASTConsumer.hh"

using std::unique_ptr;

DetectDynamicAction::DetectDynamicAction() {}

unique_ptr<ASTConsumer> DetectDynamicAction::CreateASTConsumer(CompilerInstance &CI, 
                                                                 StringRef InFile) 
{
  return unique_ptr<ASTConsumer>(new DetectDynamicASTConsumer());
}
