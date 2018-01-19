# TLS_Communication

This code using c++ (:shipit:) to implement TLS communication.


### build and excute server side


```
 g++ -Wall -o server server.cpp -L/usr/lib -lssl -lcrypto
 sudo ./server <portnum>
```

### build and excute client side

```
 g++ -Wall -o client client.cpp -L/usr/lib -lssl -lcrypto
 ./client <host_name> <port_number>
```

Note
> to use wireshark to verify handshark TLS1.2 protocal. you need to used port number less than 1024 with root permission. 

### use wireshark to show communication.
![tls1.2_handshark](https://github.com/billduc/screenshot/blob/master/tls1.2_handshark.png)