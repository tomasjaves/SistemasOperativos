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
ayuda=						# variable para la condicion -h
no_arg= 					# variable para la condicion de la ejecución del script sin argumento
usu= 							# variable para la condicion -u
usuario=					# variable para la condicion -u, toma todos los usuarios pasados por parámetros
invertir=					# variable para la condicion -inv
dev=							# variable para la condicion -devicefiles
no_header=    		# variable para la condicion -noheader
no_valido=				# variable para la condicion erronea
s_open=						# variable para la condicion -sopen
s_device=					# variable para la condicion -sdevice

# FUNCIONES
tabla() # si no se le pasan parámetros al script, se muestra esta tabla.
{
muestra=  																		# creamos una variable "muestra" donde almacenaremos todas las operaciones.
echo "${TEXT_YELLOW}TIPOS DE SISTEMAS DE ARCHIVOS MONTADOS EN EL SISTEMA$TEXT_RESET"
muestra+="${TEXT_GREEN}Type Filesystem  Used  Mounted_on Number_of_type Major Minor${TEXT_RESET}\n"
system=$(mount | cut -d' ' -f5 | sort | uniq) # cogemos todos los tipos de archivos que hay para poder iterar con ellos
for tipo in $system;  												# for de cada tipo dentro de "system" que almacena todos los tipos de archivos.
  do
  a_ejercicio=$(df -aht $tipo | tr -s ' ' ' ' | cut -d' ' -f1,3,6 | sort -k2 -n | tail -n 1) # obtenemos los campos 1(Tipo archivo), 3(Usado) y 6(Montado) de cada tipo de sistema
  c_ejercicio=$(mount | cut -d' ' -f5 | sort | grep $tipo | wc -w) 	# proporciona con wc cuantas veces existe cada tipo de sistema
	a2_ejercicio=$(mount | grep $tipo | cut -d' ' -f1 | grep /dev) 		# cogemos los archivos de dispositivos
  if [[ "$a2_ejercicio" ]]; then																		# si se cumple la condición de que existe un archivo de dispositivo, entonces se hace el nºmayor,menor.
	mayor=$(stat -c %t ${a2_ejercicio})
	mayor=$(echo "obase=10; ibase=16; $mayor;" | bc)
	menor=$(stat -c %T ${a2_ejercicio})
	menor=$(echo "obase=10; ibase=16; $menor;" | bc)
  else																															# si no se cumple la condición, asterisco en nºmayor,menor.
	mayor=\'*\'
	menor=\'*\'
	fi
  muestra+="${tipo} ${a_ejercicio} ${c_ejercicio} ${mayor} ${menor}\n"
  done
echo -e $muestra | column -t
}

usage() # si al script se le pasa como parámetro -h, se muestra el funcionamiento mediante esta función.
{
echo "${TEXT_ULINE}${TEXT_BLACK}OPCIONES${TEXT_RESET}${TEXT_ULINE}:
1) -h 
2) -inv
3) -devicefiles
4) -u
5) ${TEXT_RESET}
Si ${TEXT_YELLOW}no se elige opcion${TEXT_RESET}, muestra una tabla con todos los tipos de sistemas de archivos que están montados en el sistema con diferente información.
Si la opción es ${TEXT_YELLOW}-h${TEXT_RESET}, se ofrece una ayuda
Si la opción es ${TEXT_YELLOW}-inv${TEXT_RESET}, cambia el orden en el que se muestran la tabla original.
Si la opción es ${TEXT_YELLOW}-devicefiles${TEXT_RESET}, muestra una tabla con los archivos con los dispositivos representados en el sistema operativo como archivos (DEVICE FILES)
Si la opción es ${TEXT_YELLOW}-u${TEXT_RESET}, muestra los archivos abiertos por el usuario real del proceso"
}

inv() # si al script se le pasa como parámetro -inv, se muestra la tabla invertida.
{
	muestra=  																			 # creamos una variable "muestra" donde almacenaremos todas las operaciones.
	echo "${TEXT_YELLOW}TIPOS DE SISTEMAS DE ARCHIVOS MONTADOS EN EL SISTEMA (INVERSA)$TEXT_RESET"
	muestra+="${TEXT_GREEN}Type Filesystem  Used  Mounted_on Number_of_type Major Minor${TEXT_RESET}\n"
	system=$(mount | cut -d' ' -f5 | sort -r | uniq) # cogemos todos los tipos de archivos que hay para poder iterar con ellos
	for tipo in $system;														 # for de cada tipo dentro de "system" que almacena todos los tipos de archivos.
	do
	a_ejercicio=$(df -aht $tipo | tr -s ' ' ' ' | cut -d' ' -f1,3,6 | sort -k2 -n | tail -n 1) # obtenemos los campos 1(Tipo archivo), 3(Usado) y 6(Montado) de cada tipo de sistema
	c_ejercicio=$(mount | cut -d' ' -f5 | sort | grep $tipo | wc -w)  # proporciona con wc cuantas veces existe cada tipo de sistema
	a2_ejercicio=$(mount | grep $tipo | cut -d' ' -f1 | grep /dev)    # cogemos los archivos de dispositivos
  if [[ "$a2_ejercicio" ]]; then																		# si se cumple la condición de que existe un archivo de dispositivo, entonces se hace el nºmayor,menor.
		mayor=$(stat -c %t ${a2_ejercicio})
		mayor=$(echo "obase=10; ibase=16; $mayor;" | bc)
		menor=$(stat -c %T ${a2_ejercicio})
		menor=$(echo "obase=10; ibase=16; $menor;" | bc)
  else																															# si no se cumple la condición, asterisco en nºmayor,menor.
		mayor=\'*\'
		menor=\'*\'
  fi
  muestra+="${tipo} ${a_ejercicio} ${c_ejercicio} ${mayor} ${menor} \n"
  done
  echo -e $muestra | column -t
}

