#include "tools.h"

std::error_code print(const std::string& str) {
  int bytes_written = write(STDOUT_FILENO, str.c_str(), str.size());
  if (bytes_written == -1)
  {
    return std::error_code(errno, std::system_category());
  }
  return std::error_code(0, std::system_category());
} 