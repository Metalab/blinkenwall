#include <dirent.h>
#include <string.h>
#include <stdio.h>
#include <stdint.h>
#include <time.h>
#include <unistd.h>
#include <stdlib.h>
#include "./font.c"


#define SCREEN_WIDTH  24
#define SCREEN_HEIGHT 24
#define COLOUR 0
#define X_SLEEPTIME_BETWEEN 100


struct s_folder
{
	char path[1000];
	int file_count;
        char* name[100];
        char type[100];
};


struct s_MoveData
{
	//CharPosition ist die derzeitige position beim abarbeiten des aktuellen Buchstabens
	uint8_t CharPosition;
	//AktuellerChar beinhaltet den aktuellen Buchstaben
	uint8_t AktuellerChar;
	//Mit dieser Variable wird auf den aktuellen buchstaben gezeigt der gerade angezeigt wird
	uint8_t TextPos;
};


void msleep (unsigned int ms) {
    int microsecs;
    struct timeval tv;
    microsecs = ms * 1000;
    tv.tv_sec  = microsecs / 1000000;
    tv.tv_usec = microsecs % 1000000;
    select (0, NULL, NULL, NULL, &tv);
}


void DrawScreenbufferToStdout(uint8_t screenbuffer[SCREEN_HEIGHT][SCREEN_WIDTH][3]) {
        fwrite(screenbuffer, 1, SCREEN_WIDTH * SCREEN_HEIGHT * 3, stdout);

        fflush(stdout);

}


void DrawScreenbufferToStderr(uint8_t screenbuffer[SCREEN_HEIGHT][SCREEN_WIDTH][3]) {

        int x, y;
	fprintf(stderr, "\n\r");
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
        fprintf(stderr, "-----------\n\r\n\r------------\n\r");
        
}


void MoveScreenbufferLeft( uint8_t screenbuffer[SCREEN_HEIGHT][SCREEN_WIDTH][3], uint8_t offset, uint8_t height) {

        int x, y;
        for (y=offset; y<offset+height; ++y) {
                for (x=0; x<SCREEN_WIDTH-1; ++x) {
                        screenbuffer[y][x][0] = screenbuffer[y][x+1][0];
                        screenbuffer[y][x][1] = screenbuffer[y][x+1][1];
                        screenbuffer[y][x][2] = screenbuffer[y][x+1][2];
                }
        }
        
}

void WrtiteCharOut( uint8_t screenbuffer[SCREEN_HEIGHT][SCREEN_WIDTH][3], struct s_MoveData * MoveData, uint8_t offset) {


                MoveScreenbufferLeft(screenbuffer, offset, FONT_HEIGHT);
                
                
                //printf("aktueller buchstabe: %c\n", AktuellerChar);
                
                //schleife von 0 bis 8 durchlaufen lassen und alle 9 bit der Spalte anzuzeigeni
                uint8_t i;
                for (i=0; i<=FONT_HEIGHT-1; i++)
                {
                        if ( font[MoveData[0].AktuellerChar][i] & (1<<MoveData[0].CharPosition) ) {
                                screenbuffer[i+offset][SCREEN_WIDTH-1][COLOUR]=255;
                        }else{
                                screenbuffer[i+offset][SCREEN_WIDTH-1][COLOUR]=0;
                        }
                }
                
                //Nächste spalte selektieren
                MoveData[0].CharPosition--;
                //Wenn die Spalte 0 erreicht wurde wieder auf 8 für den nächsten Buchstaben setzen
                if(MoveData[0].CharPosition == 0){
                        MoveData[0].CharPosition = FONT_WIDTH+1;
                        MoveData[0].TextPos++;
                }

}


void WriteTextInScreen(uint8_t screenbuffer[SCREEN_HEIGHT][SCREEN_WIDTH][3], char *TextString, uint8_t offset){

	struct s_MoveData * MoveData = malloc(sizeof(struct s_MoveData));
	uint8_t i;


        MoveData[0].CharPosition = FONT_WIDTH+1;
        MoveData[0].AktuellerChar = 0;
        MoveData[0].TextPos = 0;

        MoveData[0].CharPosition = FONT_WIDTH+1;
        MoveData[0].TextPos=0;
        MoveData[0].AktuellerChar = TextString[MoveData[0].TextPos];



	for (i=0; i<SCREEN_WIDTH; i++){

                MoveData[0].AktuellerChar = TextString[MoveData[0].TextPos];
		if (MoveData[0].AktuellerChar == 0){
			//damit nicht auf den nächsten buchstaben nach null geschoben werden kann
			MoveData[0].CharPosition = FONT_WIDTH+1;
		}
		WrtiteCharOut(screenbuffer, MoveData, offset);
	}

}