error_exit() # si hay algún error con las opciones, se ejecuta esta función.
{
	echo "No se pudo ejecutar esa opción, prueba con -h, -inv, -devicefiles ó -u (nombre usuario)"
        echo -n "$1" 1>&2
        exit 1
}

device_files() # si al script se le pasa como parámetro -devicefiles, se muestra la tabla de los device files
{
  muestra=  																		# creamos una variable "muestra" donde almacenaremos todas las operaciones.
	cuenteo=0																			# creamos una variable "cuenteo" donde almacenaremos el total de los archivos abiertos.
	echo "${TEXT_YELLOW}DISPOSITIVOS REPRESENTADOS EN EL SISTEMA OPERATIVO COMO ARCHIVOS (DEVICE FILES)${TEXT_RESET}"
	muestra+="${TEXT_PURPLE}Files Filesystem Major Minor Open_files ${TEXT_RESET}\n"
	system=$(mount | cut -d' ' -f5 | sort | uniq) # cogemos todos los tipos de archivos que hay para poder iterar con ellos
	for tipo in $system;													# for de cada tipo dentro de "system" que almacena todos los tipos de archivos.
	do
		a2_ejercicio=$(mount | grep $tipo | cut -d' ' -f1 | grep /dev/) 	 # cogemos los archivos de dispositivos
  	if [[ $a2_ejercicio ]]; then																			 # si se cumple la condición de que existe un archivo de dispositivo, entonces se hace el nºmayor,menor y hacemos el cuenteo.
   		mayor=$(stat -c %t ${a2_ejercicio})
			mayor=$(echo "obase=10; ibase=16; $mayor;" | bc)
			menor=$(stat -c %T ${a2_ejercicio})
			menor=$(echo "obase=10; ibase=16; $menor;" | bc)
			cuenteo=$(lsof ${a2_ejercicio} | wc -l)
		  muestra+="${tipo} ${a2_ejercicio} ${mayor} ${menor} ${cuenteo-1}" # a cuenteo se le resta uno por la cabecera.
		fi
		cuenteo=0
	done
  echo -e $muestra | column -t
}

