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
#include "funciones_shell.h"

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

std::error_code read(int fd, std::vector<uint8_t>& buffer)
{
  ssize_t bytes_read = read(fd, buffer.data(), buffer.size());
  if (bytes_read < 0)
  {
  return std::error_code(errno, std::system_category());
  }
  buffer.resize(bytes_read);
  return std::error_code(0, std::system_category());
}

std::error_code write(int fd, const std::vector<uint8_t>& buffer)
{
  ssize_t bytes_read = write(fd, buffer.data(), buffer.size());
  if (bytes_read < 0)
  {
  return std::error_code(errno, std::system_category());
  }
  return std::error_code(0, std::system_category());
}

std::string dirname(const std::string& path)
{
  std::array<char, PATH_MAX> buffer;
  path.copy(buffer.data(), buffer.size());
  return std::string(dirname(buffer.data()));
}

std::string basename(const std::string& path)
{
  std::array<char, PATH_MAX> buffer;
  path.copy(buffer.data(), buffer.size());
  return std::string(basename(buffer.data()));
}

std::error_code copy_file(const std::string& src_path, const std::string& dst_path, bool preserve_all) {
  struct stat src_stat;
  stat(src_path.c_str(), &src_stat);
  if(errno == ENOENT) { // ALMACENA EN ERRNO LA INFORMACIÓN SOBRE SI EL ARCHIVO EXISTE (ENOENT).
    return std::error_code(errno, std::system_category()); // devuelve un error con un argumento predefinido en errno
  }
  if (!S_ISREG(src_stat.st_mode)) { // st_mode nos devuelve el tipo de archivo. S_ISREG() comprueba si es un archivo regular
    return std::error_code(errno, std::system_category()); // devuelve un error con un argumento predefinido en errno
  }
  struct stat dst_stat;
  stat(dst_path.c_str(), &dst_stat);
  std::string new_dst_path = dst_path; // Puesto que dst_path es constante, creamos un nuevo dst_path para poder almacenar el archivo dentro.
  if(errno != ENOENT) { // ALMACENA EN ERRNO LA INFORMACIÓN SOBRE SI EL DIRECTORIO EXISTE (ENOENT).
    assert(src_stat.st_dev != dst_stat.st_dev || src_stat.st_ino != dst_stat.st_ino); // Asegura que son distintos dispositivos y distintos inodos
    if (S_ISDIR(dst_stat.st_mode)) { // st_mode nos devuelve el tipo de archivo. S_ISDIR() comprueba si es un directorio
      new_dst_path = dirname(new_dst_path) + "/" + basename(src_path);
    }
  }
  int fd_src = open(src_path.c_str(), O_RDONLY); // se abre en modo 'read only'
  int fd_dst = open(new_dst_path.c_str(), O_WRONLY | O_CREAT | O_TRUNC); // se abre en 'write only' y sino se crea y en ambos casos se elimina
                                                                         // todo lo que exista con O_TRUNC.
  // Asegurarnos de que se cierre en caso de que falle
  // SRC
  if (fd_src < 0){
    return std::error_code(errno, std::system_category());
  }
  auto src_guard = scope::scope_exit(
    [fd_src]{ close(fd_src); }
  );
  // DST
  if (fd_dst < 0){
    return std::error_code(errno, std::system_category());
  }
  auto dst_guard = scope::scope_exit(
    [fd_dst]{ close(fd_dst); }
  );

  std::vector<uint8_t> buffer(16ul * 1024 * 1024); // creamos un buffer para recoger los datos.
  do {
    read(fd_src, buffer); // lee del src con el buffer.
    write(fd_dst, buffer); // escribe al dst con el buffer.
  } while(buffer.size() > 0);
  close(fd_src); // cerramos src.
  close(fd_dst); // cerramos dst.
  if(preserve_all) {
    mode_t permisos = src_stat.st_mode&~S_IFMT; // cambiamos los permisos con la negacion ~.
    chmod(new_dst_path.c_str(), permisos); // le damos a dst los permisos modificados.
    chown(new_dst_path.c_str(), src_stat.st_uid, src_stat.st_gid); // le damos los mismos usuarios propietarios con st_uid y los mismos grupos con st_gid.
    struct utimbuf u_time; // creamos una estructura utimebuf para almacenar el tiempo de acceso y tiempo de modificación de src para almacenarla en dst_path.
    u_time.actime = src_stat.st_atime; 
    u_time.modtime = src_stat.st_mtime;
    utime(new_dst_path.c_str(), &u_time); // con la función utime le damos estos tiempos a dst_path.
  }
  return std::error_code();
}

