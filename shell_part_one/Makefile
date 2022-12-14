# Universidad de La Laguna
# Escuela Superior de Ingeniería y Tecnología
# Grado en Ingeniería Informática
# Computabilidad y Algoritmia
#
# @author Tomás Javes Tommasone alu0101515458@ull.edu.es
# @date Dec 05/2022
# @brief Makefile de los archivos pertenecientes a la práctica 7

CXX = g++						             # The C++ compiler command
CXXFLAGS = -std=c++17 -g -Wall	 # The C++ compiler options

# The all target builds all of the programs handled by the makefile.
all: program 

program: funciones_shell.o main.o
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)

# Indicate that the all and clean targets do not
# correspond to actual files.
.PHONY: all clean

	$(CXX) $(CXXFLAGS) -c $<

# The clean target removes all of the executable files
# and object files produced by the build process
# We can use it for additional housekeeping purposes
clean:
	rm -f *.fa
	rm -f *.gra
	rm -f *.o funciones_shell
	rm -f *.o funciones_shell doc
	rm -rf *~ basura b i
	rm -rf program
	find . -name '*~' -exec rm {} \;
	find . -name basura -exec rm {} \;
