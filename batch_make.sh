#!/bin/bash
# **********************************************************************
#
#          batch_make.sh
#        ================
#        Uwe Berger; 2018
#
#  * make ueber alle Unterverzeichnisse
#  * auszufuehrender make-Target wird als Aufrufparameter dem
#    Script mitgegeben
#
#  ---------
#  Have fun!
#
# **********************************************************************

# Ausgangsverzeichnis ermitteln
MYDIR=$(dirname "$(readlink -e "$0")")

# Make-Section aus Uebergabeparameter setzen
MAKE_TARGET=$1

# **********************************************************************
# Unterverzeichnisse durchlaufen
dir_recursiv() {
  for d in *; do
    if [ -d "$d" ]; then
      (cd -- "$d" && dir_recursiv)
		# Wenn eine Makefile vorhanden, dann make-Kommando absetzen
		if [ -f $d/Makefile ]; then
			echo "*** Makefile in $d gefunden ***"
			cd $d
			make $MAKE_TARGET
			cd ..
			echo ""
		fi
    fi
  done
}

# **********************************************************************
# **********************************************************************
# **********************************************************************
(cd $MYDIR; dir_recursiv)


