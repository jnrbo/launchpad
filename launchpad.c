/*
 * Authors:
 * Júnior B.
 * Raysonn T.
*/

#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>

#include <pthread.h>

#define HIGH 	1
#define LOW  	0
#define INPUT	1
#define OUTPUT 	0

#define MAX     2

//gcc blabla.c -0 blabla

bool GPIOExport(int pino);
bool GPIODirection(int pino, int direcao);
bool GPIOWrite(int pino, int estado);
bool GPIORead(int pino);
bool GPIOUnexport(int pino);

/** START OF PROGRAM **/
void delay(int millis) {
	usleep(millis * 1000);
}

typedef struct jMusic {
	char name[7];
	int time;
} music;

music playList[16] = {
	{"01", 1231}, {"02", 1231}, {"03", 1231}, {"04", 1231},
	{"05", 1231}, {"06", 1231}, {"07", 1231}, {"08", 1231},
	{"09", 1231}, {"10", 1231}, {"11", 1231}, {"12", 1231},
	{"13", 1231}, {"14", 1231}, {"15", 1231}, {"16", 1231}
};

int buttonLine[4] = {23, 18, 15, 14};
int buttonCol[4] = {11, 9, 10, 22};

int ledLine[4] = {3, 4, 17, 27};
int ledCol[4] = {7, 8, 25, 24};

void *playMusic(void *);
pthread_t music_thread;
int position = -1;
int pI, pJ;
int i, j;
void initializeButtons() {
	for (i = 0; i < 4; i++) {
		GPIOUnexport(buttonLine[i]);
		GPIOUnexport(buttonCol[i]);
		GPIOUnexport(ledLine[i]);
		GPIOUnexport(ledCol[i]);
		if (GPIOExport(buttonLine[i]) && GPIOExport(buttonCol[i]) && GPIOExport(ledLine[i]) && GPIOExport(ledCol[i])) {
			GPIODirection(buttonLine[i], INPUT);
			GPIODirection(buttonCol[i], OUTPUT);

			GPIODirection(ledLine[i], INPUT);
			GPIODirection(ledCol[i], OUTPUT);
		}
	}

	while (1) {
		for (i = 0; i < 4; i++) {
			for (j = 0; j < 4; j++) {
				GPIOWrite(buttonCol[i], HIGH);
				if (GPIORead(buttonLine[j])) {
					position = 1 + j + (4 * i);
					printf("\npressed l%d c%d",j, i );
					if (pthread_create(&music_thread, NULL, playMusic, NULL)) {
						printf("\n\nErro ao tocar musica %d \n", position);
					}
				}
				GPIOWrite(buttonCol[i], HIGH);
			}
		}
	}
}

void *playMusic(void *kkkkk) {
	int time = playList[position].time;
	if (time > 0) {
		//char command[30];
		//snprintf (command, 30, "sudo ./ffplay sounds/%s.mp3", playList[position].name);
		//system(command);

		GPIOWrite(ledLine[pI], HIGH);
		GPIOWrite(ledCol[pJ], LOW);
		delay(time);
		GPIOWrite(ledLine[pI], LOW);
		GPIOWrite(ledCol[pJ], HIGH);
	} else {
		
		for (i = 0; i < 3; i++) {
			GPIOWrite(ledLine[pI], HIGH);
			GPIOWrite(ledCol[pJ], LOW);
			usleep(500000);
			GPIOWrite(ledLine[pI], LOW);
			GPIOWrite(ledCol[pJ], HIGH);
		}
	}
}

void main() {
	initializeButtons();
}


/** END OF PROGRAM **/

bool GPIOExport(int pino) {
	char 	buffer[3];
	int 	arquivo;

	arquivo = open("/sys/class/gpio/export", O_WRONLY);
	
	if( arquivo == -1 )
		return false;

	snprintf( buffer, 3, "%d", pino);
	if( write( arquivo, buffer, 3 ) == -1 ) {
		close(arquivo);
		return false;
	}
	
	close(arquivo);
	return true;
}

bool GPIOUnexport(int pino) {
	char 	buffer[3];
	int 	arquivo;

	arquivo = open("/sys/class/gpio/unexport", O_WRONLY);
	
	if (arquivo == -1) {
		return false;
	}

	snprintf( buffer, 3, "%d", pino);

	if (write( arquivo, buffer, 3 ) == -1) {
		close(arquivo);
		return false;
	}
	
	close(arquivo);
	return true;
}


bool GPIODirection (int pino, int direcao) {
	int 	arquivo;
	char	caminho[35];

	snprintf( caminho, 35, "/sys/class/gpio/gpio%d/direction", pino );
	
	arquivo = open( caminho, O_WRONLY );
	if( arquivo == -1 ) {
		return false;
	}
	 
	if( write( arquivo, ((direcao == INPUT)?"in":"out"), 3) == -1 )	{
		close(arquivo);
		return false;
	}

	close(arquivo);
	return true;
}


bool GPIOWrite (int pino, int estado) {
	int 	arquivo;
	char	caminho[35];

	snprintf (caminho, 35, "/sys/class/gpio/gpio%d/value", pino);
	
	arquivo = open( caminho, O_WRONLY );
	
	if( arquivo == -1 ) {
		return false;
	}
	
	if( write( arquivo, (estado == HIGH)?"1":"0", 1) == -1 ) {
		close(arquivo);
		return false;
	}

	close(arquivo);
	return true;
}

bool GPIORead(int pino) {

    int     arquivo;
    char    caminho[35];
    char  retorno[2];

    snprintf( caminho, 35, "/sys/class/gpio/gpio%d/value", pino );
    
    arquivo = open( caminho, O_RDONLY );
    if ( arquivo == -1 ) {
        return false;
    }
   
    read(arquivo, retorno, 2);
    close(arquivo);
    return atoi(retorno);
}
