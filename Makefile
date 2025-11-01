CXXFLAGS      = -Wall -Isrc -Isrc/lib -Isrc/client -Isrc/server -g
LIBFLAGS      = -fPIC $(CXXFLAGS)
LD_FLAGS      = -Wall -L./ 

SRC_DIR       = ./src
TEST_DIR 	  = ./tests
BUILD_DIR     = ./build

CLIENT_DIR    		= $(SRC_DIR)/client
CLIENT_OBJS	  		= $(patsubst %.cpp,%.o,$(wildcard $(CLIENT_DIR)/*.cpp))
CLIENT_BUILD_OBJS   = $(patsubst $(CLIENT_DIR)/%,$(BUILD_DIR)/c_%,$(CLIENT_OBJS))

SERVER_DIR    		= $(SRC_DIR)/server
SERVER_OBJS	  		= $(patsubst %.cpp,%.o,$(wildcard $(SERVER_DIR)/*.cpp))
SERVER_BUILD_OBJS   = $(patsubst $(SERVER_DIR)/%,$(BUILD_DIR)/s_%,$(SERVER_OBJS))

LIBRARY_DIR   		= $(SRC_DIR)/lib
LIBRARY_OBJS	    = $(patsubst %.cpp,%.o,$(wildcard $(LIBRARY_DIR)/*.cpp))
LIBRARY_BUILD_OBJS  = $(patsubst $(LIBRARY_DIR)/%,$(BUILD_DIR)/l_%,$(LIBRARY_OBJS))

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

client: $(CLIENT_BUILD_OBJS) lib ## Compile the client file
	$(CXX) $(LD_FLAGS) -o client $(BUILD_DIR)/c_*.o -lcommon

server: $(SERVER_BUILD_OBJS) lib ## Compile the server file
	$(CXX) $(LD_FLAGS) -o server $(BUILD_DIR)/s_*.o -lcommon

lib: $(LIBRARY_BUILD_OBJS) ## Generate the calc lib file (is needed)
	ar -rc libcommon.a -o $(BUILD_DIR)/l_*.o

libcalc: lib ## Generate the calc lib file (is needed)

clean: ## Clean generated files
	rm -f *.o  build/*.o
	rm -f *.a *.out

help: ## Show this help
	@grep -E '^[a-zA-Z_-]+:.*?## .*$$' $(MAKEFILE_LIST) | sort | awk 'BEGIN {FS = ":.*?## "}; {printf "\033[36m%-15s\033[0m %s\n", $$1, $$2}'