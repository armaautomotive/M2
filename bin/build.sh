

gcc -c -Wall -Werror -fpic ./src/module_c.c -o ./lib/module.o -lrt 
gcc -shared -o ./lib/libmodule.so ./lib/module.o


gcc -o ./bin/m2 ./src/unqlite.c ./src/users.c ./src/strings.c ./src/processes.c ./src/m2.c


