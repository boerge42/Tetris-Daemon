#!/bin/bash
# ***************************************************************************
# 
#      ...
# ================
# Uwe Berger; 2019
#
# ...vollkommen sinnfrei: alle Farben mit "komischen 4x4"-Zahlen" einmal 
# "durchschalten"...
#
# ...sorry fuer copy&paste, hatte keine Lust mir etwas "intellektuelles" 
# zu ueberlegen...;-)
#
# ---------
# Have fun!
#
# ***************************************************************************

while true
do
    mosquitto_pub -t 'tetris/next_brick' -m '1111100110011111'
    sleep 1
    mosquitto_pub -t 'tetris/next_brick' -m '1111000100010001'
    sleep 1
    mosquitto_pub -t 'tetris/next_brick' -m '1111011111101111'
    sleep 1
    mosquitto_pub -t 'tetris/next_brick' -m '1111011101111111'
    sleep 1
    mosquitto_pub -t 'tetris/next_brick' -m '1001100111110001'
    sleep 1
    mosquitto_pub -t 'tetris/next_brick' -m '1111111001111111'
    sleep 1
    mosquitto_pub -t 'tetris/next_brick' -m '1111100011111111'
    sleep 1
    mosquitto_pub -t 'tetris/next_brick' -m '1111000101110001'
    sleep 1
    mosquitto_pub -t 'tetris/next_brick' -m '1111111110011111'
    sleep 1
    mosquitto_pub -t 'tetris/next_brick' -m '1111111100011111'
    sleep 1
    mosquitto_pub -t 'tetris/next_brick' -m '2222200220022222'
    sleep 1
    mosquitto_pub -t 'tetris/next_brick' -m '2222000200020002'
    sleep 1
    mosquitto_pub -t 'tetris/next_brick' -m '2222022222202222'
    sleep 1
    mosquitto_pub -t 'tetris/next_brick' -m '2222022202222222'
    sleep 1
    mosquitto_pub -t 'tetris/next_brick' -m '2002200222220002'
    sleep 1
    mosquitto_pub -t 'tetris/next_brick' -m '2222222002222222'
    sleep 1
    mosquitto_pub -t 'tetris/next_brick' -m '2222200022222222'
    sleep 1
    mosquitto_pub -t 'tetris/next_brick' -m '2222000202220002'
    sleep 1
    mosquitto_pub -t 'tetris/next_brick' -m '2222222220022222'
    sleep 1
    mosquitto_pub -t 'tetris/next_brick' -m '2222222200022222'
    sleep 1
    mosquitto_pub -t 'tetris/next_brick' -m '3333300330033333'
    sleep 1
    mosquitto_pub -t 'tetris/next_brick' -m '3333000300030003'
    sleep 1
    mosquitto_pub -t 'tetris/next_brick' -m '3333033333303333'
    sleep 1
    mosquitto_pub -t 'tetris/next_brick' -m '3333033303333333'
    sleep 1
    mosquitto_pub -t 'tetris/next_brick' -m '3003300333330003'
    sleep 1
    mosquitto_pub -t 'tetris/next_brick' -m '3333333003333333'
    sleep 1
    mosquitto_pub -t 'tetris/next_brick' -m '3333300033333333'
    sleep 1
    mosquitto_pub -t 'tetris/next_brick' -m '3333000303330003'
    sleep 1
    mosquitto_pub -t 'tetris/next_brick' -m '3333333330033333'
    sleep 1
    mosquitto_pub -t 'tetris/next_brick' -m '3333333300033333'
    sleep 1
    mosquitto_pub -t 'tetris/next_brick' -m '4444400440044444'
    sleep 1
    mosquitto_pub -t 'tetris/next_brick' -m '4444000400040004'
    sleep 1
    mosquitto_pub -t 'tetris/next_brick' -m '4444044444404444'
    sleep 1
    mosquitto_pub -t 'tetris/next_brick' -m '4444044404444444'
    sleep 1
    mosquitto_pub -t 'tetris/next_brick' -m '4004400444440004'
    sleep 1
    mosquitto_pub -t 'tetris/next_brick' -m '4444444004444444'
    sleep 1
    mosquitto_pub -t 'tetris/next_brick' -m '4444400044444444'
    sleep 1
    mosquitto_pub -t 'tetris/next_brick' -m '4444000404440004'
    sleep 1
    mosquitto_pub -t 'tetris/next_brick' -m '4444444440044444'
    sleep 1
    mosquitto_pub -t 'tetris/next_brick' -m '4444444400044444'
    sleep 1
    mosquitto_pub -t 'tetris/next_brick' -m '5555500550055555'
    sleep 1
    mosquitto_pub -t 'tetris/next_brick' -m '5555000500050005'
    sleep 1
    mosquitto_pub -t 'tetris/next_brick' -m '5555055555505555'
    sleep 1
    mosquitto_pub -t 'tetris/next_brick' -m '5555055505555555'
    sleep 1
    mosquitto_pub -t 'tetris/next_brick' -m '5005500555550005'
    sleep 1
    mosquitto_pub -t 'tetris/next_brick' -m '5555555005555555'
    sleep 1
    mosquitto_pub -t 'tetris/next_brick' -m '5555500055555555'
    sleep 1
    mosquitto_pub -t 'tetris/next_brick' -m '5555000505550005'
    sleep 1
    mosquitto_pub -t 'tetris/next_brick' -m '5555555550055555'
    sleep 1
    mosquitto_pub -t 'tetris/next_brick' -m '5555555500055555'
    sleep 1
    mosquitto_pub -t 'tetris/next_brick' -m '6666600660066666'
    sleep 1
    mosquitto_pub -t 'tetris/next_brick' -m '6666000600060006'
    sleep 1
    mosquitto_pub -t 'tetris/next_brick' -m '6666066666606666'
    sleep 1
    mosquitto_pub -t 'tetris/next_brick' -m '6666066606666666'
    sleep 1
    mosquitto_pub -t 'tetris/next_brick' -m '6006600666660006'
    sleep 1
    mosquitto_pub -t 'tetris/next_brick' -m '6666666006666666'
    sleep 1
    mosquitto_pub -t 'tetris/next_brick' -m '6666600066666666'
    sleep 1
    mosquitto_pub -t 'tetris/next_brick' -m '6666000606660006'
    sleep 1
    mosquitto_pub -t 'tetris/next_brick' -m '6666666660066666'
    sleep 1
    mosquitto_pub -t 'tetris/next_brick' -m '6666666600066666'
    sleep 1
    mosquitto_pub -t 'tetris/next_brick' -m '7777700770077777'
    sleep 1
    mosquitto_pub -t 'tetris/next_brick' -m '7777000700070007'
    sleep 1
    mosquitto_pub -t 'tetris/next_brick' -m '7777077777707777'
    sleep 1
    mosquitto_pub -t 'tetris/next_brick' -m '7777077707777777'
    sleep 1
    mosquitto_pub -t 'tetris/next_brick' -m '7007700777770007'
    sleep 1
    mosquitto_pub -t 'tetris/next_brick' -m '7777777007777777'
    sleep 1
    mosquitto_pub -t 'tetris/next_brick' -m '7777700077777777'
    sleep 1
    mosquitto_pub -t 'tetris/next_brick' -m '7777000707770007'
    sleep 1
    mosquitto_pub -t 'tetris/next_brick' -m '7777777770077777'
    sleep 1
    mosquitto_pub -t 'tetris/next_brick' -m '7777777700077777'
    sleep 1
done
