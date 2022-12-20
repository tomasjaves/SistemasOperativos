#ifndef TERMINAL_H
#define TERMINAL_H
#include <iostream>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <cassert>
#include <cstdlib> 
#include <vector>
#include <system_error>
#include <fstream>
#include <sstream>
#include <unistd.h>
#include <stdio.h>
#include <errno.h>
#include <array>
#include <climits>
#include <libgen.h>
#include <utime.h>
#include "scope.hpp"
#include "tools.h"

namespace shell
{
  using command = std::vector<std::string>;
}

namespace shell
{
struct command_result
  {
    int return_value;
    bool is_quit_requested;
    command_result(int return_value, bool request_quit=false)
    : return_value{return_value},
    is_quit_requested{request_quit}
    {}
    static command_result quit(int return_value=0)
    {
      return command_result{return_value, true};
    }
  };
}

class Terminal {
  public:
  void start();
  void print_prompt(int last_command_status);
  std::error_code cd_command(const std::vector<std::string>& args);
  void echo_command(const std::vector<std::string>& args);
  int foo_command(const std::vector<std::string>& args);
  bool EsComandoInterno(shell::command);
  shell::command_result execute_commands(const std::vector<shell::command>& commands);
  std::vector<shell::command> parse_line(const std::string& line);
  std::error_code read_line(int fd, std::string& line);

  private:
};

#endif