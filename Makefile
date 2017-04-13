.PHONY: build visualize benchmark

build: a.out
a.out: GraphDrawing.cpp
	g++ -std=c++11 -Wall -O2 -DLOCAL $^

RANDOM = $(shell bash -c 'echo $$RANDOM')
SEED ?= ${RANDOM}
SEED := ${SEED}
visualize: tester.jar a.out
	java -jar tester.jar -exec ./a.out -seed ${SEED} -vis -debug \
	    | grep -v '^[0-9]\+-[0-9]\+ = [0-9]\+ (dist [0-9]\+\.[0-9]\+)$$' \
	    | grep -v '^[0-9]\+ [0-9]\+\.[0-9]\+ [0-9]\+\.[0-9]\+$$' \
	    | grep -v '^[0-9]\+ - ([0-9]\+,[0-9]\+)$$'

score: tester.jar a.out
	for i in `seq 10` ; do java -jar tester.jar -exec ./a.out -seed `bash -c 'echo $$RANDOM'` ; done \
	    | tee /dev/stderr \
	    | grep Score \
	    | awk '{ sum += $$3; } END { printf("%f\n", sum / NR * 1000000); }'

