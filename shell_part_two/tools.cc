#include "tools.h"

// NOMBRE: Tomás Javes Tommasone
// GRUPO: PE102
// GMAIL: alu0101515458@ull.edu.es

// PARA COMPILAR, USÉ LA HERRAMIENTA MAKE. EL EJECUTABLE SE LLAMA "program" Y SE EJECUTA DE LA FORMA:
// "./program".
//  SI HUBIERA ERROR, COMPILAR CON: g++ --std=c++17 main.cc tools.cc funciones_shell.cc terminal.cc

// Método para imprimir por pantalla.
std::error_code print(const std::string& str) {
  if (write(STDOUT_FILENO, str.c_str(), str.size()) == -1) // Si la llamada a la función write da error.
  {
    return std::error_code(errno, std::system_category()); // Devolvemos un error correspondiente.
  }
  return std::error_code(0, std::system_category());       // Si no da error, devolvemos un 0.
}

// Método para obtener el nombre de usuario.
std::string getUserlogin() {
    char* userlogin = getlogin();          // Devuelve un puntero a una cadena de caracteres que contiene el nombre de usuario.
    if (userlogin == nullptr) {            // Si getlogin() devuelve un puntero nulo significa que getlogin() 
                                           // no pudo obtener el nombre de usuario de forma exitosa.
        std::array<char, 256> uname {};    // Creamos un array de chars de tamaño 256 necesario para almacenar el nombre.
        if (getlogin_r(uname.data(), uname.size() - 1) != 0) { // Si no nos devuelve 0, no se pudo obtener el nombre.

            struct passwd* pwd = getpwuid(getuid());           // Se llama a la función getpwuid(), que toma como argumento
                                                               // el ID de usuario del proceso actual y devuelve un puntero 
                                                               // a una estructura passwd que contiene información sobre el usuario.
            if (pwd) return pwd->pw_name;  // Si pwd contiene información, devolvemos pw_name contenido en la estructura.
        }
        return uname.data();               // Si da 0, devolvemos el nombre contenido en uname.
    }
    return userlogin;                      // Si el getlogin() no apunta a nullptr, devolvemos el nombre almacenado en userlogin.
}

// Función destinada al uso del comando "cd -"
std::string directory() {
  std::string directorio = getcwd(nullptr, 0); // Almacenamos el directorio actual y lo devolvemos.
  return directorio;
} 