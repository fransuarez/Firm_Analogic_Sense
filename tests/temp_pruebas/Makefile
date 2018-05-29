#PATH DEL MAKEFILE
MAKEFILE_DIR := $(dir $(abspath $(lastword $(MAKEFILE_LIST))))

#PATH A LA BASE DEL FIRMWARE
BASE_PATH = $(abspath $(MAKEFILE_DIR)../../)/

#-------------------------------------------------------------------
#PATH A LA BASE DE LAS LIBS MODULES
LIBS_PATH = $(BASE_PATH)modules/
LIBS_SUPPORT= $(LIBS_PATH)support/

#-------------------------------------------------------------------
#PATH EN DONDE QUEDAN LOS ARCHIVOS OBJETO
OUT_PATH = $(MAKEFILE_DIR)out/
OBJ_PATH = $(OUT_PATH)obj/

#includes all the project definitions
include project.mk

#-------------------------------------------------------------------
# para que make busque en otros directorios ademas del raíz
vpath %.o $(OBJ_PATH)
vpath %.c $(SRC_PATH)

#flags que se le pasa al compilador
CFLAGS += $(foreach inc, $(INC_PATH), -I$(inc) )
CFLAGS += -D_WIN32 -ggdb

#regla que linkea luego de tener los requisitos
all: info objs
	@echo "*** Linkeando" $(PROJECT_NAME) "***"
	gcc $(OBJ_PATH)*.o -lm -o $(MAKEFILE_DIR)$(PROJECT_NAME)

#regla que compila cada .c por serparado en un .o
%.o: %.c
	@echo
	@echo "*** Compilando $< ***"
	@echo
	@mkdir -p $(OBJ_PATH)
	@gcc -c $< -Wall $(CFLAGS) -o $(OBJ_PATH)$(notdir $(patsubst %.c,%.o, $< ))

#regla que como prerequisito necesita tener todos los .o (que se compilan por separado).
objs: $(foreach src_file, $(SRC_FILES), $(patsubst %.c, %.o, $(src_file) ) )


#regla que imprime simplemente la informacion
info:
	@echo '---------------------------------------------------------------------'
	@echo 'Firmware Base Path:   '$(BASE_PATH)
	@echo 'Test Path:            '$(MAKEFILE_DIR)
#	@echo 'Libs Path:            '$(LIBS_PATH)
#	@echo 'Libs Support:		 '$(LIBS_SUPPORT)
	@echo '---------------------------------------------------------------------'
	@echo 'Out Path:		'$(OUT_PATH)
	@echo 'SRC Files:       '$(SRC_FILES)
#	@echo 'OBJ Files:       '$(SRC_FILES)
#	@echo 'MFL_Last_Word:        '$(lastword $(MAKEFILE_LIST))
#	@echo 'MFL_LW_AbsPath:       '$(abspath $(lastword $(MAKEFILE_LIST)))
	@echo '---------------------------------------------------------------------'

clean:
	@rm -rf $(OBJ_PATH)*.o $(PROJECT_NAME)
#	rm -f $(EXE) $(OBJ_FILES) $(DEP_FILES)
