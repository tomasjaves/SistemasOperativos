# Prácticas de Sistemas Operativos

Este repositorio contiene las prácticas desarrolladas para la asignatura de Sistemas Operativos en el curso académico 2023-2024.
El proyecto se centra en la implementación de diferentes aspectos de los sistemas operativos, con un enfoque particular en la creación de shells y scripts de bash.

## Estructura del Repositorio

- `bash/`: Scripts y ejercicios relacionados con Bash.
- `shell/`: Implementación básica de una shell.
- `shell_part_one/`: Primera parte de la implementación de una shell avanzada.
- `shell_part_two/`: Segunda parte de la implementación de una shell avanzada.

Cada carpeta contiene el código fuente en C++, junto con la documentación necesaria para comprender y ejecutar cada práctica.

## Compilación y Ejecución

Para compilar y ejecutar los proyectos de shell, navega al directorio correspondiente y utiliza los siguientes comandos:

```bash
cd shell_part_two
make
./program
```

Si existe algún error, puedes compilar manualmente:

```bash
g++ --std=c++17 main.cc tools.cc funciones_shell.cc terminal.cc -o program
```

Para limpiar los archivos compilados:

```bash
make clean
```

# Uso
Cada práctica puede tener requisitos específicos de entrada/salida. Consulta la documentación dentro de cada carpeta de práctica para obtener instrucciones detalladas sobre su uso.
