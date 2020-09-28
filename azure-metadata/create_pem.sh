#!/bin/sh
openssl pkcs12 -passin pass: -in bengreen.eu.pfx -nocerts -out bg.eu.key -nodes
openssl pkcs12 -passin pass: -in bengreen.eu.pfx -nokeys -out bg.eu.cert

