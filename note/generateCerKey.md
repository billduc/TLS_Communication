## generate CA cert and key files
* $openssl genrsa -des3 -out ca.key 4096
* $openssl req -new -x509 -days 365 -key ca.key -out ca.crt
 
## generate Server cert and key files
* $openssl genrsa -des3 -out ssl_server.key 4096
* $openssl req -new -key ssl_server.key -out ssl_server.csr
 
## Sign using CA
* $openssl x509 -req -days 365 -in ssl_server.csr -CA ca.crt -CAkey ca.key -set_serial 01 -out ssl_server.crt
 
## generate Client cert and key files
* $openssl genrsa -des3 -out ssl_client.key 4096
* $openssl req -new -key ssl_client.key -out ssl_client.csr
 
## Sign using CA
* $openssl x509 -req -days 365 -in ssl_client.csr -CA ca.crt -CAkey ca.key -set_serial 01 -out ssl_client.crt

## generate self-signed certificate
* openssl genrsa -out key.pem 2048
* openssl req -new -sha256 -key key.pem -out csr.csr
* openssl req -x509 -sha256 -days 365 -key key.pem -in csr.csr -out certificate.pem
* openssl req -in csr.csr -text -noout | grep -i "Signature.*SHA256" && echo "All is well" || echo "This certificate will stop working in 2017! You must update OpenSSL to generate a widely-compatible certificate"


The first OpenSSL command generates a 2048-bit (recommended) RSA private key.

The second command generates a Certificate Signing Request, which you could instead use to generate a CA-signed certificate. This step will ask you questions; be as accurate as you like since you probably aren’t getting this signed by a CA.

The third command generates a self-signed x509 certificate suitable for use on web servers. This is the file you were after all along, congrats!

The check at the end ensures you will be able to use your certificate beyond 2016. OpenSSL on OS X is currently insufficient, and will silently generate a SHA-1 certificate that will be rejected by browsers in 2017. Update using your package manager, or with Homebrew on a Mac and start the process over.