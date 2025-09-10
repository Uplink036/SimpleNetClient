CC_FLAGS= -Wall -I. -Isrc/lib -Isrc/client -Isrc/server -g
CXXFLAGS= -Wall -I. -Isrc/lib -Isrc/client -Isrc/server -g
LD_FLAGS= -Wall -L./ 

SRC_DIR       = ./src
TEST_DIR 	  = ./tests
BUILD_DIR     = ./build

CLIENT_DIR    = $(SRC_DIR)/client
CLIENT_OBJ    = c_main.o c_ip.o
CLIENT_OBJS   = $(patsubst %,$(BUILD_DIR)/%,$(CLIENT_OBJ))

SERVER_DIR    = $(SRC_DIR)/server
SERVER_OBJ    = s_main.o
SERVER_OBJS   = $(patsubst %,$(BUILD_DIR)/%,$(SERVER_OBJ))

LIBRARY_DIR   = $(SRC_DIR)/lib

all: libcalc manual_test client server ## Compile everything

$(BUILD_DIR)/s_%.o: $(SERVER_DIR)/%.cpp
	$(CXX) $(CXXFLAGS) -o $@ -c $^

$(BUILD_DIR)/c_%.o : $(CLIENT_DIR)/%.cpp
	$(CXX) $(CXXFLAGS) -o $@ -c $^

main.o: lib/main.cpp
	$(CXX) $(CC_FLAGS) $(CFLAGS) -c lib/main.cpp -o $(BUILD_DIR)/main.o

tests_client:
	$(CXX) $(CC_FLAGS) -o $(BUILD_DIR)/t_client.o -c $(TEST_DIR)/client/*.cpp 

tests: client tests_client
	rm $(BUILD_DIR)/c_main.o
	$(CXX) $(LD_FLAGS) -o test_client.out $(BUILD_DIR)/c_*.o $(BUILD_DIR)/t_client.o -lcalc

manual_test: main.o calcLib.o ## Compile a test file of calculations
	$(CXX) $(LD_FLAGS) -o test.out $(BUILD_DIR)/main.o -lcalc

client: $(CLIENT_OBJS) calcLib.o ## Compile the client file
	$(CXX) $(LD_FLAGS) -o client.out $(BUILD_DIR)/c_*.o -lcalc

server: $(SERVER_OBJS) calcLib.o ## Compile the server file
	$(CXX) $(LD_FLAGS) -o server.out $(BUILD_DIR)/s_*.o -lcalc

calcLib.o: $(LIBRARY_DIR)/calcLib.c $(LIBRARY_DIR)/calcLib.h
	gcc -Wall -fPIC -c $(LIBRARY_DIR)/calcLib.c -o $(BUILD_DIR)/calcLib.o

libcalc: calcLib.o ## Generate the calc lib file (is needed)
	ar -rc libcalc.a -o $(BUILD_DIR)/calcLib.o

clean: ## Clean generated files
	rm -r *.o 
	rm *.a *.out

help: ## Show this help
	@grep -E '^[a-zA-Z_-]+:.*?## .*$$' $(MAKEFILE_LIST) | sort | awk 'BEGIN {FS = ":.*?## "}; {printf "\033[36m%-15s\033[0m %s\n", $$1, $$2}'