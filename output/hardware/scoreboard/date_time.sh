#!/bin/bash

while true
do
    mosquitto_pub -t 'scoreboard/alternative' -m $(date +%d-%m-%y%H-%M-%S%u:%j:%W)
    sleep 1
done
