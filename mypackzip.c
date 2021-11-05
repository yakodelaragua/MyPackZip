#include "header.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <errno.h>
#include <dirent.h>

#define TAMANO_BUFFER 512

#define MSG_ERROR1 "Numero de argumentos erroneo\n"
#define MSG_ERROR5 "Error: insertar -i, extraer -e.\n"

//Errores insertar
#define E_OPEN "No se puede abrir o crear file_mypackzip.\n"
#define E_OPEN2 "No se puede abrir  dir_fuente.\n"

//Errores extraer
#define E_OPENEX "No se puede abrir file_mypackzip.\n"
#define E_CREAT_DIR "No se puede crear  dir_fuente.\n" 


int insertar_directorio(char* fichero_origen, char* destino);
int extraer_fichero(char* dir_destino, char* file_mypackzip, unsigned long pos);
int extraer_directorio(char* dir_destino, char * file_mypackzip);

int main(int argc, char *argv[])
{

	//llamada a insertar y extraer directorio
	if(argc == 4) {
		if(strcmp(argv[1], "-i") == 0)	
		{
			insertar_directorio(argv[2], argv[3]);
			exit(0);

		} else if(strcmp(argv[1], "-e") == 0) {
			extraer_directorio(argv[2], argv[3]);
			exit(0);
		} else {
			write(2, MSG_ERROR5, strlen(MSG_ERROR5));
			exit(1);
		} 

	} else {			
		write(2, MSG_ERROR1, strlen(MSG_ERROR1));
		exit(1);
	}

}


int insertar_directorio(char* fichero_origen, char* destino) {

    struct s_header Cab;
	int tamOri;
	int n;
	char* nombre;

//	Directorio
	DIR *dir;
    struct dirent *rdir;

//	Fichero destino
	int fd;

	char buf[TAMANO_BUFFER];

	//abrir direccion
	dir = opendir(fichero_origen); //abrir direccion

    if(dir == NULL) {	//error al abrir
        write(2, E_OPEN2, strlen(E_OPEN2));
        exit(2);
    }


	fd = open(destino, O_RDWR | O_CREAT | O_APPEND, 0644);		//abrir fichero destino
	if(fd < 0) {
		write(2, E_OPEN, strlen(E_OPEN));
		exit(1);
	}
	
	//Espacion para ruta
    char *ori = (char *)malloc(sizeof(* fichero_origen));

	while ((rdir = readdir(dir)) != NULL) { //Leer ficheros contenidos en direccion
		nombre = rdir->d_name;

		if((strcmp(nombre, "..") == 0) || (strcmp(nombre, ".") == 0)) { //actual y padre

		} else {
			//Nombre completo ruta relativa
			strcpy(ori, fichero_origen);    //copia de dir_fuente en ori
            char *direccion = strcat(ori, "/");
            char *no = strcat(direccion, nombre);   //nombre completo

			//abrir fichero destino
            int fo = open(no, O_RDONLY);


			//terminar cabecera
			strcpy(Cab.InfoF.FileName, no);
			tamOri = lseek(fo, 0L, SEEK_END);
			Cab.InfoF.TamOri = tamOri;
			Cab.InfoF.TamComp = tamOri;

			lseek(fo, 0L, SEEK_SET);	//apuntar al inicio del fichero original

			//escribir cabecera
			write(fd, &Cab, sizeof(Cab));
			lseek(fd, sizeof(Cab), SEEK_CUR);

			printf("Insertando fichero: \n");
			printf(" -nombre del fichero: %s \n", Cab.InfoF.FileName);
			printf(" -tamano original: %ld \n", Cab.InfoF.TamOri);

			while ((n = read (fo, buf, tamOri)) > 0) {	//leer fichero origen
				write(fd, buf, n);	//escribir fichero destino

   			}
			
			for (n = 0; n < TAMANO_BUFFER; n++) buf[n] = '\0'; //reset buf

			close(fo);

		}

	}

	return 0;

}


int extraer_directorio(char* dir_destino, char * file_mypackzip) {
	unsigned long i = 0;
	while(extraer_fichero(dir_destino, file_mypackzip, i) != 1) {
		i++;
	}
	return 0;
}

int extraer_fichero(char* dir_destino, char* file_mypackzip, unsigned long pos) {
  	struct s_header Cab;
	unsigned long i;

//	Original file
	int fo;
//	Destiny file
	int fd;

	char buf[TAMANO_BUFFER];
		

//	Control de errores:
    fo = open(file_mypackzip, O_RDONLY);	//abrir fichero origen
	if(fo < 0) {
		write(2, E_OPENEX, strlen(E_OPENEX));
		exit(2);
	}
	
	lseek(fo, 0, SEEK_SET);

	//recorrer ficheros hasta llegar a la posicion indicada
	for(i = 0; i < pos; i++) {
		read(fo, &Cab, sizeof(Cab));
		int b = 0;
		b += Cab.InfoF.TamComp;
		lseek(fo, b, SEEK_CUR);

	}
	
	read(fo, &Cab, sizeof(Cab));

	if(strstr(Cab.InfoF.FileName, dir_destino)) { //si pertenece a dir_destino	

		fd = open(Cab.InfoF.FileName, O_CREAT| O_RDWR |O_APPEND, 0644);	//abrir nuevo fichero con el nombre leido anteriormente

		if(fd < -1) {
			write(2, E_CREAT_DIR, strlen(E_CREAT_DIR));
			exit(2);
		}

		lseek(fd, sizeof(Cab), SEEK_SET);	//al final de la cabecera

	//	Copiar archivo
		read(fo, buf, TAMANO_BUFFER);	
		write(fd, buf, Cab.InfoF.TamOri);	
		
		//Comprobacion fin fichero
		int actual = lseek(fo, 0, SEEK_CUR);
		int fin = lseek(fo, 0, SEEK_END);
		if(actual == fin){
			return 1;
		}
	   
		close (fd);

	}
	   
close (fo);
return 0;
	
}