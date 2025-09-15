CXXFLAGS      = -Wall -Isrc -Isrc/lib -Isrc/client -Isrc/server -g
LIBFLAGS      = -fPIC $(CXXFLAGS)
LD_FLAGS      = -Wall -L./ 

SRC_DIR       = ./src
TEST_DIR 	  = ./tests
BUILD_DIR     = ./build

CLIENT_DIR    = $(SRC_DIR)/client
CLIENT_OBJ    = c_main.o
CLIENT_OBJS   = $(patsubst %,$(BUILD_DIR)/%,$(CLIENT_OBJ))

SERVER_DIR    = $(SRC_DIR)/server
SERVER_OBJ    = s_main.o
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

main.o: lib/main.cpp
	$(CXX) $(CXXFLAGS) $(CFLAGS) -c lib/main.cpp -o $(BUILD_DIR)/main.o

tests_client:
	$(CXX) $(CXXFLAGS) -o $(BUILD_DIR)/t_client.o -c $(TEST_DIR)/client/*.cpp 

tests: client tests_client
	{ \
		set -e; \
		FILES_TO_COMPILE=$$(find ./build/*.o ! -name "*_main.o");\
		$(CXX) $(LD_FLAGS) -o test_client.out $$FILES_TO_COMPILE -lcommon; \
	}
manual_test: main.o calcLib.o ## Compile a test file of calculations
	$(CXX) $(LD_FLAGS) -o test.out $(BUILD_DIR)/main.o -lcommon

client: $(CLIENT_OBJS) lib ## Compile the client file
	$(CXX) $(LD_FLAGS) -o client.out $(BUILD_DIR)/c_*.o -lcommon

server: $(SERVER_OBJS) lib ## Compile the server file
	$(CXX) $(LD_FLAGS) -o server.out $(BUILD_DIR)/s_*.o -lcommon

lib: $(LIBRARY_OBJS) ## Generate the calc lib file (is needed)
	ar -rc libcommon.a -o $(BUILD_DIR)/l_*.o

libcalc: lib ## Generate the calc lib file (is needed)

clean: ## Clean generated files
	rm -f *.o  build/*.o
	rm -f *.a *.out

help: ## Show this help
	@grep -E '^[a-zA-Z_-]+:.*?## .*$$' $(MAKEFILE_LIST) | sort | awk 'BEGIN {FS = ":.*?## "}; {printf "\033[36m%-15s\033[0m %s\n", $$1, $$2}'