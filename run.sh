#!/bin/bash
# -*- coding: utf-8 -*-


VERSION=0.1
NAME=$(basename $0)
NM=$0
AUTHOR="@svg153, @mrgarri, @roberseik (based on garquiscript.sh)"

CUR_DIR="$(pwd)"
bold=`tput bold`
normal=`tput sgr0`


UPDATE_source=
EDSU2015_source=http://laurel.datsi.fi.upm.es/~ssoo/SD.dir/practicas/edsu.tgz
EDSU2015_TRIQUI="@triqui3.fi.upm.es:~/DATSI/SD/EDSU.2015/"


FICH_TEMAS="fichero_temas"
PUERTO_IN=8000
PUERTO_EDSU=9000
SERVIDOR=localhost

FICH_IN_IN=intermediario.c
FICH_IN_CO=comun.c
FICH_IN_COh=comun.h
FICH_IN_dir=./intermediario/
#@TODO: FICH_IN={ ${FICH_IN_dir}${FICH_IN_*} } 
# -> expanda variables para que salga "./intermediario/intermediario.c ./intermediario/comun.c ./intermediario/comun.h"

FICH_ED_ED=editor.c
FICH_ED_TEST=test_editor.sh
FICH_ED_TEST_AVANZ=test_editor_avanz.sh
FICH_ED_dir=./editor/
#@TODO: FICH_ED={ ${FICH_ED_dir}${FICH_ED_ED} }

FICH_SU_SU=subscriptor.c
FICH_SU_edsuCO=edsu_comun.c
FICH_SU_edsuCOh=edsu_comun.h
FICH_SU_TEST=test_subscriptor.sh
FICH_SU_TEST_AVANZ=test_subscriptor_avanz.sh
FICH_SU_dir=./subscriptor/
#@TODO: FICH_SU={ ${FICH_SU_dir}${FICH_SU_SU} }

#@TODO: FICHEROS_BASE={ ${FICH_IN_IN} ${FICH_ED_ED} ${FICH_SU_SU} }

#@TODO: FICHEROS_ENVIAR={ ${FICH_IN} ${FICH_ED} ${FICH_SU} }
#@TODO: FICHEROS_TEST={ ${FICH_IN} ${FICH_ED} ${FICH_SU} }
#@TODO: FICHEROS_TEST_AVAN={ ${FICH_IN} ${FICH_ED} ${FICH_SU} }
#FICHEROS_ENVIAR=./intermediario/intermediario.c ./intermediario/comun.c ./intermediario/comun.h ./subscriptor/subscriptor.c ./subscriptor/edsu_comun.c ./subscriptor/edsu_comun.h ./editor/editor.c


HE="-h"
HELP="--help"

CO="-c"
COMP="--compile"
CIN="-cin"
CINTER="--compile-intermediario"
CSU="-csu"
CSUBSC="--compile-subscriptor"
CED="-ced"
CEDITO="--compile-editor"

R="-r"
RUN="--run"
RIN="-rin"
RINTER="--run-intermediario"
RSU="-rsu"
RSUBSC="--run-subscriptor"
RED="-red"
REDITO="--run-editor"
RA="-ra"
RAVANC="--run-avanced"
RASU="-rasu"
RASUBSC="--run-avanced-subscriptor"
RAED="-raed"
RAEDITO="--run-avanced-editor"


#### Comando para poder renombrar las tabs de guake
# gconftool-2 --set /apps/guake/general/use_vte_titles --type boolean false
# http://askubuntu.com/questions/254566/annoying-autorenaming-in-guake



function mostrar_descargando {
	echo -ne "Downloading files.  \r"
	sleep 0.5
	echo -ne "Downloading files.. \r"
	sleep 0.5
	echo -ne "Downloading files...\r"
	sleep 0.5
}

function download {
	wget --quiet --output-document=$1 $2 &
	while [[ $(ps | grep -c "wget") -gt 0 ]]
		do
			mostrar_descargando
	done
	printf "\rDone!                    \n"
}

function actualizar {
	# Download new version
	download $CUR_DIR/$NAME.tmp $UPDATE_source
	# Copy over modes from old version
	OCTAL_MODE=$(stat -c '%a' $0)
	chmod $OCTAL_MODE $CUR_DIR/$NAME.tmp
	# Overwrite old file with new
	mv -f $CUR_DIR/$NAME.tmp $NAME
	
	if [[ $? == 0 ]]
		then
			
			NEWVER=$(head ${NM} | grep "VERSION=" | sed 's/[^0-9.]//g')
			if [[ $NEWVER != $VERSION ]]
				then
					echo "Succesfully updated to version $NEWVER!"
				else
					echo "No updates found."
			fi
			exit 0
		else
			echo "There was an error performing the update, please try again later. Exit code: $?"
			exit 1
	fi
}



