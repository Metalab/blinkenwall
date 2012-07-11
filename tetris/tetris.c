// Printf tetris

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define WALL_WIDTH 9
#define WALL_HEIGHT 5
#define WALL_SIZE WALL_WIDTH * WALL_HEIGHT

#define KEY_UP    'w'
#define KEY_DOWN  's'
#define KEY_LEFT  'a'
#define KEY_RIGHT 'd'

#define STONE_SIZE 5

#define	GRAVITY_X 1
#define	GRAVITY_Y 0
#define STONE_X_START -6
#define STONE_Y_START 0


#define DEBUG 1

void paint_field(uint8_t playfield[WALL_WIDTH][WALL_HEIGHT][3], uint8_t stone[STONE_SIZE][STONE_SIZE][3], int stonex, int stoney) {
    char field[WALL_WIDTH][WALL_HEIGHT];
    int x, y;

    memset(field, '.', WALL_SIZE);

    for (y=0; y<WALL_HEIGHT; ++y) {
        for (x=0; x<WALL_WIDTH; ++x) {
            if(playfield[x][y][0] != 0 || playfield[x][y][1] != 0 || playfield[x][y][2] != 0){
		field[x][y]='#';
	    }
        }
    }

    
    for (y=0; y<STONE_SIZE; ++y) {
        for (x=0; x<STONE_SIZE; ++x) {
    	    if(stone[x][y][0] > 0 || stone[x][y][1] > 0 || stone[x][y][2] > 0){
                if (x+stonex >= 0 && x+stonex < WALL_WIDTH && y+stoney >= 0 && y+stoney < WALL_HEIGHT)
                    field[x+stonex][y+stoney]='*';
	    }
	}
    }
	    
	    
    for (y=0; y<WALL_HEIGHT; ++y) {
        for (x=0; x<WALL_WIDTH; ++x) {
            fprintf(stderr, "%c", field[x][y]);
        }
        fprintf(stderr, "\n\r");
    }
    fprintf(stderr, "\n\r");
}



void send_field(uint8_t playfield[WALL_WIDTH][WALL_HEIGHT][3], uint8_t stone[STONE_SIZE][STONE_SIZE][3], int stonex, int stoney) {
    uint8_t output[WALL_WIDTH][WALL_HEIGHT][3];
    int i, x, y;

    memset(output, 0, WALL_SIZE*3);
    
    
    for (y=0; y<WALL_HEIGHT; ++y) {
        for (x=0; x<WALL_WIDTH; ++x) {
            for (i=0; i<3; ++i) {
                output[x][y][i]=playfield[x][y][i];
            }
        }
    }

    
    for (y=0; y<STONE_SIZE; ++y) {
        for (x=0; x<STONE_SIZE; ++x) {
            if(stone[x][y][0] > 0 || stone[x][y][1] > 0 || stone[x][y][2] > 0){
                if (x+stonex >= 0 && x+stonex < WALL_WIDTH && y+stoney >= 0 && y+stoney < WALL_HEIGHT){
                    for (i=0; i<3; ++i) {
                        output[x+stonex][y+stoney][i]=stone[x][y][i]; 
                    }
                }
            }
	}
    }
	    
	    
    for (y=0; y<WALL_HEIGHT; ++y) {
        for (x=0; x<WALL_WIDTH; ++x) {
            putchar(output[x][y][0]);
            putchar(output[x][y][1]);
            putchar(output[x][y][2]);
	}
    }
    fflush(stdout);
}

void move_playfield_down(uint8_t playfield[WALL_WIDTH][WALL_HEIGHT][3],
                         uint8_t stone[STONE_SIZE][STONE_SIZE][3], int stonex, int stoney){
    uint8_t playfield_tmp [WALL_WIDTH][WALL_HEIGHT][3];
    int i, x, y;
    //FIX IT GEORG
    for (i=0; i<3; ++i) {
        playfield [8][0][i]=0;
        playfield [8][1][i]=0;
        playfield [8][2][i]=0;
        playfield [8][3][i]=0;
        playfield [8][4][i]=0;
    }
    paint_field(playfield, stone, stonex, stoney);
    send_field(playfield, stone, stonex, stoney);
    sleep(1);
    //playfield um eins nach gravity
    memcpy(playfield_tmp,playfield,sizeof(playfield));
    for (y=0; y<WALL_HEIGHT; ++y) {
        for (x=0; x<WALL_WIDTH; ++x) {
            playfield[x][y][0]=playfield_tmp[x+(GRAVITY_X*-1)][y+(GRAVITY_Y*-1)][0];
            playfield[x][y][1]=playfield_tmp[x+(GRAVITY_X*-1)][y+(GRAVITY_Y*-1)][1];
            playfield[x][y][2]=playfield_tmp[x+(GRAVITY_X*-1)][y+(GRAVITY_Y*-1)][2];
        }
    }
    //FIX IT GEORG
    for (i=0; i<3; ++i) {
        playfield [0][0][i]=0;
        playfield [0][1][i]=0;
        playfield [0][2][i]=0;
        playfield [0][3][i]=0;
        playfield [0][4][i]=0;
    }
    paint_field(playfield, stone, stonex, stoney);
    send_field(playfield, stone, stonex, stoney);
    sleep(1);
}

