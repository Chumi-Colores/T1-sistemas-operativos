###############################################################################
#                             Makefile Simple                                 #
###############################################################################

# Compilador
CC=gcc -std=gnu11

# Carpetas
SRC=src
OBJ=obj

# Optimizaciones
OPT=-g
# OPT=-O3

# Flags
CFLAGS=-Wunused -Wall $(OPT)

# Librerías
LIB=-lm

# Nombre del ejecutable
PROGRAM=DCCambios

###############################################################################
# Archivos                                                                    #
###############################################################################

# Todos los .c en src
SRCFILES := $(wildcard $(SRC)/*.c)

# Todos los .h en src
HDRFILES := $(wildcard $(SRC)/*.h)

# Archivos objeto correspondientes
OBJFILES := $(patsubst $(SRC)/%.c, $(OBJ)/%.o, $(SRCFILES))

###############################################################################
# Reglas                                                                      #
###############################################################################

all: $(OBJ) $(PROGRAM)
	@echo "done compiling"

# Crear carpeta de objetos si no existe
$(OBJ):
	@mkdir -p $(OBJ)

# Compilar .c -> .o
$(OBJ)/%.o: $(SRC)/%.c $(HDRFILES) Makefile
	@$(CC) $(CFLAGS) -c $< -o $@ $(LIB) && echo "compiled '$@'"

# Linkear objetos -> binario final
$(PROGRAM): $(OBJFILES)
	@$(CC) $(CFLAGS) $^ -o $@ $(LIB) && echo "compiled '$@'"

# Limpiar
clean:
	@rm -fv $(PROGRAM) && rm -rfv $(OBJ) && echo "done cleaning"
