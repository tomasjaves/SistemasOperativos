#include "terminal.h"
#include "funciones_shell.h"
#include "tools.h"

// NOMBRE: Tomás Javes Tommasone
// GRUPO: PE102
// GMAIL: alu0101515458@ull.edu.es

// PARA COMPILAR, USÉ LA HERRAMIENTA MAKE. EL EJECUTABLE SE LLAMA "program" Y SE EJECUTA DE LA FORMA:
// "./program".
//  SI HUBIERA ERROR, COMPILAR CON: g++ --std=c++17 main.cc tools.cc funciones_shell.cc terminal.cc

// Función "print_prompt" que imprime por pantalla el prompt correspondiente.
void Terminal::print_prompt(int last_command_status) {
  std::string login = getlogin();                // Cogemos el nombre del usuario actual.
  std::string hostname;
  hostname.resize(1000);
  gethostname(hostname.data(), hostname.size()); // Cogemos el nombre de la máquina.
  std::string cwd = getcwd(nullptr, 0);          // Cogemos la ruta actual de la shell.
  // llamamos a la función print() con "$>" en caso de ejecución correcta del anterior comando.
  if(last_command_status == 0) print(login + "@" + hostname + ":" + cwd + "$> ");
  // llamamos a la función print() con "$<" en caso de ejecución incorrecta del anterior comando.
  else print(login + "@" + hostname + ":" + cwd + "$< ");
}

// Función "EsComandoInterno" que verifica si un comando es interno.
bool Terminal::EsComandoInterno(shell::command comando) {
  if(comando[0] == "cd" || comando[0] == "echo" || comando[0] == "cp" || comando[0] == "mv") return true;
  return false;
}

// Función "cd_command", perteneciente a la función "interno_command", para cambiar de directorios.
int Terminal::cd_command(const std::vector<std::string>& args) {
  // Utilicé un struct stat para obtener la información correspondiente
  // del directorio con el que vamos a trabajar.
  struct stat comando_stat;
  stat(args[1].c_str(), &comando_stat);
  if(args.size() != 2) {                // Si no hay dos argumentos, se devuelve un error.
    std::cerr << "El comando cd se debe ejecutar de la forma; cd dst_path/" << std::endl;
    return errno;
  }
  else {
    if(S_ISDIR(comando_stat.st_mode)) { // Si es directorio.
      chdir(args.at(1).c_str());        // Se ejecuta chdir.
      return 0;
    } else {                            // Si no es directorio.
      std::cerr << "El directorio no existe" << std::endl;
      return errno;                     // Se devuelve un error.
    }
  }
}

// Función "echo_command", perteneciente a la función "interno_command".
int Terminal::echo_command(const std::vector<std::string>& args) {
  std::string text;       // Declaramos una variable text para almacenar todos los datos a leer.
  if (args.size() == 1) { // Si el tamaño del vector de argumentos es 1, es decir, solo se ha pasado el comando "echo",
                          // imprimimos una línea en blanco.
    std::cout << std::endl;
    return 0;
  }
  // Recogemos los datos en la posición correspondiente. (Sin incluir el echo).
  for(size_t i = 1; i < args.size(); ++i) {
    text += args[i] + " ";
  }
  text += "\n";            // Añadimos un "\n" al final del texto para ejecutar el correspondiente salto de línea.
  std::cout << text;       // Devolvemos el texto.
  return 0;
}

// Función "interno_command", perteneciente a la función "execute_commands".
// Sirve como menú para la elección del comando correspondiente.
int Terminal::interno_command(const std::vector<std::string>& args) {
  if(args[0] == "cd") {           // Si recogemos un "cd" como palabra introducida por el usuario,
                                  // ejecutamos la función cd_command.
    cd_command(args);
  } else if(args[0] == "echo") {  // Si recogemos un "echo" como palabra introducida por el usuario,
                                  // ejecutamos la función echo_command.
    echo_command(args);
  } else if(args[0] == "cp") {    // Si recogemos un "cp" como palabra introducida por el usuario,
                                  // ejecutamos la función copy_file.
    if(args.size() == 4) {
      std::cout << args[2] << " " << args[3] << std::endl;
      copy_file(args[2], args[3], true);
    } else if(args.size() == 3) {
      std::cout << args[1] << " " << args[2] << std::endl;
      copy_file(args[1], args[2]);
    }
  } else if(args[0] == "mv") {    // Si recogemos un "mv" como palabra introducida por el usuario,
                                  // ejecutamos la función move_file.
    move_file(args[1], args[2]);
  }
  return 0;
}

