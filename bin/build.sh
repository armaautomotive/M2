

gcc -c -Wall -Werror -fpic ./src/module.c -o ./lib/module.o -lrt 
gcc -shared -o ./lib/libmodule.so ./lib/module.o

#gcc -L/var/www/bitcoinads.com/m2/lib -Wl,-rpath=/var/www/bitcoinads.com/m2/lib -Wall -o ./sample ./sample.c -lm2

gcc -o ./bin/m2 ./src/m2.c