user_table() # si al script se le pasa por parámetro -u (nombre), se muestra la tabla con los procesos de un usuario concreto.
{
	muestra=  																		# creamos una variable "muestra" donde almacenaremos todas las operaciones.
	cuenteo=0 																		# creamos una variable "cuenteo" donde almacenaremos el total de los archivos abiertos -1 (sin cabecera).
	almacen=0																		  # creamos una variable "almacen" donde almacenaremos el total de los archivos abiertos (con cabecera).
	system=$(mount | cut -d' ' -f5 | sort | uniq) # cogemos todos los tipos de archivos que hay para poder iterar con ellos.
	echo "${TEXT_YELLOW}TABLA DE ARCHIVOS DE DISPOSITIVO CON USUARIOS ESPECÍFICOS${TEXT_RESET}"
	muestra+="${TEXT_SALMON}Files Used Mounted_on Space_occupies Number_total Major Minor${TEXT_RESET}\n"
	echo $1
	for tipo in system; # for de cada tipo dentro de "system" que almacena todos los tipos de archivos.
	do
		if df -at $tipo | cut -d' ' -f1 | grep /dev/ &> /dev/null; # si existe un archivo de dispositivo del tipo, entonces
		then
			a_ejercicio=$(df -aht $tipo | tr -s ' ' ' ' | cut -d' ' -f1,3,6 | sort -k2 -n | tail -n 1) # obtenemos los campos 1(Tipo archivo), 3(Usado) y 6(Montado) de cada tipo de sistema 
			c_ejercicio=$(mount | cut -d' ' -f5 | sort | grep $tipo | wc -w) 							 # proporciona con wc cuantas veces existe cada tipo de sistema
			a2_ejercicio=$(mount | grep $tipo | cut -d' ' -f1 | grep /dev)   							 # cogemos los archivos de dispositivos
			contador=$(df -aht $tipo --total | tr -s ' ' ' ' | grep /dev/ | cut -d' ' -f3) # proporciona la ocupacion de cada tipo de sistema
			tipo_archivo=$(df -at $tipo | sort -k2 -n | cut -d' ' -f1 | tail -n 1) 				 # cogemos el archivo de dispositivo
			for user in $1;  						# for de cada usuario dentro de la cadena de usuarios.
			do
				if id $user &> /dev/null; # si existe la id de usuario, entonces
				then
					almacen=$(lsof -u $user $tipo_archivo | wc -l) # almacenamos en "almacen" el total de numero de archivos abiertos (con cabecera).
					cuenteo=${almacen} - 1  # puesto que nos coge todas las líneas y tenemos cabecera cuando ejecutamos 'lsof', restamos uno para quitarsela.
					if [[ "$a2_ejercicio" ]]; then																		# si se cumple la condición de que existe un archivo de dispositivo, entonces se hace el nºmayor,menor.
						mayor=$(stat -c %t ${a2_ejercicio})
						mayor=$(echo "obase=10; ibase=16; $mayor;" | bc)
						menor=$(stat -c %T ${a2_ejercicio})
						menor=$(echo "obase=10; ibase=16; $menor;" | bc)
					else																															# si no se cumple la condición, asterisco en nºmayor,menor.
						mayor=\'*\'
						menor=\'*\'
					fi
					muestra+="${a_ejercicio} ${contador} ${cuenteo} ${mayor} ${menor}\n"
					cuenteo=0 # le damos valor 0 para que reitere desde este valor.
				fi
			done
		fi
	done
	echo -e $muestra | column -t
}

table_no_header() # si al script se le pasa por parámetro -noheader, se muestra la tabla básica sin cabecera.
{
	system=$(mount | cut -d' ' -f5 | sort | uniq) # cogemos todos los tipos de archivos que hay para poder iterar con ellos.
	muestra=																			# creamos una variable "muestra" donde almacenaremos todas las operaciones.
	for tipo in $system;													# for de cada tipo dentro de "system" que almacena todos los tipos de archivos.
  do
    a_ejercicio=$(df -aht $tipo | tr -s ' ' ' ' | cut -d' ' -f1,3,6 | sort -k2 -n | tail -n 1) # obtenemos los campos 1(Tipo archivo), 3(Usado) y 6(Montado) de cada tipo de sistema
    c_ejercicio=$(mount | cut -d' ' -f5 | sort | grep $tipo | wc -w)  # proporciona con wc cuantas veces existe cada tipo de sistema
		a2_ejercicio=$(mount | grep $tipo | cut -d' ' -f1 | grep /dev)    # cogemos los archivos de dispositivos
		if [[ "$a2_ejercicio" ]]; then																		# si se cumple la condición de que existe un archivo de dispositivo, entonces se hace el nºmayor,menor.
			mayor=$(stat -c %t ${a2_ejercicio})
			mayor=$(echo "obase=10; ibase=16; $mayor;" | bc)
			menor=$(stat -c %T ${a2_ejercicio})
			menor=$(echo "obase=10; ibase=16; $menor;" | bc)
		else																															# si no se cumple la condición, asterisco en nºmayor,menor.
			mayor=\'*\'
			menor=\'*\'
		fi
  muestra+="${tipo} ${a_ejercicio} ${c_ejercicio} ${mayor} ${menor}\n"
  done
echo -e $muestra | column -t
}

table_noheader_invert() # si al script se le pasa por parámetro -noheader, se muestra la tabla invertida básica sin cabecera.
{
	system=$(mount | cut -d' ' -f5 | sort -r | uniq) # cogemos todos los tipos de archivos que hay para poder iterar con ellos.
	muestra= 																				 # creamos una variable "muestra" donde almacenaremos todas las operaciones.
	for tipo in $system;														 # for de cada tipo dentro de "system" que almacena todos los tipos de archivos.
	do
	a_ejercicio=$(df -aht $tipo | tr -s ' ' ' ' | cut -d' ' -f1,3,6 | sort -k2 -n | tail -n 1) # obtenemos los campos 1(Tipo archivo), 3(Usado) y 6(Montado) de cada tipo de sistema
	c_ejercicio=$(mount | cut -d' ' -f5 | sort | grep $tipo | wc -w)  # proporciona con wc cuantas veces existe cada tipo de sistema
	a2_ejercicio=$(mount | grep $tipo | cut -d' ' -f1 | grep /dev)    # cogemos los archivos de dispositivos
	if [[ "$a2_ejercicio" ]]; then																		# si se cumple la condición de que existe un archivo de dispositivo, entonces se hace el nºmayor,menor.
		mayor=$(stat -c %t ${a2_ejercicio})
		mayor=$(echo "obase=10; ibase=16; $mayor;" | bc)
		menor=$(stat -c %T ${a2_ejercicio})
		menor=$(echo "obase=10; ibase=16; $menor;" | bc)
	else																															# si no se cumple la condición, asterisco en nºmayor,menor.
		mayor=\'*\'
		menor=\'*\'
	fi
  muestra+="${tipo} ${a_ejercicio} ${c_ejercicio} ${mayor} ${menor} \n"
  done
  echo -e $muestra | column -t
}

