# *******************************************************************
#
# Tcl/Tk-Client fuer Tetrisd-Grid
# ===============================
#        Uwe Berger, 2018
#
#
#
# Programm-Optionen (es ist wirklich -- gemeint!!!):
# ------------------
# --h  --> MQTT-Host 				--> Default: localhost
# --p  --> MQTT-Port 				--> Default: 1883
# --U  --> MQTT-User 				--> Default: ""
# --P  --> MQTT-Password 			--> Default: ""
# --q  --> QoS (Sub/Pub) 			--> Default: 0 (Wertebereich: 0...1)
# --i  --> MQTT-Client_ID			--> Default: tcl_grid
#
# ---------
# Have fun!
#
# *******************************************************************

#!/bin/sh
# the next line restarts using wish \
exec wish "$0" "$@"

# thanks: https://chiselapp.com/user/schelte/repository/mqtt/home
source  mqtt.tcl

# Geometrie
set gvar(bx)    			40
set gvar(by)    			$gvar(bx)
set gvar(border) 			2
set gvar(fx)				10
set gvar(fy)    			20
set gvar(fxy)   			[expr $gvar(fx) * $gvar(fy)]
set gvar(border_next) 		$gvar(border)

set gvar(score_pady)		10

# Fenstergroesse
set gvar(dx) [expr ($gvar(bx) * $gvar(fx)) + ($gvar(border) * ($gvar(fx)+1))]
set gvar(dy) [expr ($gvar(by) * $gvar(fy)) + ($gvar(border) * ($gvar(fy)+1))]

# Farbdefinitionen
set gvar(colors) {gray10 green darkblue yellow red brown magenta cyan}
set gvar(bg_color_score) 	lightgrey


# Fonts
set gvar(score_label_font)	{-size 8}
set gvar(score_font) 		{-size 18}

# Topics 
set topic_grid 			"tetris/grid"
set topic_next 			"tetris/next_brick"
set topic_level 		"tetris/score/level"
set topic_points 		"tetris/score/points"
set topic_time 			"tetris/score/time"
set topic_lines 		"tetris/score/lines"
set topic_bricks 		"tetris/score/bricks"
set topic_gamestatus 	"tetris/gamestatus"
set topic_get_score 	"tetris/get_score"


# ********************************************************************
proc getopt {_argv name {_var ""} {default ""}} {
    upvar 1 $_argv argv $_var var
    set pos [lsearch -regexp $argv ^$name]
    if {$pos>=0} {
        set to $pos
        if {$_var ne ""} {
            set var [lindex $argv [incr to]]
        }
        set argv [lreplace $argv $pos $to]
        return 1
    } else {
        if {[llength [info level 0]] == 5} {set var $default}
        return 0
    }
}


# ********************************************************************
#proc set_brick_color {id c} {
#	global gvar
#	.grid itemconfigure b$id -fill [lindex $gvar(colors) $c]
#	.grid itemconfigure b$id -outline [lindex $gvar(colors) $c]
#}


# ********************************************************************
proc callback_mqtt_sub_grid {t p} {
	global gvar
	# alle Felder auf Grundfarbe stellen
	.grid itemconfigure all -fill [lindex $gvar(colors) 0]
	.grid itemconfigure all -outline [lindex $gvar(colors) 0]
	# Message "durchgehen", aber nur bei Farben <> Grundfarbe etwas tun
	set bl [split $p ""]
	for {set i 0} {($i < [llength $bl])&&($i < $gvar(fxy))} {incr i} {
		set c [lindex $bl $i]
		if {$c > 0} {
			.grid itemconfigure b$i -fill [lindex $gvar(colors) $c]
			.grid itemconfigure b$i -outline [lindex $gvar(colors) $c]
		}
	}
}

# ********************************************************************
proc callback_mqtt_sub_next {t p} {
	global gvar
	# alle Felder auf Grundfarbe stellen
	.score.next itemconfigure all -fill $gvar(bg_color_score)
	.score.next itemconfigure all -outline $gvar(bg_color_score)
	# Message "durchgehen", aber nur bei Farben <> Grundfarbe etwas tun
	set bl [split $p ""]
	for {set i 0} {($i < [llength $bl])&&($i < 16)} {incr i} {
		set c [lindex $bl $i]
		if {$c > 0} {
			.score.next itemconfigure n$i -fill [lindex $gvar(colors) $c]
			.score.next itemconfigure n$i -outline [lindex $gvar(colors) $c]
		}
	}
}

# ********************************************************************
proc callback_mqtt_sub_level {t p} {
	.score.level configure -text $p
}

# ********************************************************************
proc callback_mqtt_sub_points {t p} {
	.score.points configure -text $p
}

# ********************************************************************
proc callback_mqtt_sub_time {t p} {
	.score.time configure -text "$p\s"
}

# ********************************************************************
proc callback_mqtt_sub_lines {t p} {
	.score.lines configure -text $p
}

# ********************************************************************
proc callback_mqtt_sub_bricks {t p} {
	.score.bricks configure -text $p
}

# ********************************************************************
proc callback_mqtt_sub_gamestatus {t p} {
	global gvar
	switch $p {
			0 		{.score.gamestatus configure -text "" -bg $gvar(bg_color_score)}
			1 		{.score.gamestatus configure -text " Pause... " -bg yellow}
			2 		{.score.gamestatus configure -text " Game over! " -bg red}
			default {.score.gamestatus configure -text "...hmm...?"}
	}
}

