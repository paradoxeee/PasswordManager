OUT = main

SRC_DIR = src
INCLUDE_DIR = include

SRC = $(wildcard $(SRC_DIR)/*.cpp)

CFLAGS = -O -Wall -std=c++17 -I$(INCLUDE_DIR)
CC = g++

OBJ = $(SRC:.cpp=.o)

$(OUT): $(OBJ)
	$(CC) $(CFLAGS) -o $(OUT) $(OBJ)

$(SRC_DIR)/%.o: $(SRC_DIR)/%.cpp
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OUT) $(SRC_DIR)/*.o

run: $(OUT)
	./$(OUT)
