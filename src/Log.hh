#ifndef LOG_H
#define LOG_H

#include <string>

#define LOG_LEVEL 0

enum LogType {
  Output = -1,
  Debug,
  Error
};

constexpr bool isErrorLevel(enum LogType level) {
  return level >= 0;
}

std::string levelPrefix(enum LogType level);
void log(enum LogType type, std::string message);

#endif
