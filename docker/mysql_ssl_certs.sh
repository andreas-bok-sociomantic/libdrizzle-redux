#!/bin/bash

# script for generating SSL certificates automatically
#
# https://dev.mysql.com/doc/refman/5.7/en/creating-ssl-files-using-openssl.html
# https://dev.mysql.com/doc/refman/5.7/en/using-encrypted-connections.html

# Common Name CN must be different from server and client certificate
# combine server-cert and client-cert:   ca.pem
#   cat server-cert.pem and client-cert.pem > ca.pem
# https://serverfault.com/questions/399487/cant-connect-to-mysql-using-self-signed-ssl-certificate

SSL_CERT_CONFIG="/tmp/ssl_cert"

write_config()
{
    echo "$SSL_CERT_CONFIG""-$1.cfg"
    cat <<EOF > "$SSL_CERT_CONFIG""-$1.cfg"
[ req ]
distinguished_name="libdrizzle_redux"
prompt="no"

[ libdrizzle_redux ]
C="DE"
ST="Berlin"
L="Berlin"
O="Drizzle Developer Group"
CN="https://github.com/sociomantic-tsunami/libdrizzle-redux/$1"
EOF
}

rm -rf ssl_certs
mkdir ssl_certs && cd ssl_certs

write_config client
write_config server

# Create CA certificate
openssl genrsa 2048 > ca-key.pem
openssl req -config "$SSL_CERT_CONFIG-server.cfg" -new -x509 -nodes -days 3600 \
        -key ca-key.pem -out ca.pem

# Create server certificate, remove passphrase, and sign it
# server-cert.pem = public key, server-key.pem = private key
openssl req -config "$SSL_CERT_CONFIG-server.cfg" -newkey rsa:2048 -days 3600 \
        -nodes -keyout server-key.pem -out server-req.pem
openssl rsa -in server-key.pem -out server-key.pem
openssl x509 -req -in server-req.pem -days 3600 \
        -CA ca.pem -CAkey ca-key.pem -set_serial 01 -out server-cert.pem

# Create client certificate, remove passphrase, and sign it
# client-cert.pem = public key, client-key.pem = private key
openssl req -config "$SSL_CERT_CONFIG-client.cfg" -newkey  rsa:2048 -days 3600 \
        -nodes -keyout client-key.pem -out client-req.pem
openssl rsa -in client-key.pem -out client-key.pem
openssl x509 -req -in client-req.pem -days 3600 \
        -CA ca.pem -CAkey ca-key.pem -set_serial 01 -out client-cert.pem