int execute_program(const std::vector<std::string>& args, bool has_wait=true) {
  pid_t child = fork();             // Creamos un proceso hijo.
  int status;                       // Creamos una variable status que almacene el resultado de la ejecución de execvp.
  std::vector<const char*> result;  // Creamos un vector de char constantes para ir almacenando los argumentos de args
                                    // puesto que estos son const char**.

  if(child == 0) {                               // Si es un proceso hijo.
    for(int i = 0; i < args.size(); ++i) {
      result.push_back(args[i].c_str());         // Almacenamos en result los argumentos de args.
    }
    result.push_back(nullptr);                   // Una vez almacenemos todos los argumentos correspondientes,
                                                 // almacenamos un puntero nulo porque exec() lo necesita para poder ejecutarse.
    status = execvp(result[0], const_cast<char* const*>(result.data()));  // Ejecutamos exec() y a la par guardamos el resultado
                                                                          // devuelto en status.
  } else if(child > 0){                          // Si es un proceso padre.
    if(has_wait) {                               // Si has_wait es true.
      wait(nullptr);                             // Ejecutamos la función wait para que espere a que el hijo termine.
    }
  }
  return status;                                 // Devolvemos el valor de status.
}

shell::command_result Terminal::execute_commands(const std::vector<shell::command>& commands) {
  int return_value=0; // Creamos una variable de tipo entero que posteriormente
                      // almacenará la salida de los comandos internos/externos.

  for(auto command : commands) {
    bool has_wait = true;                            // Creamos una variable has_wait inicializada a true.
    char last_char = command[command.size() - 1][0]; // asignamos "last_char" como el último carácter del vector.
    if(last_char == '&') has_wait=false;             // Si en el comando recibimos un &, has_wait entendemos que es falso.
    if (last_char == ';' || last_char == '&' || last_char == '|') command.pop_back(); // Si last_char es uno de estos casos,
                                                                                      // lo eliminamos.
    if(command.at(0) == "exit") return shell::command_result::quit(0); // Si el comando recibe un "exit", saldremos del programa.

    if(EsComandoInterno(command)) {                                    // Si es un comando interno.
      return_value = interno_command(command);                         // Ejecutamos "interno_command" a la par que guardamos
                                                                       // el valor dado por esta función.
    } 
    else {                                               // Si no es un comando interno.
      return_value = execute_program(command, has_wait); // Ejecutamos "execute_program" a la par que guardamos
                                                         // el valor dado por esta función. 
    }
  }
  return shell::command_result(return_value, false);     // Devolvemos un shell_command_result correspondiente.
}

std::vector<shell::command> Terminal::parse_line(const std::string& line) {
  std::istringstream iss(line);         // creamos un istringstream para coger comando por comando de la cadena.
  std::vector<shell::command> comandos; // creamos el vector de vectores "comandos" que contendrá todos los comandos.
  shell::command comando_actual;        // creamos un vector de strings para ver el comando actual.

  while (!iss.eof()) {
      std::string word; // necesitamos una string para almacenar comando por comando de la cadena total de comandos
                        // pasados por parámetros.
      iss >> word;      // recogemos el comando de "line".
      if(word.empty()) continue;
      char last_char = word[word.size() - 1]; // asignamos "last_char" como el último carácter de la palabra.

      if (last_char == ';' || last_char == '&' || last_char == '|') {
        if(word.length() > 1) {
          word.erase(word.length() - 1);      // Eliminamos el último carácter de la palabra.
          comando_actual.push_back(word);     // Añadimos la palabra al vector de comandos actuales
        }
        // Puesto que este ultimo caracter nos indica el final del comando (';', '|', '&'),
        // añadiremos el comando al vector de vectores ("std::vector<shell::command> comandos;").      
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

void Terminal::start() {                   // Función para empezar con la ejecución del programa.
  int fd = STDIN_FILENO;                   // Se declara fd como STDIN_FILENO.
  std::string line;                        // Declaramos una variable string line para recoger los comandos introducidos por el usuario.
  bool condicion = true;                   
  shell::command_result result(0,condicion);
  while(condicion) {
    print_prompt(result.return_value);    // Imprimimos el prompt por cada ejecución.
    getline(std::cin, line);
    if(!line.empty()) {
      line += "\n";                              // Le añadimos un "\n" al final.
      read_line(fd, line);                       // Comenzamos con la funcionalidad de el programa.
      std::vector<shell::command> commands = parse_line(line);
      result = execute_commands(commands);       // Ejecutamos execute_commands con parametro parse_line(line).
      if(result.is_quit_requested) condicion = false;
    }
  }
}