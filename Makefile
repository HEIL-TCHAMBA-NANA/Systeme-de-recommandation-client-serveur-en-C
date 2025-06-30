CC = gcc
FLAGS = -Wall -Werror -Wextra
DEP = graphes/graphes.c graphes/graphes.h
EXE = exe

all: ${EXE}
	
${EXE}: ${DEP}
	${CC} ${DEP} ${FLAGS} -o $@