table_noheader_device()
{
  muestra=  																		# creamos una variable "muestra" donde almacenaremos todas las operaciones.
	cuenteo=0																			# creamos una variable "cuenteo" donde almacenaremos el total de los archivos abiertos.
	system=$(mount | cut -d' ' -f5 | sort | uniq) # cogemos todos los tipos de archivos que hay para poder iterar con ellos
	for tipo in $system;													# for de cada tipo dentro de "system" que almacena todos los tipos de archivos.
	do
		a2_ejercicio=$(mount | grep $tipo | cut -d' ' -f1 | grep /dev/) 	 # cogemos los archivos de dispositivos
  	if [[ $a2_ejercicio ]]; then																			 # si se cumple la condición de que existe un archivo de dispositivo, entonces se hace el nºmayor,menor y hacemos el cuenteo.
   		mayor=$(stat -c %t ${a2_ejercicio})
			mayor=$(echo "obase=10; ibase=16; $mayor;" | bc)
			menor=$(stat -c %T ${a2_ejercicio})
			menor=$(echo "obase=10; ibase=16; $menor;" | bc)
			cuenteo=$(lsof ${a2_ejercicio} | wc -l)
		  muestra+="${tipo} ${a2_ejercicio} ${mayor} ${menor} ${cuenteo-1}" # a cuenteo se le resta uno por la cabecera.
		fi
		cuenteo=0
	done
  echo -e $muestra | column -t
}

table_noheader_user()
{
	muestra=  																		# creamos una variable "muestra" donde almacenaremos todas las operaciones.
	cuenteo=0 																		# creamos una variable "cuenteo" donde almacenaremos el total de los archivos abiertos -1 (sin cabecera).
	almacen=0																		  # creamos una variable "almacen" donde almacenaremos el total de los archivos abiertos (con cabecera).
	system=$(mount | cut -d' ' -f5 | sort | uniq) # cogemos todos los tipos de archivos que hay para poder iterar con ellos.
	echo $1
	for tipo in system; # for de cada tipo dentro de "system" que almacena todos los tipos de archivos.
	do
		if df -at $tipo | cut -d' ' -f1 | grep /dev/ &> /dev/null; # si existe un archivo de dispositivo del tipo, entonces
		then
			a_ejercicio=$(df -aht $tipo | tr -s ' ' ' ' | cut -d' ' -f1,3,6 | sort -k2 -n | tail -n 1) # obtenemos los campos 1(Tipo archivo), 3(Usado) y 6(Montado) de cada tipo de sistema 
			c_ejercicio=$(mount | cut -d' ' -f5 | sort | grep $tipo | wc -w) 							 # proporciona con wc cuantas veces existe cada tipo de sistema
			a2_ejercicio=$(mount | grep $tipo | cut -d' ' -f1 | grep /dev)   							 # cogemos los archivos de dispositivos
			contador=$(df -aht $tipo --total | tr -s ' ' ' ' | grep /dev/ | cut -d' ' -f3) # proporciona la ocupacion de cada tipo de sistema
			tipo_archivo=$(df -at $tipo | sort -k2 -n | cut -d' ' -f1 | tail -n 1) 				 # cogemos el archivo de dispositivo
			for user in $1;  						# for de cada usuario dentro de la cadena de usuarios.
			do
				if id $user &> /dev/null; # si existe la id de usuario, entonces
				then
					almacen=$(lsof -u $user $tipo_archivo | wc -l) # almacenamos en "almacen" el total de numero de archivos abiertos (con cabecera).
					cuenteo=${almacen} - 1  # puesto que nos coge todas las líneas y tenemos cabecera cuando ejecutamos 'lsof', restamos uno para quitarsela.
					if [[ "$a2_ejercicio" ]]; then																		# si se cumple la condición de que existe un archivo de dispositivo, entonces se hace el nºmayor,menor.
						mayor=$(stat -c %t ${a2_ejercicio})
						mayor=$(echo "obase=10; ibase=16; $mayor;" | bc)
						menor=$(stat -c %T ${a2_ejercicio})
						menor=$(echo "obase=10; ibase=16; $menor;" | bc)
					else																															# si no se cumple la condición, asterisco en nºmayor,menor.
						mayor=\'*\'
						menor=\'*\'
					fi
					muestra+="${a_ejercicio} ${contador} ${cuenteo} ${mayor} ${menor}\n"
					cuenteo=0 # le damos valor 0 para que reitere desde este valor.
				fi
			done
		fi
	done
	echo -e $muestra | column -t
}

