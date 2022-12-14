#include "funciones_shell.h"

int main(int argc, char *argv[]) {
  const std::string palabra_m = "-m";
  const std::string palabra_a = "-a";
  std::string parametro = IdentificarParametro(argc, argv);
  Usage(argc, argv);
  if(parametro == palabra_m) {
      move_file(argv[2], argv[3]);
      std::string src_path = argv[2], dst_path = argv[3];
  } else if (parametro == palabra_a) {
      bool preserve_all = true;
      std::string src_path = argv[2], dst_path = argv[3];
      copy_file(src_path, dst_path, preserve_all);
  } else if(parametro == "0") {
      std::string src_path = argv[1], dst_path = argv[2];
      copy_file(src_path, dst_path);
  }
}