# Compiladores
CXX = g++
CC = cc

# Flags
CXXFLAGS = -Wall -std=c++17 -O3 -Iinclude
CFLAGS = -Wall
NO_WARN_FLAGS = -w  # Desativa todos os warnings
RITT_SUPPRESS_WARNINGS = -Wno-maybe-uninitialized

# Pastas
SRC_DIR = src
INCLUDE_DIR = include
BIN_DIR = bin
OBJ_DIR = obj

# Arquivos fonte
CPP_SOURCES = $(SRC_DIR)/main.cpp \
              $(SRC_DIR)/graph.cpp \
			  $(SRC_DIR)/dinics.cpp  \
              $(SRC_DIR)/ford_fulkerson.cpp \
              $(SRC_DIR)/find_path_sources/bfs.cpp \
              $(SRC_DIR)/find_path_sources/dfs_random.cpp \
              $(SRC_DIR)/find_path_sources/fattest.cpp \
			  $(SRC_DIR)/find_path_sources/capacity_scaling.cpp

C_SOURCE = $(SRC_DIR)/support_code_ritt/generate_tournament.cpp
C_OBJECT = $(OBJ_DIR)/generate_tournament.o

RITT_SOURCE = $(SRC_DIR)/support_code_ritt/maxflow.cpp

# Executáveis
EXEC_MAIN = $(BIN_DIR)/tournament
EXEC_GEN = $(BIN_DIR)/gen_tournament
EXEC_RITT = $(BIN_DIR)/ritt_max_flow
# Alvo padrão
all: $(EXEC_MAIN) $(EXEC_GEN) $(EXEC_RITT)

# Criação dos diretórios bin e obj
$(BIN_DIR):
	mkdir -p $(BIN_DIR)

$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)

# Compilar generate_tournament.cpp com warnings desativados
$(C_OBJECT): $(C_SOURCE) | $(OBJ_DIR)
	$(CXX) $(NO_WARN_FLAGS) -c $< -o $@

# Compilar max_flow (C++) com todas as fontes necessárias
$(EXEC_MAIN): $(CPP_SOURCES) | $(BIN_DIR)
	$(CXX) $(CXXFLAGS) -o $@ $(CPP_SOURCES)

# Compilar gengraph (C++) com new_washington compilado separadamente
$(EXEC_GEN): $(C_OBJECT) | $(BIN_DIR)
	$(CXX) -o $@ $^

# Compilar ritt_max_flow (C++) com warning específico desativado
$(EXEC_RITT): $(RITT_SOURCE) | $(BIN_DIR)
	$(CXX) $(CXXFLAGS) $(RITT_SUPPRESS_WARNINGS) -o $@ $(RITT_SOURCE)


# Limpar arquivos compilados
clean:
	rm -rf $(BIN_DIR) $(OBJ_DIR)

clean_graph:
	rm -rf graphs/*


.PHONY: all clean
