#!/bin/bash

while true
do
    mosquitto_pub -t 'tetris/score/points' -m $((RANDOM%100000))
    sleep 1
    mosquitto_pub -t 'tetris/score/lines' -m $((RANDOM%1000))
    sleep 1
    mosquitto_pub -t 'tetris/score/bricks' -m $((RANDOM%1000))
    sleep 1
    mosquitto_pub -t 'tetris/score/level' -m $((RANDOM%10))
    sleep 1
    mosquitto_pub -t 'tetris/score/time' -m $((RANDOM%10000))
	sleep 1
    mosquitto_pub -t 'tetris/gamestatus' -m $((RANDOM%10))
    sleep 1
done
