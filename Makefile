CC_FLAGS= -Wall -I.
LD_FLAGS= -Wall -L./ 


all: libcalc test client server ## Compile everything

servermain.o: servermain.cpp
	$(CXX)  $(CC_FLAGS) $(CFLAGS) -c servermain.cpp 

clientmain.o: clientmain.cpp
	$(CXX) $(CC_FLAGS) $(CFLAGS) -c clientmain.cpp 

main.o: main.cpp
	$(CXX) $(CC_FLAGS) $(CFLAGS) -c main.cpp 


test: main.o calcLib.o ## Compile a test file of calculations
	$(CXX) $(LD_FLAGS) -o test.out main.o -lcalc

client: clientmain.o calcLib.o ## Compile the client file
	$(CXX) $(LD_FLAGS) -o client.out clientmain.o -lcalc

server: servermain.o calcLib.o ## Compile the server file
	$(CXX) $(LD_FLAGS) -o server.out servermain.o -lcalc


calcLib.o: calcLib.c calcLib.h
	gcc -Wall -fPIC -c calcLib.c

libcalc: calcLib.o ## Generate the calc lib file (is needed)
	ar -rc libcalc.a -o calcLib.o

clean: ## Clean generated files
	rm *.o *.a *.out

help: ## Show this help
	@grep -E '^[a-zA-Z_-]+:.*?## .*$$' $(MAKEFILE_LIST) | sort | awk 'BEGIN {FS = ":.*?## "}; {printf "\033[36m%-15s\033[0m %s\n", $$1, $$2}'