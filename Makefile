CC_FLAGS= -Wall -I. -Ilib
LD_FLAGS= -Wall -L./ 


all: libcalc test client server ## Compile everything

servermain.o: src/server/main.cpp
	$(CXX)  $(CC_FLAGS) $(CFLAGS) -c src/server/main.cpp -o serverMain.o

clientmain.o: src/client/main.cpp
	$(CXX) $(CC_FLAGS) $(CFLAGS) -c src/client/main.cpp -o clientMain.o

main.o: lib/main.cpp
	$(CXX) $(CC_FLAGS) $(CFLAGS) -c lib/main.cpp 


test: main.o calcLib.o ## Compile a test file of calculations
	$(CXX) $(LD_FLAGS) -o test.out main.o -lcalc

client: clientmain.o calcLib.o ## Compile the client file
	$(CXX) $(LD_FLAGS) -o client.out clientMain.o -lcalc

server: servermain.o calcLib.o ## Compile the server file
	$(CXX) $(LD_FLAGS) -o server.out serverMain.o -lcalc

calcLib.o: lib/calcLib.c lib/calcLib.h
	gcc -Wall -fPIC -c lib/calcLib.c -o calcLib.o

libcalc: calcLib.o ## Generate the calc lib file (is needed)
	ar -rc libcalc.a -o calcLib.o

clean: ## Clean generated files
	rm *.o *.a *.out

help: ## Show this help
	@grep -E '^[a-zA-Z_-]+:.*?## .*$$' $(MAKEFILE_LIST) | sort | awk 'BEGIN {FS = ":.*?## "}; {printf "\033[36m%-15s\033[0m %s\n", $$1, $$2}'