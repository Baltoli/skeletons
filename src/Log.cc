#include "Log.hh"

#include <iostream>

#include <llvm/Support/raw_ostream.h>

void log(enum LogType type, std::string message) {
  if(isErrorLevel(type)) {
    if(type >= LOG_LEVEL) {
      llvm::errs() << levelPrefix(type) << " "
                   << message << '\n';
    }
  } else {
    std::cout << message << '\n';
  }
}

std::string levelPrefix(enum LogType level) {
  switch(level) {
    case Output:
      return "";
    case Debug:
      return "[Debug]";
    case Error:
      return "[Error]";
  }
}
