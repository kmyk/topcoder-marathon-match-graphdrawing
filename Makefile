.PHONY: visualize score

CXX := g++
CXXFLAGS := -std=c++11 -Wall -O2

RANDOM = $(shell bash -c 'echo $$RANDOM')
SEED ?= ${RANDOM}
SEED := ${SEED}
visualize:
	${CXX} ${CXXFLAGS} -DLOCAL GraphDrawing.cpp
	java -jar tester.jar -exec ./a.out -seed ${SEED} -vis -debug \
	    | grep -v '^[0-9]\+-[0-9]\+ = [0-9]\+ (dist [0-9]\+\.[0-9]\+)$$' \
	    | grep -v '^[0-9]\+ [0-9]\+\.[0-9]\+ [0-9]\+\.[0-9]\+$$' \
	    | grep -v '^[0-9]\+ - ([0-9]\+,[0-9]\+)$$'

score:
	${CXX} ${CXXFLAGS} GraphDrawing.cpp
	for i in `seq 10` ; do java -jar tester.jar -exec ./a.out -seed $$i ; done \
	    | tee /dev/stderr \
	    | grep Score \
	    | awk '{ sum += $$3; } END { printf("%f\n", sum / NR * 1000000); }'