function texto_uso {
	printf "Usage:  ${bold}$0 { -h | -c[in|ed|su] | -r[in|ed|su] | -ra[in|ed|su] }${normal}\n"
}

function mostrar_uso {
	texto_uso
}



function print_opciones {
	printf "\t${bold}$CO or $COMP:${normal} Compila: \"${FICH_BASE}\".\n"
	printf "\t${bold}$CIN or $CINTER:${normal} Compila: \"${FICH_IN_IN}\" .\n"
	printf "\t${bold}$CED or $CEDITO:${normal} Compila: \"${FICH_ED_ED}\" .\n"
	printf "\t${bold}$CSU or $CSUBSC:${normal} Compila: \"${FICH_SU_SU}\" .\n"	
	printf "\n"
	printf "\t${bold}$R or $R:${normal} Compila y ejecuta: ..... .\n"
	printf "\t${bold}$RIN or $RINTER:${normal} Compila y ejecuta: \"${FICH_IN_IN}\" .\n"
	printf "\t${bold}$RED or $REDITO:${normal} Compila y ejecuta: \"${FICH_ED_TEST}\" .\n"
	printf "\t${bold}$RSU or $RSUBSC:${normal} Compila y ejecuta: \"${FICH_SU_TEST}\" .\n"	    
   	printf "\t${bold}$RA or $RAVANC:${normal} Compila y ejecuta: intermediario.c, subcriptor.c, editor.c, abriendo 3 terminales.\n"
	printf "\t${bold}$RAED or $RAEDITO:${normal} Compila y ejecuta: \"${FICH_ED_TEST_AVANZ}\" .\n"
	printf "\t${bold}$RASU or $RASUBSC:${normal} Compila y ejecuta: \"${FICH_SU_TEST_AVANZ}\" .\n"	 
    printf "\n"
	printf "\t${bold}-e or --send:${normal} Envia: los fichero: \"${FICHEROS_ENVIAR}\" a su cuenta de \"nMat$EDSU_TRIQUI\" .\n"
}

function print_otras_opciones {
	printf "\t${bold}-h or --help:${normal} Muestra el texto de uso o ayuda.\n"
	printf "\t${bold}-v or --version:${normal} Muestra la version actual del script.\n"
	printf "\t${bold}-u or --update:${normal} Actualiza el script a la ultima version.\n"
}

function print_acciones_script {
	printf "ACCIONES:\n"
	printf ${bold}
	printf " * Compila y ejecuta los archivos: intermediario.c, subcriptor.c, editor.c\n"
	printf " * Enviar a tu cuenta de triqui.fi.upm.es, los ficheros: ${FICHEROS}\n"
    printf " * Realiza en triqui.fi.upm.es la entrega de EDSU.2015, con los ${FICHEROS_ENTREGA}\n"
	printf ${normal}
}

function print_nombre {
	printf "${bold} $NAME ${normal} \n"
}

function mostrar_ayuda {
	print_acciones_script
	printf "\n"
	texto_uso
	printf "\n"
	
	printf "OPCIONES:\n"
		print_opciones
		printf "\n"
		
	printf "Otras opciones:\n"
		print_otras_opciones
		printf "\n"
	
	printf "You can ask my cat now how this script works and she'll just meaow you.\n"
	printf "\n"	
}

function compilar_intermediario {
	cd ./intermediario
	make
	cd ..
}

function compilar_editor {
	cd ./editor
	make
	cd ..
}

function compilar_subscriptor {
	cd ./subscriptor
	make
	cd ..
}

function compilar_all {
	compilar_intermediario
	compilar_editor
	compilar_subcriptor
}


function run_guake_intermediario {
	guake -s 1 --execute-command="./intermediario $PUERTO_IN $FICH_TEMAS"
}

function run_guake_export {
	guake -s 2 --execute-command="export PUERTO=$PUERTO_EDSU"
	guake -s 2 --execute-command="export PUERTO=$SERVIDOR"
}

function run_guake_editor {
	#run_guake_export(2)
	guake -s 2 --execute-command="export PUERTO=$PUERTO_EDSU"
	guake -s 2 --execute-command="export PUERTO=$SERVIDOR"
	guake -s 2 --execute-command="./test_editor"
}

#@TODO: cambiar para que sea una sola "run_guake_edsu(fichero, idVentana)"
function run_guake_subscriptor {
	#run_guake_export(3)
	guake -s 3 --execute-command="export PUERTO=$PUERTO_EDSU"
	guake -s 3 --execute-command="export PUERTO=$SERVIDOR"
	guake -s 3 --execute-command="./test_subscriptor"
}

