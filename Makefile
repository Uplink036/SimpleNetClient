CXXFLAGS      = -Wall -Isrc -Isrc/lib -Isrc/client -Isrc/server -g
LIBFLAGS      = -fPIC $(CXXFLAGS)
LD_FLAGS      = -Wall -L./ 

SRC_DIR       = ./src
TEST_DIR 	  = ./tests
BUILD_DIR     = ./build

CLIENT_DIR    = $(SRC_DIR)/client
CLIENT_OBJ    = c_main.o c_netparser.o
CLIENT_OBJS   = $(patsubst %,$(BUILD_DIR)/%,$(CLIENT_OBJ))

SERVER_DIR    = $(SRC_DIR)/server
SERVER_OBJ    = s_main.o s_task.o s_server.o s_protocol.o
SERVER_OBJS   = $(patsubst %,$(BUILD_DIR)/%,$(SERVER_OBJ))

LIBRARY_DIR   = $(SRC_DIR)/lib
LIBRARY_OBJ    = l_ip.o l_calc.o
LIBRARY_OBJS   = $(patsubst %,$(BUILD_DIR)/%,$(LIBRARY_OBJ))

all: libcalc manual_test client server ## Compile everything

$(BUILD_DIR)/s_%.o: $(SERVER_DIR)/%.cpp
	$(CXX) $(CXXFLAGS) -o $@ -c $^

$(BUILD_DIR)/c_%.o : $(CLIENT_DIR)/%.cpp
	$(CXX) $(CXXFLAGS) -o $@ -c $^

$(BUILD_DIR)/l_%.o : $(LIBRARY_DIR)/%.cpp
	$(CXX) $(LIBFLAGS) -o $@ -c $^

main.o: src/lib/main.cpp
	$(CXX) $(CXXFLAGS) -Isrc/lib -c src/lib/main.cpp -o $(BUILD_DIR)/main.o

test_client_object: client
	$(CXX) $(CXXFLAGS) -o $(BUILD_DIR)/t_ip.o -c $(TEST_DIR)/client/testIP.cpp 
	$(CXX) $(CXXFLAGS) -o $(BUILD_DIR)/t_netparser.o -c $(TEST_DIR)/client/testNetparser.cpp 

test_server_object: server
	$(CXX) $(CXXFLAGS) -o $(BUILD_DIR)/t_task.o -c $(TEST_DIR)/server/testTask.cpp 

tests: test_client_object test_server_object ## Compile all the tests
	$(CXX) $(CXXFLAGS) -o $(BUILD_DIR)/t_suit.o -c $(TEST_DIR)/test.cpp 
	{ \
		set -e; \
		FILES_TO_COMPILE=$$(find ./build/*.o ! -name "*_main.o");\
		$(CXX) $(LD_FLAGS) -fsanitize=address -o tests.out $$FILES_TO_COMPILE -lcommon; \
	}

manual_test: main.o ## Compile a test file of calculations
	$(CXX) $(LD_FLAGS) -o test $(BUILD_DIR)/main.o -lcommon

client: $(CLIENT_OBJS) lib ## Compile the client file
	$(CXX) $(LD_FLAGS) -o client $(BUILD_DIR)/c_*.o -lcommon

server: $(SERVER_OBJS) lib ## Compile the server file
	$(CXX) $(LD_FLAGS) -o server $(BUILD_DIR)/s_*.o -lcommon

lib: $(LIBRARY_OBJS) ## Generate the calc lib file (is needed)
	ar -rc libcommon.a -o $(BUILD_DIR)/l_*.o

libcalc: lib ## Generate the calc lib file (is needed)

clean: ## Clean generated files
	rm -f *.o  build/*.o
	rm -f *.a *.out

help: ## Show this help
	@grep -E '^[a-zA-Z_-]+:.*?## .*$$' $(MAKEFILE_LIST) | sort | awk 'BEGIN {FS = ":.*?## "}; {printf "\033[36m%-15s\033[0m %s\n", $$1, $$2}'