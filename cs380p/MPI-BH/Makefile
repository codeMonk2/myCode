CC = mpic++
SRCS = ./src/*.cpp
INC = ./src/
OPTS = -Werror -O3

EXEC = nbody

all: clean compile

compile:
	$(CC) $(SRCS) $(OPTS) -I$(INC) -o $(EXEC)

clean:
	rm -f $(EXEC)
