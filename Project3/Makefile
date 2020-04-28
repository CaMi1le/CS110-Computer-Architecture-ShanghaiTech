#
# Feel free to modify and polish the Makefile if you wish.
#
# Also, you may (and should) add more compiler flags when introducing some
#   optimization techniques. BUT, it is not allowed to turn off `-W*` error
#   flags. Be strict on warnings is a good habit.
#
# Jose @ ShanghaiTech University
#

# Adapt this makefile to macos
SYSNAME := $(shell uname -s)
ifeq ($(SYSNAME), Linux)
	CC=g++
endif
ifeq ($(SYSNAME), Darwin)
	CC=g++-9
endif

CFLAGS=-Wpedantic -Wall -Werror -Wextra -O3 -mavx -mavx2 -mvzeroupper -fopenmp -std=c++11 -g

all: kmeans

kmeans: kmeans.cpp kmeans.h
	${CC} ${CFLAGS} kmeans.cpp -o kmeans

benchmark: benchmark.cpp kmeans.h
	${CC} ${CFLAGS} benchmark.cpp -o benchmark

.PHONY: clean gen plot test

clean:
	rm -f kmeans test/*_o.txt

test:
	@rm -f test/test${ID}.out \
	&& ./kmeans ./test/test${ID}.in  test/test${ID}.out

cmp:
	@rm -f test/test${ID}.out \
	&& rm -f test/test${ID}_s.out \
	&& rm -f test/test${ID}_l.out \
	&& rm -f test/test${ID}_x.out \
	&& echo "==== PKMeans ====" \
	&& ./kmeans ./test/test${ID}.in  test/test${ID}.out \
	&& echo "=== Lzhy ===" \
	&& ./kmeans_l ./test/test${ID}.in  test/test${ID}_l.out \
	&& echo "=== Xky ===" \
	&& ./kmeans_x ./test/test${ID}.in  test/test${ID}_x.out \
	&& echo "=== Benchmark ===" \
	&& ./benchmark ./test/test${ID}.in  test/test${ID}_s.out

gen: generate.py
	python3 generate.py ${FILE}

plot: plot.py
	python3 plot.py ${FILE}

submission:
	tar czvf project3.tar .git