table_device_invert()
{
  muestra=  																		# creamos una variable "muestra" donde almacenaremos todas las operaciones.
	cuenteo=0																			# creamos una variable "cuenteo" donde almacenaremos el total de los archivos abiertos.
	echo "${TEXT_YELLOW}DISPOSITIVOS REPRESENTADOS EN EL SISTEMA OPERATIVO COMO ARCHIVOS (DEVICE FILES)${TEXT_RESET}"
	muestra+="${TEXT_PURPLE}Files Filesystem Major Minor Open_files ${TEXT_RESET}\n"
	system=$(mount | cut -d' ' -f5 | sort | uniq) # cogemos todos los tipos de archivos que hay para poder iterar con ellos
	for tipo in $system;													# for de cada tipo dentro de "system" que almacena todos los tipos de archivos.
	do
		a2_ejercicio=$(mount | grep $tipo | cut -d' ' -f1 | grep /dev/) 	 # cogemos los archivos de dispositivos
  	if [[ $a2_ejercicio ]]; then																			 # si se cumple la condición de que existe un archivo de dispositivo, entonces se hace el nºmayor,menor y hacemos el cuenteo.
   		mayor=$(stat -c %t ${a2_ejercicio})
			mayor=$(echo "obase=10; ibase=16; $mayor;" | bc)
			menor=$(stat -c %T ${a2_ejercicio})
			menor=$(echo "obase=10; ibase=16; $menor;" | bc)
			cuenteo=$(lsof ${a2_ejercicio} | wc -l)
		  muestra+="${tipo} ${a2_ejercicio} ${mayor} ${menor} ${cuenteo-1}" # a cuenteo se le resta uno por la cabecera.
		fi
		cuenteo=0
	done
  echo -e $muestra | column -t | sort -r
}

sopen_device()
{
  muestra=  																		# creamos una variable "muestra" donde almacenaremos todas las operaciones.
	cuenteo=0																			# creamos una variable "cuenteo" donde almacenaremos el total de los archivos abiertos.
	echo "${TEXT_YELLOW}DISPOSITIVOS REPRESENTADOS EN EL SISTEMA OPERATIVO COMO ARCHIVOS (DEVICE FILES)${TEXT_RESET}"
	muestra+="${TEXT_PURPLE}Files Filesystem Major Minor Open_files ${TEXT_RESET}\n"
	system=$(mount | cut -d' ' -f5 | sort | uniq) # cogemos todos los tipos de archivos que hay para poder iterar con ellos
	for tipo in $system;													# for de cada tipo dentro de "system" que almacena todos los tipos de archivos.
	do
		a2_ejercicio=$(mount | grep $tipo | cut -d' ' -f1 | grep /dev/) 	 # cogemos los archivos de dispositivos
  	if [[ $a2_ejercicio ]]; then																			 # si se cumple la condición de que existe un archivo de dispositivo, entonces se hace el nºmayor,menor y hacemos el cuenteo.
   		mayor=$(stat -c %t ${a2_ejercicio})
			mayor=$(echo "obase=10; ibase=16; $mayor;" | bc)
			menor=$(stat -c %T ${a2_ejercicio})
			menor=$(echo "obase=10; ibase=16; $menor;" | bc)
			cuenteo=$(lsof ${a2_ejercicio} | wc -l)
		  muestra+="${tipo} ${a2_ejercicio} ${mayor} ${menor} ${cuenteo-1}" # a cuenteo se le resta uno por la cabecera.
		fi
		cuenteo=0
	done
  echo -e $muestra | column -t | sort -k5 -nr
}