# ********************************************************************
proc gui_init {} {
	global gvar
	# Fensterdefinitionen
	wm title . "Tetrisd Grid&Score (Uwe Berger, 2018)" 
	wm resizable . 0 0
	# leeres Spielfeld generieren/zeichnen
	canvas	.grid -width $gvar(dx) -height $gvar(dy) -bg black -bd 0
	pack .grid	-side left
	for {set i 0} {$i < $gvar(fx)} {incr i} {
		for {set j 0} {$j < $gvar(fy)} {incr j} {
			set x1 [expr $gvar(border) + $gvar(bx)*$i + $gvar(border)*$i]
			set y1 [expr $gvar(border) + $gvar(by)*$j + $gvar(border)*$j]
			set x2 [expr $x1 + $gvar(bx)]
			set y2 [expr $y1 + $gvar(by)]
			set id [expr $i*$gvar(fy) + $j]
			# puts "$i: $x1, $y1, $x2, $y2"
			.grid create rectangle 	$x1\
									$y1\
									$x2\
									$y2\
									-outline [lindex $gvar(colors) 0]\
									-fill [lindex $gvar(colors) 0]\
									-tags b$id
		}
	}
	
	# Score-Frame etc.
	frame .score
	pack .score	-side left -anchor n -padx 20 -pady 10
	
	# naechster Spielstein
	canvas	.score.next -width [expr ($gvar(bx)*4) + ($gvar(border_next)*5)]\
	              -height [expr ($gvar(by)*4) + ($gvar(border_next)*5)]\
	              -bd 0
	grid .score.next -column 0 -row 0 -columnspan 2 -padx 50
	for {set i 0} {$i < 4} {incr i} {
	for {set j 0} {$j < 4} {incr j} {
			set x1 [expr $gvar(border_next) + $gvar(bx)*$i + $gvar(border_next)*$i]
			set y1 [expr $gvar(border_next) + $gvar(by)*$j + $gvar(border_next)*$j]
			set x2 [expr $x1 + $gvar(bx)]
			set y2 [expr $y1 + $gvar(by)]
			set id [expr $j*4 + $i]
			# puts "$i: $x1, $y1, $x2, $y2"
			.score.next create rectangle 	$x1\
									$y1\
									$x2\
									$y2\
									-outline $gvar(bg_color_score)\
									-fill $gvar(bg_color_score)\
									-tags n$id
		}
	}

	# Level
	label .score.l_level -text "Level:" -font $gvar(score_label_font)
	grid .score.l_level -column 0 -row 1 -pady $gvar(score_pady) -sticky sw
	label .score.level -text "" -font $gvar(score_font)
	grid .score.level -column 1 -row 1 -pady $gvar(score_pady) -sticky e

	# Points
	label .score.l_points -text "Points:" -font $gvar(score_label_font)
	grid .score.l_points -column 0 -row 2 -pady $gvar(score_pady) -sticky sw
	label .score.points -text "" -font $gvar(score_font)
	grid .score.points -column 1 -row 2 -pady $gvar(score_pady) -sticky e

	# Time
	label .score.l_time -text "Time:" -font $gvar(score_label_font)
	grid .score.l_time -column 0 -row 3 -pady $gvar(score_pady) -sticky sw
	label .score.time -text "" -font $gvar(score_font)
	grid .score.time -column 1 -row 3 -pady $gvar(score_pady) -sticky e

	# Lines
	label .score.l_lines -text "Lines:" -font $gvar(score_label_font)
	grid .score.l_lines -column 0 -row 4 -pady $gvar(score_pady) -sticky sw
	label .score.lines -text "" -font $gvar(score_font)
	grid .score.lines -column 1 -row 4 -pady $gvar(score_pady) -sticky e

	# Bricks
	label .score.l_bricks -text "Bricks:" -font $gvar(score_label_font)
	grid .score.l_bricks -column 0 -row 5 -pady $gvar(score_pady) -sticky sw
	label .score.bricks -text "" -font $gvar(score_font)
	grid .score.bricks -column 1 -row 5 -pady $gvar(score_pady) -sticky e

	# Gamestatus
	label .score.gamestatus -text "" -font $gvar(score_font)
	grid .score.gamestatus -column 0 -row 6 -columnspan 2 -pady $gvar(score_pady)
}

# ********************************************************************
# ************* Main *************************************************
# ********************************************************************

# Kommandozeileparameter auswerten...
getopt argv --h host 		localhost
getopt argv --p port 		1883
getopt argv --U user 		""
getopt argv --P pwd  		""
getopt argv --i client_id   	"tcl_grid"
getopt argv --q qos 			0

# ...und/oder Plausibilitaet bzw. Wertebereiche ...
if {$qos < 0} {set qos 0}
if {$qos > 1} {set qos 1}

# Subscriber starten
set client_sub [mqtt new]
$client_sub configure -username $user -password $pwd 
$client_sub connect $client_id $host $port
$client_sub subscribe $topic_grid callback_mqtt_sub_grid $qos
$client_sub subscribe $topic_next callback_mqtt_sub_next $qos
$client_sub subscribe $topic_level callback_mqtt_sub_level $qos
$client_sub subscribe $topic_points callback_mqtt_sub_points $qos
$client_sub subscribe $topic_time callback_mqtt_sub_time $qos
$client_sub subscribe $topic_lines callback_mqtt_sub_lines $qos
$client_sub subscribe $topic_bricks callback_mqtt_sub_bricks $qos
$client_sub subscribe $topic_gamestatus callback_mqtt_sub_gamestatus $qos

# ...und Score via MQTT "anfordern"
$client_sub publish $topic_get_score "1" $qos

# GUI initialisieren
gui_init

