CXX = g++
CXXFLAGS = -pedantic -std=c++17

SRC = funciones_shell.cc main.cc terminal.cc tools.cc
OBJ = $(SRC:.cc=.o)
EXEC = program

all: $(EXEC)

$(EXEC): $(OBJ)
	$(CXX) $(LDFLAGS) -o $@ $(OBJ) $(LBLIBS)

clean:
	rm -rf $(OBJ) $(EXEC)