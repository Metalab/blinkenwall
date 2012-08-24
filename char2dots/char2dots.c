
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <sys/time.h>
#include "./font.c"


#define WALL_WIDTH  18
#define WALL_HEIGHT 9
#define WALL_SIZE WALL_WIDTH * WALL_HEIGHT
#define COLOUR 0
#define MAX_INTENSITY 5
#define X_SLEEPTIME_BETWEEN 100

//CharPosition ist die derzeitige position beim abarbeiten des aktuellen Buchstabens
uint8_t CharPosition;
//AktuellerChar beinhaltet den aktuellen Buchstaben
uint8_t AktuellerChar;
//CharMoveTimer wenn dieser in Timer interrupt voll ist wird eine Spalte weitergeschoben (CharMoveSub = 1)
uint8_t CharMoveTimer;
//CharMoveSub wird im Timerinterrupt gesetzt. Standart = 0 wenn 1 dann wird in der main eine Spalte weitergeschoben
volatile uint8_t CharMoveSub;
//BlinkTimer ist die Blinkfrequenz der derzeit angewählten Spalten
uint8_t BlinkTimer;
//Mit dieser Variable wird auf den aktuellen buchstaben gezeigt der gerade angezeigt wird
uint8_t TextPos;
//Mit dieser variable werden die Spalten des Buchstabens markiert um zu blinken 0=Normal 1=Blinken
uint8_t CharBlink;


uint8_t screenbuffer[WALL_WIDTH][WALL_HEIGHT][3];


void msleep (unsigned int ms) {
    int microsecs;
    struct timeval tv;
    microsecs = ms * 1000;
    tv.tv_sec  = microsecs / 1000000;
    tv.tv_usec = microsecs % 1000000;
    select (0, NULL, NULL, NULL, &tv);  
}


void DrawScreenbufferToStdout(void) {
	
	int x, y;
	
	
	for (y=0; y<WALL_HEIGHT; ++y) {
		for (x=WALL_WIDTH-6; x>-1; --x) {
		//for (x=0; x<WALL_WIDTH; ++x) {
			putchar(screenbuffer[x][y][0]);
			putchar(screenbuffer[x][y][1]);
			putchar(screenbuffer[x][y][2]);
		}
		
			putchar(screenbuffer[17][y+1][0]);
			putchar(screenbuffer[17][y+1][1]);
			putchar(screenbuffer[17][y+1][2]);
			
			putchar(screenbuffer[16][y+1][0]);
			putchar(screenbuffer[16][y+1][1]);
			putchar(screenbuffer[16][y+1][2]);
			
			putchar(screenbuffer[15][y+1][0]);
			putchar(screenbuffer[15][y+1][1]);
			putchar(screenbuffer[15][y+1][2]);
			
			putchar(screenbuffer[14][y+1][0]);
			putchar(screenbuffer[14][y+1][1]);
			putchar(screenbuffer[14][y+1][2]);
			
			putchar(screenbuffer[13][y+1][0]);
			putchar(screenbuffer[13][y+1][1]);
			putchar(screenbuffer[13][y+1][2]);
			

				

			
	}
	
	/*for (y=0; y<WALL_HEIGHT; ++y) {
		for (x=WALL_WIDTH-1; x>-1; --x) {
		//for (x=0; x<WALL_WIDTH; ++x) {
			fwrite(screenbuffer[x][y], 1, 3, stdout);
		}
	}*/
	//fwrite(screenbuffer, 1, WALL_WIDTH * WALL_HEIGHT * 3, stdout);
	fflush(stdout);
}


void DrawScreenbufferToStderr(void) {

	int x, y;
	for (y=0; y<WALL_HEIGHT; ++y) {
		for (x=0; x<WALL_WIDTH; ++x) {
			if ( screenbuffer[x][y][0] > 0 || screenbuffer[x][y][1] > 0 || screenbuffer[x][y][2] > 0 ){
				fprintf(stderr, "#");
			}else{
				fprintf(stderr, " ");
			}
		}
		fprintf(stderr, "\n\r");
	}
	fprintf(stderr, "\n\r\n\r\n\r");
	
}

void MoveScreenbufferLeft(void) {

	int x, y;
	for (y=0; y<WALL_HEIGHT; ++y) {
		for (x=0; x<WALL_WIDTH-1; ++x) {
			screenbuffer[x][y][0] = screenbuffer[x+1][y][0];
			screenbuffer[x][y][1] = screenbuffer[x+1][y][1];
			screenbuffer[x][y][2] = screenbuffer[x+1][y][2];
		}
	}
	
}

void WrtiteCharOut(void) {

		MoveScreenbufferLeft();
		
		
		//printf("aktueller buchstabe: %c\n", AktuellerChar);
		
		//schleife von 0 bis 8 durchlaufen lassen und alle 9 bit der Spalte anzuzeigen
		for (uint8_t i=0; i!=9; i++)
		{
			if ( font[AktuellerChar][i] & (1<<CharPosition) ) {
				screenbuffer[WALL_WIDTH-1][i][COLOUR]=MAX_INTENSITY;
			}else{
				screenbuffer[WALL_WIDTH-1][i][COLOUR]=0;
			}
		}
		
		//Nächste spalte selektieren
		CharPosition--;
		//Wenn die Spalte 0 erreicht wurde wieder auf 8 für den nächsten Buchstaben setzen
		if(CharPosition == 0){
			CharPosition = 8;
			TextPos++;
		}

}

int main(int argc, char * argv[]) {
	
	char* TextString = argv[1];
	
	
	printf("text: %s\n", TextString);
	
	
	CharPosition = 8;
	CharMoveTimer = 0;
	CharMoveSub = 0;
	CharBlink = 0;
	TextPos=0;
	
	
	//Main Loop
	while(1)
	{
		//Anzuzeigenden buchstaben in AktuellerChar schieben
		AktuellerChar = TextString[TextPos];
		//Wenn Satz zuende dann ist das letzte char 0. deswegen Textpos=0
		if (AktuellerChar == 0){
			TextPos=0;
			AktuellerChar = TextString[TextPos];
		}
		
		
		//500ms warten und zeichen schreiben
		msleep(X_SLEEPTIME_BETWEEN);
		WrtiteCharOut();
		
		DrawScreenbufferToStderr();
		DrawScreenbufferToStdout();
	}
		
	return (1);
}