std::error_code move_file(const std::string& src_path, const std::string& dst_path) {
  struct stat src_stat;
  struct stat dst_stat;
  stat(src_path.c_str(), &src_stat);
  if(errno == ENOENT) { // ALMACENA EN ERRNO LA INFORMACIÓN SOBRE SI EL ARCHIVO EXISTE (ENOENT).
    return std::error_code(errno, std::system_category()); // devuelve un error con un argumento predefinido en errno
  }
  std::string new_dst_path = dst_path; // Puesto que dst_path es constante, creamos un nuevo dst_path para poder almacenar el archivo dentro.
  if (S_ISDIR(dst_stat.st_mode)) { // st_mode nos devuelve el tipo de archivo. S_ISDIR() comprueba si es un directorio
    new_dst_path += "/" + basename(src_path);
  }
  if(src_stat.st_dev == dst_stat.st_dev) {
    int err=rename(src_path.c_str(), dst_path.c_str());
    if(err != 0) {
      return std::error_code(errno,std::system_category());
    }
    rename(src_path.c_str(), dst_path.c_str());
  } else {
    copy_file(src_path, dst_path, true);
    unlink(src_path.c_str());
  }
  return std::error_code();
}

std::error_code print(const std::string& str) {
  int bytes_written = write(STDOUT_FILENO, str.c_str(), str.size());
  if (bytes_written == -1)
  {
    return std::error_code(errno, std::system_category());
  }
  return std::error_code(0, std::system_category());
} 

void print_prompt(int last_command_status) {
  if(last_command_status == 1) print("$< "); // llamamos a la función print("$>") en caso de ejecución correcta del anterior comando.
  else print("$> ");                         // llamamos a la función print("$<") en caso de ejecución incorrecta del anterior comando.
}

bool EsComandoInterno(shell::command comando) { // Función que verifica si un comando es interno o no.
  if(comando[0] == "cd" || comando[0] == "echo" || comando[0] == "cp" || comando[0] == "mv") return true;
  return false;
}

std::error_code cd_command(const std::vector<std::string>& args) { // Función para cambiar de directorio en caso de no error.
  struct stat comando_stat;
  std::cout << args[1] << std::endl;
  stat(args[1].c_str(), &comando_stat);
  if(args.size() != 3) { 
    std::cerr << "El comando cd se debe ejecutar de la forma; cd dst_path/" << std::endl;
    return std::error_code(errno, std::system_category());         // Si hay más de dos argumentos se devuelve un error.
  }
  else {
    if(S_ISDIR(comando_stat.st_mode)) { // Si es directorio.
      chdir(args.at(1).c_str());        // Se ejecuta chdir.
      std::cout << "Te encuentras actualmente en el directorio " << args[1] << std::endl;
      return std::error_code();
    } else {                            // Si no es directorio.
      std::cerr << "El directorio no existe" << std::endl;
      return std::error_code(errno, std::system_category()); // Se devuelve un error correspondiente.
    }
  }
}

void echo_command(const std::vector<std::string>& args) { // Función echo command.
  std::string text;       // Declaramos una variable text para almacenar todos los datos a leer.
  if (args.size() == 1) { // Si el tamaño del vector de argumentos es 1, es decir, solo se ha pasado el comando "echo",
                          // imprimimos una línea en blanco.
    std::cout << std::endl;
    return;
  }
  // Recogemos los datos en la posición correspondiente. (Sin incluir el echo).
  for(size_t i = 1; i < args.size(); ++i) {
    text += args[i] + " ";
  }
  text += "\n";                   // Añadimos un "\n" al final del texto para ejecutar el correspondiente salto de línea.
  std::cout << text;              // Devolvemos el texto.
}

int foo_command(const std::vector<std::string>& args) {
  if(args[0] == "cd") {
    cd_command(args);
  } else if(args[0] == "echo") {
    echo_command(args);
  } else if(args[0] == "cp") {
    if(args.size() == 5) {
      copy_file(args[2], args[3], true);
    } else if(args.size() == 4) {
      copy_file(args[1], args[2]);
    }
  } else if(args[0] == "mv") {
    move_file(args[1], args[2]);
  }
  return 0;
}

