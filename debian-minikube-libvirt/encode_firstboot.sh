#!/bin/busybox ash
# Handle the way cloud_init expects base64 encoded data, we could put it on one line but this is prettier
base64 firstboot.sh |awk '{ if(NR==1){ print $0 }else{ print "      "$0 } }' > firstboot.sh.b64

