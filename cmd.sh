#!/bin/bash
set -x
./NodeStatusSvr -i http://192.168.8.224:9000/ndas/NodeResMonServerInit -g http://192.168.8.224:9000/ndas/NodeResMonGetList -p http://192.168.8.224:9000/ndas/NodeResMonReport $*