function run_guake {
	run_guake_intermediario
	run_guake_editor
	run_guake_subscriptor
}
function run_guake {
	run_guake_intermediario
	run_guake_editor
	run_guake_subscriptor
}

function levantar_guake {
	guake -n "tab1" -r editor --execute-command="cd \"$route/editor\""
	guake -n "tab2" -r intermediario --execute-command="cd \"$route/intermediario\""
	guake -n "tab3" -r subscriptor --execute-command="cd \"$route/subscriptor\""
}


function run_intermediario {
	cd ./intermediario
    ./intermediario $PUERTO_IN $FICH_TEMAS
	cd ..
}

function run_editor {
	cd ./editor
	./test_editor
	cd ..
}

function run_subscriptor {
	cd ./subscriptor
	./test_subscriptor
	cd ..
}

function run_all {
	levantar_guake
	run_guake
}

function run_avanced_editor {
	cd ./editor
	./test_editor_avanz
	cd ..
}

function run_avanced_subscriptor {
	cd ./subscriptor
	./test_subscriptor_avanz
	cd ..
}

function run_avanced_all {
	levantar_guake
	run_guake_avanced
}

function print_info_envio {
	printf "Se van a enviar los ficheros:\n"
	printf "\t${bold}\"${FICHEROS_ENVIAR}\"\n"
	printf "a su cuenta de:\n"
	printf "\t${bold}\"nMat@triqui3.fi.upm.es:~/DATSI/SD/EDSU.2015/\"\n"
	printf "para ello, tendrá que instroduccir su contraseña para poder enviarlos al servidor.\n"
	printf "${bold}{¿Esta seguro de querer continuar? [Y/n].}\n"
}

function enviar_aTriqui {

	printf "Por favor introucca su cuenta en triqui. Ejemplo: t110025.\n"
	printf "\n"
				
	while [[ -z "$USER" ]] ; do
		read USER_IMPUT
		if [[ $input == "N" || $input == "n" || $input == "" ]] ; then
			printf "${bold}No se realizara el envio.\n"
			printf "Se cerrara el script.\n"
			printf "Adios.\n"
			exit 0
		else
			printf "${bold}Por favor, introduca un caracter valido [Y/n].\n"
		fi
	done
	
	scp ${FICHEROS_ENVIAR} $USER@triqui3.fi.upm.es:~/DATSI/SD/EDSU.2015/
}

function aTriqui {
	print_info_envio

	while [[ -z "$USER_IMPUT" ]] ; do
		read USER_IMPUT
		if [[ $USER_IMPUT == "N" || $USER_IMPUT == "n" || $USER_IMPUT == "" ]] ; then
			printf "${bold}No se realizara el envio.\n"
			printf "Se cerrara el script.\n"
			printf "Adios.\n"
			exit 0	
		elif [[ $input == "Y" || $input == "y" ]] ; then
			enviar_aTriqui
		else
			printf "${bold}Por favor, introduca un caracter valido [Y/n].\n"
		fi
	done
		

	
}

################################################
# ------ MAIN
################################################

case "$1" in
	"$HE")
    	mostrar_uso
    	exit 0
        ;;
    "$HELP")
    	mostrar_ayuda
    	exit 0
        ;;
        
# ------ COMPILE
    "$CO" | "$COMP")
        compilar_all
        exit 0
        ;;
     
    "$CIN" | "$CINTER")
        compilar_intermediario
        exit 0
        ;;
       
    "$CED" | "$CEDITO")
	    compilar_editor
        exit 0
        ;;
        
    "$CSU" | "$CSUBSC")
		compilar_subcriptor
		exit 0
        ;;
        
# ------ RUN
    "$R" | "$RUN")
        compilar_all
        run_all
        exit 0
        ;;
     
    "$RIN" | "$RINTER")
        compilar_intermediario
        run_intermediario
        exit 0
        ;;
       
    "$RED" | "$REDITO")
        compilar_editor
        run_editor
        exit 0
        ;;
        
    "$RSU" | "$RSUBSC")
        compilar_subscriptor
        run_subscriptor
		exit 0
        ;;

# ------ RUN AVANCED
    "$RA" | "$RAVANC")
        compilar_all
        run_avanced_all
        exit 0
        ;;
        
    "$RAED" | "$RAEDITO")
        compilar_editor
        run_avanced_editor
        exit 0
        ;;
        
    "$RASU" | "$RASUBSC")
        compilar_subscriptor
        run_avanced_subscriptor
		exit 0
        ;;        

    *)
        mostrar_ayuda
        exit 0
        
esac

################################################
# ------ MAIN
################################################

