CC = gcc
FLAGS = -Wall -Werror -Wextra
DEP = function_principale.c function_principale.h
EXE = exe

all: ${EXE}
	
${EXE}: ${DEP}
	${CC} ${DEP} ${FLAGS} -o $@