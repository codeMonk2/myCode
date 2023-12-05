CC = nvcc
SRCS = ./src/*.cpp
SRCS += ./src/*.cu
INC = ./src/
OPTS = -std=c++14 --Werror all-warnings -arch=sm_75 -O3

EXEC = bin/kmeans

all: clean compile

compile:
	$(CC) $(SRCS) $(OPTS) -I$(INC) -o $(EXEC)

clean:
	rm -f $(EXEC)
