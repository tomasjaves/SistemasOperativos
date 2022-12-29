#include "tools.h"

// NOMBRE: Tomás Javes Tommasone
// GRUPO: PE102
// GMAIL: alu0101515458@ull.edu.es

// PARA COMPILAR, USÉ LA HERRAMIENTA MAKE. EL EJECUTABLE SE LLAMA "program" Y SE EJECUTA DE LA FORMA:
// "./program".
//  SI HUBIERA ERROR, COMPILAR CON: g++ --std=c++17 main.cc tools.cc funciones_shell.cc terminal.cc

std::error_code print(const std::string& str) {
  int bytes_written = write(STDOUT_FILENO, str.c_str(), str.size());
  if (bytes_written == -1)
  {
    return std::error_code(errno, std::system_category());
  }
  return std::error_code(0, std::system_category());
} 