sopen_device_user()
{
	muestra=  																		# creamos una variable "muestra" donde almacenaremos todas las operaciones.
	cuenteo=0 																		# creamos una variable "cuenteo" donde almacenaremos el total de los archivos abiertos -1 (sin cabecera).
	almacen=0																		  # creamos una variable "almacen" donde almacenaremos el total de los archivos abiertos (con cabecera).
	system=$(mount | cut -d' ' -f5 | sort | uniq) # cogemos todos los tipos de archivos que hay para poder iterar con ellos.
	echo "${TEXT_YELLOW}TABLA DE ARCHIVOS DE DISPOSITIVO CON USUARIOS ESPECÍFICOS${TEXT_RESET}"
	muestra+="${TEXT_SALMON}Files Used Mounted_on Space_occupies Number_total Major Minor${TEXT_RESET}\n"
	echo $1
	for tipo in system; # for de cada tipo dentro de "system" que almacena todos los tipos de archivos.
	do
		if df -at $tipo | cut -d' ' -f1 | grep /dev/ &> /dev/null; # si existe un archivo de dispositivo del tipo, entonces
		then
			a2_ejercicio=$(mount | grep $tipo | cut -d' ' -f1 | grep /dev)   							 # cogemos los archivos de dispositivos
			contador=$(df -aht $tipo --total | tr -s ' ' ' ' | grep /dev/ | cut -d' ' -f3) # proporciona la ocupacion de cada tipo de sistema
			tipo_archivo=$(df -at $tipo | sort -k2 -n | cut -d' ' -f1 | tail -n 1) 				 # cogemos el archivo de dispositivo
			for user in $1;  						# for de cada usuario dentro de la cadena de usuarios.
			do
				if id $user &> /dev/null; # si existe la id de usuario, entonces
				then
					almacen=$(lsof -u $user $tipo_archivo | wc -l) # almacenamos en "almacen" el total de numero de archivos abiertos (con cabecera).
					cuenteo=${almacen} - 1  # puesto que nos coge todas las líneas y tenemos cabecera cuando ejecutamos 'lsof', restamos uno para quitarsela.
					if [[ "$a2_ejercicio" ]]; then																		# si se cumple la condición de que existe un archivo de dispositivo, entonces se hace el nºmayor,menor.
						mayor=$(stat -c %t ${a2_ejercicio})
						mayor=$(echo "obase=10; ibase=16; $mayor;" | bc)
						menor=$(stat -c %T ${a2_ejercicio})
						menor=$(echo "obase=10; ibase=16; $menor;" | bc)
					else																															# si no se cumple la condición, asterisco en nºmayor,menor.
						mayor=\'*\'
						menor=\'*\'
					fi
					muestra+="${tipo} ${tipo_archivo} ${contador} ${cuenteo} ${mayor} ${menor}\n"
					cuenteo=0 # le damos valor 0 para que reitere desde este valor.
				fi
			done
		fi
	done
	echo -e $muestra | column -t | sort -k4 -nr
}

sopen_user()
{
	muestra=  																		# creamos una variable "muestra" donde almacenaremos todas las operaciones.
	cuenteo=0 																		# creamos una variable "cuenteo" donde almacenaremos el total de los archivos abiertos -1 (sin cabecera).
	almacen=0																		  # creamos una variable "almacen" donde almacenaremos el total de los archivos abiertos (con cabecera).
	system=$(mount | cut -d' ' -f5 | sort | uniq) # cogemos todos los tipos de archivos que hay para poder iterar con ellos.
	echo "${TEXT_YELLOW}TABLA DE ARCHIVOS DE DISPOSITIVO CON USUARIOS ESPECÍFICOS${TEXT_RESET}"
	muestra+="${TEXT_SALMON}Files Used Mounted_on Space_occupies Number_total Major Minor${TEXT_RESET}\n"
	echo $1
	for tipo in system; # for de cada tipo dentro de "system" que almacena todos los tipos de archivos.
	do
		if df -at $tipo | cut -d' ' -f1 | grep /dev/ &> /dev/null; # si existe un archivo de dispositivo del tipo, entonces
		then
			a2_ejercicio=$(mount | grep $tipo | cut -d' ' -f1 | grep /dev)   							 # cogemos los archivos de dispositivos
			contador=$(df -aht $tipo --total | tr -s ' ' ' ' | grep /dev/ | cut -d' ' -f3) # proporciona la ocupacion de cada tipo de sistema
			tipo_archivo=$(df -at $tipo | sort -k2 -n | cut -d' ' -f1 | tail -n 1) 				 # cogemos el archivo de dispositivo
			for user in $1;  						# for de cada usuario dentro de la cadena de usuarios.
			do
				if id $user &> /dev/null; # si existe la id de usuario, entonces
				then
					almacen=$(lsof -u $user $tipo_archivo | wc -l) # almacenamos en "almacen" el total de numero de archivos abiertos (con cabecera).
					cuenteo=${almacen} - 1  # puesto que nos coge todas las líneas y tenemos cabecera cuando ejecutamos 'lsof', restamos uno para quitarsela.
					if [[ "$a2_ejercicio" ]]; then																		# si se cumple la condición de que existe un archivo de dispositivo, entonces se hace el nºmayor,menor.
						mayor=$(stat -c %t ${a2_ejercicio})
						mayor=$(echo "obase=10; ibase=16; $mayor;" | bc)
						menor=$(stat -c %T ${a2_ejercicio})
						menor=$(echo "obase=10; ibase=16; $menor;" | bc)
					else																															# si no se cumple la condición, asterisco en nºmayor,menor.
						mayor=\'*\'
						menor=\'*\'
					fi
					muestra+="${tipo} ${tipo_archivo} ${contador} ${cuenteo} ${mayor} ${menor}\n"
					cuenteo=0 # le damos valor 0 para que reitere desde este valor.
				fi
			done
		fi
	done
	echo -e $muestra | column -t | sort -k4 -nr
}

