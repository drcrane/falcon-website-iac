#!/bin/sh
openssl pkcs12 -export -nodes -out bengreen.eu.pfx -inkey bengreen.eu.key -in bengreen.eu.cert -passout pass:

