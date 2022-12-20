#include "terminal.h"
#include "funciones_shell.h"
#include "tools.h"

void Terminal::print_prompt(int last_command_status) {
  if(last_command_status == 1) print("$< ");   // llamamos a la función print("$>") en caso de ejecución correcta del anterior comando.
  else print("$> ");                           // llamamos a la función print("$<") en caso de ejecución incorrecta del anterior comando.
}

bool Terminal::EsComandoInterno(shell::command comando) { // Función que verifica si un comando es interno o no.
  if(comando[0] == "cd" || comando[0] == "echo" || comando[0] == "cp" || comando[0] == "mv") return true;
  return false;
}

std::error_code Terminal::cd_command(const std::vector<std::string>& args) { // Función para cambiar de directorio en caso de no error.
  struct stat comando_stat;
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

void Terminal::echo_command(const std::vector<std::string>& args) { // Función echo command.
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
  text += "\n";            // Añadimos un "\n" al final del texto para ejecutar el correspondiente salto de línea.
  std::cout << text;       // Devolvemos el texto.
}

int Terminal::foo_command(const std::vector<std::string>& args) {
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

shell::command_result Terminal::execute_commands(const std::vector<shell::command>& commands) {
  // SI HAY UN VECTOR VACÍO EN PARSE_LINE() NO HAY QUE LLAMAR A LA FUNCIÓN.
  int return_value=0;
  for(auto command : commands) {
    if(command.at(0) == "exit") return shell::command_result::quit(0);
    if(EsComandoInterno(command)) {
      return_value = foo_command(command);
      std::cout << return_value << std::endl;
    } 
    //else {
    //  execute_program(command);
    //  return_value = execute_program(command);  
    // }
  }
  return shell::command_result(return_value, false);
}

std::vector<shell::command> Terminal::parse_line(const std::string& line) {
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
          comando_actual.push_back(std::string{last_char});      // Introducimos el carácter en solitario 
                                                                 // como último argumento del comando actual
          comandos.push_back(comando_actual); // Añadimos el comando actual a la lista de comandos
          comando_actual.clear();             // Vaciamos el comando contenido en el comando actual.
      } else if (word[0] == '#') {
          break;                              // Si la línea empieza con un '#', omitimos esa línea.
      } else {
          comando_actual.push_back(word);     // Si no hemos llegado al final del comando ni es una línea comentada,
                                              // entendemos que no ha llegado su fin. Añadimos al comando actual la extensión.
      }
  }

  if (!comando_actual.empty()) {
      comandos.push_back(comando_actual); // En el caso de no tener un fin de comando (';', '|', '&'),
                                          // añadimos a comandos lo que tenemos en el comando actual.
  }
  return comandos;
}

std::error_code Terminal::read_line(int fd, std::string& line) {
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
      // PRUEBA MIRAR SI ESTÁ BIEN.
      if(!parse_line(line).empty()) {             // Si el parse_line no está vacío.
        execute_commands(parse_line(line));       // Ejecutamos execute_commands con parametro parse_line(line).
      }
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

void Terminal::start() {           // Función para empezar con la ejecución del programa.
  int fd = STDIN_FILENO, return_value=0; // Se declara fd como STDIN_FILENO.
  std::string line;                // Declaramos una variable string line para recoger los comandos introducidos por el usuario.
  bool condicion = true;
  while(condicion) {
    print_prompt(return_value);    // Imprimimos el prompt por cada ejecución.
    getline(std::cin, line);
    if(!line.empty()) {
      line += "\n";                  // Le añadimos un "\n" al final.
      read_line(fd, line);           // Comenzamos con la funcionalidad de el programa.
    }
  }
}