int check_for_ready_line(uint8_t playfield[WALL_WIDTH][WALL_HEIGHT][3]){

    int i, line;
  
    if (GRAVITY_X == 1)  line=WALL_WIDTH-1;
    if (GRAVITY_X == -1) line=0;
    if (GRAVITY_Y == 1)  line=WALL_HEIGHT-1;
    if (GRAVITY_Y == -1) line=0;
  
    if (GRAVITY_X != 0){
        for (i=0; i<WALL_HEIGHT; ++i) {
            //if (DEBUG) fprintf(stderr, " x:%d   y: %d\n\r", line,i);
            if(playfield[line][i][0] == 0 && playfield[line][i][1] == 0 && playfield[line][i][2] == 0)
                return 0;
        }
    }

  
    if (GRAVITY_Y != 0){
        for (i=0; i<WALL_WIDTH; ++i) {
            //if (DEBUG) fprintf(stderr, "1 x:%d   y: %d\n\r", i,line);
            if(playfield[i][line][0] == 0 && playfield[i][line][1] == 0 && playfield[i][line][2] == 0)
                return 0;      
        }        
    }
  
    return 1;
  
}



int check_for_touch(uint8_t playfield[WALL_WIDTH][WALL_HEIGHT][3], uint8_t stone[STONE_SIZE][STONE_SIZE][3], int stonex, int stoney) {

    int x, y;
    for (y=0; y<STONE_SIZE; ++y) {
        for (x=0; x<STONE_SIZE; ++x) {
    	    if(stone[x][y][0] > 0 || stone[x][y][1] > 0 || stone[x][y][2] > 0){
                if (x+stonex >= WALL_WIDTH && GRAVITY_X == 1){
                    if (DEBUG) fprintf(stderr, "WALL ");
                    return 1;
                }
                if (x+stonex <= 0 && GRAVITY_X == -1){
                    if (DEBUG) fprintf(stderr, "WALL ");
                    return 2;
                }
                if (y+stoney >= WALL_HEIGHT && GRAVITY_Y == 1){
                    if (DEBUG) fprintf(stderr, "WALL ");
                    return 3;
                }
                if (y+stoney <= 0 && GRAVITY_Y == -1){
                    if (DEBUG) fprintf(stderr, "WALL ");
                    return 4;
                }
	    }
	}
    }
    
    
    for (y=0; y<STONE_SIZE; ++y) {
        for (x=0; x<STONE_SIZE; ++x) {
	    if(stone[x][y][0] != 0 || stone[x][y][1] != 0 || stone[x][y][2] != 0){
		if(stonex+x >= 0 && stonex+x < WALL_WIDTH && stoney+y >= 0 && stoney+y < WALL_HEIGHT){
                    if (DEBUG) fprintf(stderr, "X:%d Y:%d\n\r", stonex+x, stoney+y);
                    if(playfield[stonex+x][stoney+y][0] != 0 || playfield[stonex+x][stoney+y][1] != 0 || playfield[stonex+x][stoney+y][2] != 0){
                        if (DEBUG) fprintf(stderr, "BRICK ON X:%d Y:%d ", stonex, stoney);
                        return 5;
                    }
		}	
	    }
        }
    }
    
    return 0;
  
}  

int main(int argc, char * argv[]) {
    uint8_t playfield [WALL_WIDTH][WALL_HEIGHT][3];
    uint8_t stone [STONE_SIZE][STONE_SIZE][3];
    int stonex;
    int stoney;
    
    stonex=STONE_X_START;
    stoney=STONE_Y_START;
    
    memset(playfield, 0, WALL_SIZE*3);
    memset(stone, 0, STONE_SIZE*STONE_SIZE*3);
    
    
    playfield [8][0][2]=255;
    playfield [8][1][2]=255;
    playfield [8][2][2]=255;
    playfield [8][4][1]=255;
    
    playfield [7][0][0]=255;
    playfield [7][1][0]=255;
    playfield [7][4][1]=255;  
    
    
    stone [2][2][0]=255;
    stone [2][3][0]=255;
    stone [1][3][0]=255;
    stone [3][3][0]=255;
    
    
    int i, x, y;
    
    int err;
    while(1) {
	if (DEBUG) fprintf(stderr, "WHILE BEGIN\n\r");
	
	stonex=stonex+GRAVITY_X;
	stoney=stoney+GRAVITY_Y;

	
	if (err = check_for_touch(playfield, stone, stonex, stoney)){
	    
	    if (DEBUG) fprintf(stderr, "TOUCHED WITH RETURN: %d\n\r", err);

	    for (y=0; y<STONE_SIZE; ++y) {
		for (x=0; x<STONE_SIZE; ++x) {
		    if(stone[x][y][0] > 0 || stone[x][y][1] > 0 || stone[x][y][2] > 0){
			playfield[x+stonex-GRAVITY_X][y+stoney-GRAVITY_Y][0]=stone[x][y][0];
			playfield[x+stonex-GRAVITY_X][y+stoney-GRAVITY_Y][1]=stone[x][y][1];
			playfield[x+stonex-GRAVITY_X][y+stoney-GRAVITY_Y][2]=stone[x][y][2];			
		    }
		}
	    }
	    stonex=STONE_X_START;
	    stoney=STONE_Y_START;
	}
	
	while (check_for_ready_line(playfield)){
            if (DEBUG) fprintf(stderr, "LINE READYD\n\r");
            move_playfield_down(playfield, stone, stonex, stoney);
	}
    
    exit:
        system ("/bin/stty echo cooked");

        return 0;

    }
}

