#!/bin/bash

while true
do
    mosquitto_pub -t 'tetris/next_brick' -m '3300330000000000'
    sleep 1
    mosquitto_pub -t 'tetris/next_brick' -m '2000222000000000'
    sleep 1
    mosquitto_pub -t 'tetris/next_brick' -m '0050555000000000'
    sleep 1
    mosquitto_pub -t 'tetris/next_brick' -m '0110110000000000'
    sleep 1
    mosquitto_pub -t 'tetris/next_brick' -m '4400044000000000'
    sleep 1
    mosquitto_pub -t 'tetris/next_brick' -m '6660060000000000'
    sleep 1
    mosquitto_pub -t 'tetris/next_brick' -m '0000777700000000'
    sleep 1
done
