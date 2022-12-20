#include "funciones_shell.h"

void Usage(int argc, char *argv[]) {
  const std::string parametro = "--help";
  if(argv[1] == parametro){
    std::cout << "Formas de uso: Ejecute " << argv[0] << " (-a)(-m) ruta_fichero ruta destino." << std::endl;
    exit(EXIT_SUCCESS);
  } else if(argc != 3 && argc != 4) {
      std::cout << "Error, no se puede ejecutar ese comando." << std::endl;
      exit(EXIT_FAILURE);
  }
}

std::string IdentificarParametro(int argc, char *argv[]) {
  const std::string parametro = "-m";
  const std::string parametro2 = "-a";
  const std::string no_parametro = "0";
  if(argc == 4) {
    if (argv[1] == parametro) {return parametro;}
    if (argv[1] == parametro2) {return parametro2;}
  } else if ((argc == 3 && argv[1] == parametro) || (argc == 3 && argv[1] == parametro2)) {
    std::cout << "Error. Para ejecutar " << argv[1] << " tiene que utilizar 3 parámetros (parametro) (ruta_fichero) (ruta_destino)" << std::endl;
    exit(EXIT_FAILURE);
  }
  return no_parametro;
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