shell::command_result execute_commands(const std::vector<shell::command>& commands) {
  // SI HAY UN VECTOR VACÍO EN PARSE_LINE() NO HAY QUE LLAMAR A LA FUNCIÓN.
  int return_value=0;
  for(auto command : commands) {
    if(command.at(0) == "exit") return shell::command_result::quit(0);
    if(EsComandoInterno(command)) {
      return_value = foo_command(command);
    } 
    //else {
    //  execute_program(command);
    //  return_value = execute_program(command);  
    // }
  }
  return shell::command_result(return_value, false);
}


std::vector<shell::command> parse_line(const std::string& line) {
  std::istringstream iss(line);         // creamos un istringstream para coger comando por comando de la cadena.
  std::vector<shell::command> comandos; // creamos el vector de vectores "comandos" que contendrá todos los comandos.
  shell::command comando_actual;        // creamos un vector de strings para ver el comando actual.

  while (!iss.eof()) {
      std::string word; // necesitamos una string para almacenar comando por comando de la cadena total de comandos
                        // pasados por parámetros.
      iss >> word;      // recogemos el comando de "line".
      char last_char = word[word.size() - 1]; // asignamos "last_char" como el último carácter de la palabra.

      if (last_char == ';' || last_char == '&' || last_char == '|') {
        // Puesto que este ultimo caracter nos indica el final del comando (';', '|', '&'),
        // añadiremos el comando al vector de vectores ("std::vector<shell::command> comandos;").
          word.erase(word.size() - 1);        // Eliminamos el último carácter de la palabra.
          comando_actual.push_back(word);     // Añadimos la palabra al vector de comandos actuales
          comando_actual.push_back(std::string{last_char});      // Introducimos el carácter en solitario como último argumento del comando actual
          comandos.push_back(comando_actual); // Añadimos el comando actual a la lista de comandos
          comando_actual.clear();             // Vaciamos el comando contenido en el comando actual.
      } else if (word[0] == '#') {
          break;                          // Si la línea empieza con un '#', omitimos esa línea.
      } else {
          comando_actual.push_back(word); // Si no hemos llegado al final del comando ni es una línea comentada,
                                          // entendemos que no ha llegado su fin. Añadimos al comando actual la extensión.
      }
  }

  if (!comando_actual.empty()) {
      comandos.push_back(comando_actual); // En el caso de no tener un fin de comando (';', '|', '&'),
                                          // añadimos a comandos lo que tenemos en el comando actual.
  }
  return comandos;
}

std::error_code read_line(int fd, std::string& line) {
  std::string pending_input = line; // creamos pending input para utilizarlo como almacen de line.
  ssize_t posicion;                 // creamos una posición que utilizaremos posteriormente con "find".
  std::array<uint8_t,512> buffer;   // creamos un buffer para leer.

  while(true) {
    line.clear();                                 // borramos todo lo que haya en line (asegurarnos).
    posicion = pending_input.find("\n");          // almacenamos en "posición" la posición de "\n".
    if(posicion != std::string::npos) {           // si existe la posición.
      line = pending_input.substr(0, posicion+1); // almacenamos en line el contenido de pending_input sin el \n.
      pending_input = pending_input.substr(posicion+1, pending_input.size()-posicion-1); // pending_input almacena ahora lo que no está
                                                                                         // contenido en line. (desde \n hasta el final).
      execute_commands(parse_line(line));
      return std::error_code();
    }

    ssize_t bytes=read(fd, buffer.data(), buffer.size());    // se almacena en bytes el resultado del read.
    if(bytes==-1) {
      return std::error_code(errno, std::system_category()); // si es -1 devuelve un error específico con errno.
    }

    if(bytes==0) {                // si bytes es 0 (buffer no tiene nada).
      if(pending_input.empty()) { // si pending_input está vacío.
        line = pending_input;     // line contendrá lo mismo que pending_input (nada/vacío).
        line += "\n";             // añadimos al final un "\n".
        pending_input.clear();    // vaciamos pending_input.
      }
      return std::error_code();
    } else {                        // si bytes no es 0 (buffer con contenido).
      for(int i = 0; i < buffer.size(); ++i) {
        pending_input += buffer[i]; // añadimos a pending_input la información del buffer.
      }
    }
  }
  return std::error_code();
}

void start() {
  int fd = STDIN_FILENO, return_value=0;
  std::string line;
  while(true) {
    print_prompt(return_value);
    getline(std::cin, line);
    line += "\n";
    read_line(fd, line);
  }
}

/*
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
*/

int main() {
  start();
  return 0;
}