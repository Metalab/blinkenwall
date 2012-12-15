#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <sys/time.h>
#include "./font.c"




void msleep (unsigned int ms) {
    int microsecs;
    struct timeval tv;
    microsecs = ms * 1000;
    tv.tv_sec  = microsecs / 1000000;
    tv.tv_usec = microsecs % 1000000;
    select (0, NULL, NULL, NULL, &tv);  
}

/*
void DrawScreenbufferToStdout(void) {
	fwrite(screenbuffer, 1, SCREEN_WIDTH * SCREEN_HEIGHT * 3, stdout);

	fflush(stdout);

}


void DrawScreenbufferToStderr(void) {

	int x, y;
	for (y=0; y<SCREEN_HEIGHT; ++y) {
		for (x=0; x<SCREEN_WIDTH; ++x) {
			if ( screenbuffer[y][x][0] > 0 || screenbuffer[y][x][1] > 0 || screenbuffer[y][x][2] > 0 ){
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
	for (y=0; y<SCREEN_HEIGHT; ++y) {
		for (x=0; x<SCREEN_WIDTH-1; ++x) {
			screenbuffer[y][x][0] = screenbuffer[y][x+1][0];
			screenbuffer[y][x][1] = screenbuffer[y][x+1][1];
			screenbuffer[y][x][2] = screenbuffer[y][x+1][2];
		}
	}
	
}
*/

//uint8_t *screenbuffer[SCREEN_HEIGHT][SCREEN_WIDTH][3], 
void WrtiteCharOut( struct s_MoveData * MoveData) {


/*printf("%d\n", MoveData[0].CharPosition);
MoveData[0].CharPosition--;
printf("%d\n", MoveData[0].CharPosition);
*/
/*
		MoveScreenbufferLeft();
		
		
		//printf("aktueller buchstabe: %c\n", AktuellerChar);
		
		//schleife von 0 bis 8 durchlaufen lassen und alle 9 bit der Spalte anzuzeigeni
		uint8_t i;
		for (i=0; i<=FONT_HEIGHT-1; i++)
		{
			if ( font[AktuellerChar][i] & (1<<CharPosition) ) {
				screenbuffer[i][SCREEN_WIDTH-1][COLOUR]=255;
			}else{
				screenbuffer[i][SCREENWIDTH-1][COLOUR]=0;
			}
		}
		
		//Nächste spalte selektieren
		CharPosition--;
		//Wenn die Spalte 0 erreicht wurde wieder auf 8 für den nächsten Buchstaben setzen
		if(CharPosition == 0){
			CharPosition = FONT_WIDTH+1;
			TextPos++;
		}
*/
}


//void WriteCompleteLineOut()

/*
int main(int argc, char * argv[]) {
	
	char* TextString = argv[1];
	




	
	fprintf(stderr, "text: %s\n", TextString);
	
	
	CharPosition = FONT_WIDTH+1;
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
*/