sdevice_table()
{
	muestra=  																		# creamos una variable "muestra" donde almacenaremos todas las operaciones.
	echo "${TEXT_YELLOW}TIPOS DE SISTEMAS DE ARCHIVOS MONTADOS EN EL SISTEMA$TEXT_RESET"
	muestra+="${TEXT_GREEN}Type Filesystem  Used  Mounted_on Number_of_type Major Minor${TEXT_RESET}\n"
	system=$(mount | cut -d' ' -f5 | sort | uniq) # cogemos todos los tipos de archivos que hay para poder iterar con ellos
	for tipo in $system;  												# for de cada tipo dentro de "system" que almacena todos los tipos de archivos.
	  do
	  a_ejercicio=$(df -aht $tipo | tr -s ' ' ' ' | cut -d' ' -f1,3,6 | sort -k2 -n | tail -n 1) # obtenemos los campos 1(Tipo archivo), 3(Usado) y 6(Montado) de cada tipo de sistema
	  c_ejercicio=$(mount | cut -d' ' -f5 | sort | grep $tipo | wc -w) 	# proporciona con wc cuantas veces existe cada tipo de sistema
		a2_ejercicio=$(mount | grep $tipo | cut -d' ' -f1 | grep /dev) 		# cogemos los archivos de dispositivos
	  if [[ "$a2_ejercicio" ]]; then																		# si se cumple la condición de que existe un archivo de dispositivo, entonces se hace el nºmayor,menor.
		mayor=$(stat -c %t ${a2_ejercicio})
		mayor=$(echo "obase=10; ibase=16; $mayor;" | bc)
		menor=$(stat -c %T ${a2_ejercicio})
		menor=$(echo "obase=10; ibase=16; $menor;" | bc)
	  else																															# si no se cumple la condición, asterisco en nºmayor,menor.
		mayor=\'*\'
		menor=\'*\'
		fi
	  muestra+="${tipo} ${a_ejercicio} ${c_ejercicio} ${mayor} ${menor}\n"
	  done
	echo -e $muestra | column -t | sort -k5 -nr	
}

sdevice_table_inv()
{
	muestra=  																		# creamos una variable "muestra" donde almacenaremos todas las operaciones.
	echo "${TEXT_YELLOW}TIPOS DE SISTEMAS DE ARCHIVOS MONTADOS EN EL SISTEMA$TEXT_RESET"
	muestra+="${TEXT_GREEN}Type Filesystem  Used  Mounted_on Number_of_type Major Minor${TEXT_RESET}\n"
	system=$(mount | cut -d' ' -f5 | sort | uniq) # cogemos todos los tipos de archivos que hay para poder iterar con ellos
	for tipo in $system;  												# for de cada tipo dentro de "system" que almacena todos los tipos de archivos.
	  do
	  a_ejercicio=$(df -aht $tipo | tr -s ' ' ' ' | cut -d' ' -f1,3,6 | sort -k2 -n | tail -n 1) # obtenemos los campos 1(Tipo archivo), 3(Usado) y 6(Montado) de cada tipo de sistema
	  c_ejercicio=$(mount | cut -d' ' -f5 | sort | grep $tipo | wc -w) 	# proporciona con wc cuantas veces existe cada tipo de sistema
		a2_ejercicio=$(mount | grep $tipo | cut -d' ' -f1 | grep /dev) 		# cogemos los archivos de dispositivos
	  if [[ "$a2_ejercicio" ]]; then																		# si se cumple la condición de que existe un archivo de dispositivo, entonces se hace el nºmayor,menor.
		mayor=$(stat -c %t ${a2_ejercicio})
		mayor=$(echo "obase=10; ibase=16; $mayor;" | bc)
		menor=$(stat -c %T ${a2_ejercicio})
		menor=$(echo "obase=10; ibase=16; $menor;" | bc)
	  else																															# si no se cumple la condición, asterisco en nºmayor,menor.
		mayor=\'*\'
		menor=\'*\'
		fi
	  muestra+="${tipo} ${a_ejercicio} ${c_ejercicio} ${mayor} ${menor}\n"
	  done
	echo -e $muestra | column -t | sort -k5 -n
}

