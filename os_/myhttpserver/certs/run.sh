openssl genpkey -algorithm RSA -out server_private.key -pkeyopt rsa_keygen_bits:2048
openssl req -new -key server_private.key -out server.csr
openssl x509 -req -days 365 -in server.csr -signkey server_private.key -out server.crt
openssl x509 -in server.crt -out server_cert.pem -outform PEM
openssl rsa -in server_private.key -out server_key.pem -outform PEM
