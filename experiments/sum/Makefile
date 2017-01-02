SRCS := main.c sum.c
EXPERIMENT := fill.c

NAME := sum
PROGS := ${NAME} ${STRAT}_${NAME}

.PHONY: all
all: ${PROGS}

clean:
	rm -f ${PROGS} *_${EXPERIMENT} *_${NAME}
	rm -f meta

${STRAT}_${EXPERIMENT} : ${EXPERIMENT}
	../../scripts/reorder.sh ${STRAT} . ${EXPERIMENT}

${NAME}: ${SRCS} ${EXPERIMENT}
	${CC} ${CFLAGS} ${SRCS} ${EXPERIMENT} -o $@

${STRAT}_${NAME}: ${SRCS} ${STRAT}_${EXPERIMENT}
	${CC} ${CFLAGS} ${SRCS} ${STRAT}_${EXPERIMENT} -o $@