void myreaddir(struct s_folder * folder, int foldernumber){

        struct dirent *dp;
        DIR *dir = opendir(folder[foldernumber].path);

        folder[foldernumber].file_count=0;
	folder[foldernumber].name[folder[foldernumber].file_count]=strdup(""); 

	if (dir){
                while (dp=readdir(dir)) {
                        if (dp->d_name[0] != '.' ){
				folder[foldernumber].name[folder[foldernumber].file_count]=strdup(dp->d_name);
	                        folder[foldernumber].type[folder[foldernumber].file_count]=dp->d_type;
	                        //printf("%s    ___   %d\n", filelist[*filelist_count], filetype[*filelist_count]);
	                        folder[foldernumber].file_count++;
			}
                }
        }else{
                //printf("An error occoured while opening \"%s\"\n", folder[foldernumber].path);
        }


}


char read_command()
{
    fd_set readfds;
    struct timeval tv;
    int retval = 0;
    char cmd_buf[1024];

        
    FD_ZERO(&readfds);
    FD_SET(0, &readfds);
    tv.tv_sec = 0;
    tv.tv_usec = 0;
        
    retval = select(STDIN_FILENO+1, &readfds, NULL, NULL, &tv);
    if (retval > 0) {
        int num_read = read(STDIN_FILENO, cmd_buf, 255);
        if (num_read > 0) {
            return(cmd_buf[0]);
        } else {
            return 0;
        }
    } else {
        return 0;
    }
}



int main(int argc, char * argv[]) {

//	char TextString = argv[1];
	char TextString[100];
	uint8_t load_next_text=0;

	struct s_folder * folder = malloc(sizeof(struct s_folder)*8);
	struct s_MoveData * MoveData = malloc(sizeof(struct s_MoveData));

	uint8_t screenbuffer[SCREEN_HEIGHT][SCREEN_WIDTH][3];

	int input;
	int menu_pos=0;
	int i;
	uint8_t folder_deep=0;

memset(screenbuffer, 0, SCREEN_WIDTH * SCREEN_HEIGHT * 3);

	MoveData[0].CharPosition = FONT_WIDTH+1;
        MoveData[0].AktuellerChar = 0;
	MoveData[0].TextPos = 0;




	//folder[0].path=strdup("/home/blinkentisch/programme");
	strcpy(folder[0].path, "/home/blinkentisch/programme");
	myreaddir(folder, folder_deep);

	WriteTextInScreen(screenbuffer, "Menu", 0);

/*printf("\n\n\n");
	for (i=0; i< filelist_count; i++){

		printf("%s    ___   %d\n", filelist[i], filetype[i]);

	}
*/
//	strcpy(TextString, folder[folder_deep].name[menu_pos]);
//	strcat(TextString, " ");

	while (1){
	
	        input = read_command();

        	switch(input) {
	        case 'w':
			load_next_text=1;
			menu_pos--;
			if (menu_pos < 0) {
				menu_pos=folder[folder_deep].file_count-1;
			}
	            break;
	        case 's':
                        load_next_text=1;
                        menu_pos++;
			if(menu_pos==folder[folder_deep].file_count){
				menu_pos=0;
			}
	            break;
                case 'd':
			if(folder[folder_deep].type[menu_pos]==DT_DIR){
				load_next_text=1;
				WriteTextInScreen(screenbuffer, folder[folder_deep].name[menu_pos], 0);
				folder_deep++;
				strcpy(folder[folder_deep].path, folder[folder_deep-1].path);
				strcat(folder[folder_deep].path, "/");
				strcat(folder[folder_deep].path, folder[folder_deep-1].name[menu_pos]); 	
				myreaddir(folder, folder_deep);
				menu_pos=0;
			}else{
				//printf("No Folder");
			}
                    break;
                case 'a':
			if (folder_deep){
				load_next_text=1;
				folder_deep--;
				if (!folder_deep){
					WriteTextInScreen(screenbuffer, "Menu", 0);
				}else{
					WriteTextInScreen(screenbuffer, folder[folder_deep-1].name[menu_pos], 0);
				}
                        	menu_pos=0;
			}
                    break;
	        default:
	            break;
        	}

		





		MoveData[0].AktuellerChar = TextString[MoveData[0].TextPos];
                if (MoveData[0].AktuellerChar == 0 || load_next_text==1){
                        load_next_text=0;
		        MoveData[0].CharPosition = FONT_WIDTH+1;
		        strcpy(TextString, " ");
		        strcat(TextString, folder[folder_deep].name[menu_pos]);
                        MoveData[0].TextPos=0;
                        MoveData[0].AktuellerChar = TextString[MoveData[0].TextPos];
                }
		WrtiteCharOut(screenbuffer, MoveData, 17);

                //DrawScreenbufferToStderr(screenbuffer);
                DrawScreenbufferToStdout(screenbuffer);





		msleep(X_SLEEPTIME_BETWEEN);

	}
}
