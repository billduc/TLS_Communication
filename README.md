# TLS_Communication

- g++ -Wall -o client client.cpp -L/usr/lib -lssl -lcrypto
./client <host_name> <port_number>

- g++ -Wall -o server server.cpp -L/usr/lib -lssl -lcrypto
./server <portnum>

- create a Certificate.
penssl req -x509 -nodes -days 365 -newkey rsa:1024 -keyout mycert.pem -out mycert.pem

- Install the OpenSSL library, for the ubuntu use the below command.

+ sudo apt-get install libssl–dev

- if errer Unable to locate package libssl-dev

+ dpkg --add-architecture i386
+ apt-get update
+ apt-get install libssl-dev:i386

- if error /usr/include/openssl/e_os2.h:56:33: fatal error: openssl/opensslconf.h: No such file or directory

+sudo apt-get install libssl-dev