#ifndef FUNCIONES_SHELL_H
#define FUNCIONES_SHELL_H

// NOMBRE: Tomás Javes Tommasone
// GRUPO: PE102
// GMAIL: alu0101515458@ull.edu.es

#include <iostream>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <cassert>
#include <cstdlib> 
#include <vector>
#include <system_error>
#include <fstream>
#include <unistd.h>
#include <stdio.h>
#include <errno.h>
#include <array>
#include <climits>
#include <libgen.h>
#include <utime.h>
#include "scope.hpp"

// NOMBRE: Tomás Javes Tommasone
// GRUPO: PE102
// GMAIL: alu0101515458@ull.edu.es

// PARA COMPILAR, USÉ LA HERRAMIENTA MAKE. EL EJECUTABLE SE LLAMA "program" Y SE EJECUTA DE LA FORMA:
// "./program".
//  SI HUBIERA ERROR, COMPILAR CON: g++ --std=c++17 main.cc tools.cc funciones_shell.cc terminal.cc

void Usage(int argc, char *argv[]);
std::string IdentificarParametro(int argc, char *argv[]);
std::error_code read(int fd, std::vector<uint8_t>& buffer);
std::error_code write(int fd, const std::vector<uint8_t>& buffer);
std::string dirname(const std::string& path);
std::string basename(const std::string& path);
std::error_code copy_file(const std::string& src_path, const std::string& dst_path, bool preserve_all=false);
std::error_code move_file(const std::string& src_path, const std::string& dst_path);

#endif