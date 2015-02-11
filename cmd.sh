#!/bin/bash
set -x
./NodeStatusSvr -i http://dbagent.cdn.imgo.tv/ndas/NodeResMonServerInit -g http://dbagent.cdn.imgo.tv/ndas/NodeResMonGetList -p http://dbagent.cdn.imgo.tv/ndas/NodeResMonReport $*