sdevice_table_inv_user()
{
	muestra=  																		# creamos una variable "muestra" donde almacenaremos todas las operaciones.
	cuenteo=0 																		# creamos una variable "cuenteo" donde almacenaremos el total de los archivos abiertos -1 (sin cabecera).
	almacen=0																		  # creamos una variable "almacen" donde almacenaremos el total de los archivos abiertos (con cabecera).
	system=$(mount | cut -d' ' -f5 | sort | uniq) # cogemos todos los tipos de archivos que hay para poder iterar con ellos.
	echo "${TEXT_YELLOW}TABLA DE ARCHIVOS DE DISPOSITIVO CON USUARIOS ESPECÍFICOS${TEXT_RESET}"
	muestra+="${TEXT_SALMON}Files Used Mounted_on Space_occupies Number_total Major Minor${TEXT_RESET}\n"
	echo $1
	for tipo in system; # for de cada tipo dentro de "system" que almacena todos los tipos de archivos.
	do
		if df -at $tipo | cut -d' ' -f1 | grep /dev/ &> /dev/null; # si existe un archivo de dispositivo del tipo, entonces
		then
			c_ejercicio=$(mount | cut -d' ' -f5 | sort | grep $tipo | wc -w) 							 # proporciona con wc cuantas veces existe cada tipo de sistema
			a2_ejercicio=$(mount | grep $tipo | cut -d' ' -f1 | grep /dev)   							 # cogemos los archivos de dispositivos
			contador=$(df -aht $tipo --total | tr -s ' ' ' ' | grep /dev/ | cut -d' ' -f3) # proporciona la ocupacion de cada tipo de sistema
			tipo_archivo=$(df -at $tipo | sort -k2 -n | cut -d' ' -f1 | tail -n 1) 				 # cogemos el archivo de dispositivo
			for user in $1;  						# for de cada usuario dentro de la cadena de usuarios.
			do
				if id $user &> /dev/null; # si existe la id de usuario, entonces
				then
					almacen=$(lsof -u $user $tipo_archivo | wc -l) # almacenamos en "almacen" el total de numero de archivos abiertos (con cabecera).
					cuenteo=${almacen} - 1  # puesto que nos coge todas las líneas y tenemos cabecera cuando ejecutamos 'lsof', restamos uno para quitarsela.
  				if [[ "$a2_ejercicio" ]]; then																		# si se cumple la condición de que existe un archivo de dispositivo, entonces se hace el nºmayor,menor.
						mayor=$(stat -c %t ${a2_ejercicio})
						mayor=$(echo "obase=10; ibase=16; $mayor;" | bc)
						menor=$(stat -c %T ${a2_ejercicio})
						menor=$(echo "obase=10; ibase=16; $menor;" | bc)
  				else																															# si no se cumple la condición, asterisco en nºmayor,menor.
						mayor=\'*\'
						menor=\'*\'
					fi
					muestra+="${a2_ejercicio} ${contador} ${cuenteo} ${c_ejercicio} ${mayor} ${menor}\n"
					cuenteo=0 # le damos valor 0 para que reitere desde este valor.
				fi
			done
		fi
	done
	echo -e $muestra | column -t | sort -k6 -n
}

# FUNCION PRINCIPAL
if [ "$1" == "" ]; then # si el argumento es nulo, no_arg=1
	no_arg=1
else
	while [ "$1" != "" ]; do # mientras que el argumento no sea nulo, iteramos respecto todas las opciones que tenemos.
		case $1 in
			-h | --help)
				ayuda=1
				shift
				;;

			-inv | --invert)
				invertir=1
				shift
				;;

			-df | -devicefiles)
				dev=1
				shift
				;;

			-u | --user)
				usu=1
				shift
				;;

			-nh | -noheader)
				no_header=1
				shift
				;;

			-sopen)
				s_open=1
				shift
				;;
			
			-sdevice)
				s_device=1
				shift
				;;

			*)
				if [[ $usu ]]; then
					usuario+=$1" "
				else
					no_valido=1
				fi
				shift
				;;
		esac
	done
fi

if [[ "$s_open" ]] && [[ "$dev" ]] && [[ "$usu" ]]; then
	sopen_device_user $usuario
elif [[ "$s_device" ]] && [[ "$invertir" ]] && [[ "$usu" ]]; then
	sdevice_table_inv_user $usuario
elif [[ "$no_header" ]] && [[ "$invertir" ]]; then
	table_noheader_invert
elif [[ "$no_header" ]] && [[ "$dev" ]]; then
	table_noheader_device
# elif [[ "$no_header" ]] && [[ "$usu" ]]; then
# 	table_noheader_user $usuario
elif [[ $invertir ]] && [[ "$dev" ]]; then
	table_device_invert
elif [[ "$s_open" ]] && [[ "$dev" ]]; then
	sopen_device
elif [[ "$s_open" ]] && [[ "$usu" ]]; then
	sopen_user $usuario
elif [[ "$s_device" ]] && [[ "$invertir" ]]; then
	sdevice_table_inv
elif [[ "$no_arg" ]]; then
	tabla
elif [[ "$ayuda" ]] ; then
	usage
elif [[ "$invertir" ]]; then
	inv
elif [[ "$dev" ]]; then
	device_files
elif [[ "$usu" ]]; then
	user_table $usuario
elif [[ "$no_header" ]]; then
	table_no_header
elif [[ "$s_device" ]]; then
	sdevice_table
else
	error_exit
fi