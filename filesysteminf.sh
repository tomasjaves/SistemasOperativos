#!/bin/bash
# FILTROS
TEXT_RED=$(tput setaf 1)
TEXT_DEEPGREEN=$(tput setaf 2)
TEXT_YELLOW=$(tput setaf 3)
TEXT_BLUE=$(tput setaf 4)
TEXT_PURPLE=$(tput setaf 5)
TEXT_CIAN=$(tput setaf 6)
TEXT_WHITE=$(tput setaf 7)
TEXT_GREY=$(tput setaf 8)
TEXT_SALMON=$(tput setaf 9)
TEXT_GREEN=$(tput setaf 10)
TEXT_RESET=$(tput sgr0)
TEXT_BLACK=$(tput bold)
TEXT_ULINE=$(tput sgr 0 1)

#ARGUMENTOS
ARGUMENTO=$1

# FUNCIONES
tabla()
{
echo "${TEXT_YELLOW}TIPOS DE SISTEMAS DE ARCHIVOS MONTADOS EN EL SISTEMA$TEXT_RESET"
echo "${TEXT_GREEN}Type  | Filesystem  | Used   | Mounted On | Number of type | Number major/minor${TEXT_RESET}"
system=$(mount | cut -d' ' -f5 | sort | uniq)
muestra=
for tipo in $system;
        do
        a_ejercicio=$(df -aht $tipo | tr -s ' ' ' ' | cut -d' ' -f1,3,6 | sort -k2 -n | tail -n 1)
        c_ejercicio=$(mount | cut -d' ' -f5 | sort | grep $tipo | wc -w)
        a2_ejercicio=$(df -aht $tipo | tr -s ' ' ' ' | sort -k2 -n | cut -d' ' -f6 | tail -n 1)
        mayor=$(stat -c %t ${a2_ejercicio})
        if [ "$mayor" == "0" ]; then
                mayor=\'*\'
        fi
        menor=$(stat -c %T ${a2_ejercicio})
        if [ "$menor" == "0" ]; then
                menor=\'*\'
        fi
        muestra+="${tipo} ${a_ejercicio} ${c_ejercicio} ${mayor} ${menor}\n"
        done
echo -e $muestra | column -t
}

usage()
{
echo "${TEXT_ULINE}${TEXT_BLACK}OPCIONES${TEXT_RESET}${TEXT_ULINE}:
1) -h 
2) -inv
3) -devicefiles
4) -u${TEXT_RESET}
Si ${TEXT_YELLOW}no se elige opcion${TEXT_RESET}, muestra una tabla con todos los tipos de sistemas de archivos que están montados en el sistema con diferente información.
Si la opción es ${TEXT_YELLOW}-h${TEXT_RESET}, se ofrece una ayuda
Si la opción es ${TEXT_YELLOW}-inv${TEXT_RESET}, cambia el orden en el que se muestran la tabla original.
Si la opción es ${TEXT_YELLOW}-devicefiles${TEXT_RESET}, muestra una tabla con los archivos con los dispositivos representados en el sistema operativo como archivos (DEVICE FILES)
Si la opción es ${TEXT_YELLOW}-u${TEXT_RESET}, muestra los archivos abiertos por el usuario real del proceso"
}

inv()
{
echo "${TEXT_YELLOW}TIPOS DE SISTEMAS DE ARCHIVOS MONTADOS EN EL SISTEMA (INVERSA)$TEXT_RESET"
echo "${TEXT_GREEN}Type  | Filesystem  | Used   | Mounted On | Number of type | Number major/minor${TEXT_RESET}"
        system=$(mount | cut -d' ' -f5 | sort -r | uniq)
        muestra=
        for tipo in $system;
        do
	a_ejercicio=$(df -aht $tipo | tr -s ' ' ' ' | cut -d' ' -f1,3,6 | sort -k2 -n | tail -n 1)
	c_ejercicio=$(mount | cut -d' ' -f5 | sort | grep $tipo | wc -w)
	a2_ejercicio=$(df -aht $tipo | tr -s ' ' ' ' | sort -k2 -n | cut -d' ' -f6 | tail -n 1)
        mayor=$(stat -c %t ${a2_ejercicio})
        if [ "$mayor" == "0" ]; then
                mayor=\'*\'
        fi
        menor=$(stat -c %T ${a2_ejercicio})
        if [ "$menor" == "0" ]; then
                menor=\'*\'
        fi
        muestra+="${tipo} ${a_ejercicio} ${c_ejercicio} ${mayor} ${menor}\n"
        done
        echo -e $muestra | column -t
}

error_exit()
{
	echo "No se pudo ejecutar esa opción, prueba con -h, -inv, -devicefiles ó -u (nombre usuario)"
        echo -n "$1" 1>&2
        exit 1
}

device_files()
{
	echo "${TEXT_YELLOW}DISPOSITIVOS REPRESENTADOS EN EL SISTEMA OPERATIVO COMO ARCHIVOS (DEVICE FILES)${TEXT_RESET}"
	echo "${TEXT_PURPLE}Files | Amount${TEXT_RESET}"
	dev_file=$(lsof | tr -s ' ' ' ' | grep /dev | cut -d' ' -f1 | sort | uniq)
	muestra=
	for tipo in $dev_file
	do
	counter=$(lsof -c $tipo | grep /dev/ | sort | wc -l)
	muestra+="${tipo} ${counter}\n"
        done
        echo -e $muestra | column -t
}

usuario_proceso()
{	
	echo "${TEXT_YELLOW}ARCHIVOS ABIERTOS POR EL USUARIO REAL DEL PROCESO${TEXT_RESET}"
	echo "${TEXT_PURPLE}User | Process${TEXT_RESET}"
	proceso=$(lsof -u $ARGUMENTO | tr -s ' ' ' ' | grep /dev | cut -d' ' -f1 | sort | uniq)
	for tipo in $proceso
		do
		muestra+="${ARGUMENTO} ${tipo}\n"
		done
  	echo -e $muestra | column -t
}

# FUNCION PRINCIPAL
if [ "$ARGUMENTO" == "" ]; then
	tabla
else
	while [ "$ARGUMENTO" != "" ]; do
		case $ARGUMENTO in
			-h | --help)
				usage
				exit
				;;
			-inv)
				inv
				exit
				;;
			-devicefiles | -df)
				device_files
				exit
				;;
			-u)
				shift
				ARGUMENTO=$1
				usuario_proceso
				exit
				;;
			*)
				error_exit
				exit
				;;
		esac
		shift
	done
fi
