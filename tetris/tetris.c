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

#define MAX_STONE_SIZE 4
#define STONE_SIZE 5

#define STONE_X_START 3
#define STONE_Y_START 0

#define SLEEP_TIME 1000000

#define DEBUG 1

typedef struct stone {
    uint8_t data[MAX_STONE_SIZE][MAX_STONE_SIZE][3];
    int width;
    int height;
    int x;
    int y;
} stone_t;

struct stone stones[2] =  { {
        { { {0, 255, 255}, {0, 255, 255}, {0, 255, 255}, {0, 255, 255} },
          { {0,   0,   0}, {0,   0,   0}, {0,   0,   0}, {0,   0,   0} },
          { {0,   0,   0}, {0,   0,   0}, {0,   0,   0}, {0,   0,   0} },
          { {0,   0,   0}, {0,   0,   0}, {0,   0,   0}, {0,   0,   0} } },
        4, // WIDTH
        1, // HEIGHT
        0, // Position X
        -1 // Position Y
    }, {
        { { {0,   0, 255}, {0,   0,   0}, {0,   0,   0}, {0,   0,   0} },
          { {0,   0, 255}, {0,   0, 255}, {0,   0, 255}, {0,   0,   0} },
          { {0,   0,   0}, {0,   0,   0}, {0,   0,   0}, {0,   0,   0} },
          { {0,   0,   0}, {0,   0,   0}, {0,   0,   0}, {0,   0,   0} } }, 
        3, // WIDTH
        2, // HEIGHT
        1, // Position X
        -2 // Position Y
    }, {
        { { {0,   0,   0}, {0,   0,   0}, {255, 127, 0}, {0,   0,   0} },
          { {255, 127, 0}, {255, 127, 0}, {255, 127, 0}, {0,   0,   0} },
          { {0,   0,   0}, {0,   0,   0}, {0,   0,   0}, {0,   0,   0} },
          { {0,   0,   0}, {0,   0,   0}, {0,   0,   0}, {0,   0,   0} } }, 
        3, // WIDTH
        2, // HEIGHT
        1, // Position X
        -2 // Position Y
    }, {
        { { {255, 255, 0}, {255, 255, 0}, {0,   0,   0}, {0,   0,   0} },
          { {255, 255, 0}, {255, 255, 0}, {0,   0,   0}, {0,   0,   0} },
          { {0,   0,   0}, {0,   0,   0}, {0,   0,   0}, {0,   0,   0} },
          { {0,   0,   0}, {0,   0,   0}, {0,   0,   0}, {0,   0,   0} } }, 
        2, // WIDTH
        2, // HEIGHT
        2, // Position X
        -2 // Position Y
    }, {
        { { {0,   0,   0}, {0,   255, 0}, {0,   255, 0}, {0,   0,   0} },
          { {0,   255, 0}, {0,   255, 0}, {0,   0,   0}, {0,   0,   0} },
          { {0,   0,   0}, {0,   0,   0}, {0,   0,   0}, {0,   0,   0} },
          { {0,   0,   0}, {0,   0,   0}, {0,   0,   0}, {0,   0,   0} } }, 
        3, // WIDTH
        2, // HEIGHT
        1, // Position X
        -2 // Position Y
    }, {
        { { {0,   0,   0}, {128, 0, 128}, {0,   0,   0}, {0,   0,   0} },
          { {128, 0, 128}, {128, 0, 128}, {128, 0, 128}, {0,   0,   0} },
          { {0,   0,   0}, {0,   0,   0}, {0,   0,   0}, {0,   0,   0} },
          { {0,   0,   0}, {0,   0,   0}, {0,   0,   0}, {0,   0,   0} } }, 
        3, // WIDTH
        2, // HEIGHT
        1, // Position X
        -2 // Position Y
    }, {
        { { {255, 0,   0}, {255, 0,   0}, {0,   0,   0}, {0,   0,   0} },
          { {0,   0,   0}, {255, 0,   0}, {255, 0,   0}, {0,   0,   0} },
          { {0,   0,   0}, {0,   0,   0}, {0,   0,   0}, {0,   0,   0} },
          { {0,   0,   0}, {0,   0,   0}, {0,   0,   0}, {0,   0,   0} } }, 
        3, // WIDTH
        2, // HEIGHT
        1, // Position X
        -2 // Position Y
    } };

void rotate90clock(uint8_t * infield, uint8_t * outfield,
              int w_in,  int h_in) {
    int x, y;
    for (x=0; x<h_in; ++x) {
        for (y=0; y<w_in; ++y) {
            outfield[x*3+y*w_in*3] = infield[(w_in-y-1)*3+x*w_in*3];
            outfield[x*3+y*w_in*3+1] = infield[(w_in-y-1)*3+x*w_in*3+1];
            outfield[x*3+y*w_in*3+2] = infield[(w_in-y-1)*3+x*w_in*3+2];
        }
    }
}

void rotate90cclock(uint8_t * infield, uint8_t * outfield,
              int w_in,  int h_in) {
    int x, y;
    for (x=0; x<h_in; ++x) {
        for (y=0; y<w_in; ++y) {
            outfield[y*3+x*w_in*3] = infield[x*3+(w_in-y-1)*w_in*3];
            outfield[y*3+x*w_in*3+1] = infield[x*3+(w_in-y-1)*w_in*3+1];
            outfield[y*3+x*w_in*3+2] = infield[x*3+(w_in-y-1)*w_in*3+2];
        }
    }
}

void paint_field(uint8_t playfield[WALL_WIDTH][WALL_HEIGHT][3],
                 struct stone * st) {
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

    for (y=0; y<st->height; ++y) {
        for (x=0; x<st->width; ++x) {
    	    if(st->data[x][y][0] > 0 || st->data[x][y][1] > 0 || st->data[x][y][2] > 0){
                if (x+st->x >= 0 && x+st->x < WALL_WIDTH && y+st->y >= 0 && y+st->y < WALL_HEIGHT)
                    field[x+st->x][y+st->y]='*';
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

void send_field(uint8_t playfield[WALL_WIDTH][WALL_HEIGHT][3],
                struct stone * st){
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
            if(st->data[x][y][0] > 0 || st->data[x][y][1] > 0 || st->data[x][y][2] > 0){
                if (x+st->x >= 0 && x+st->x < WALL_WIDTH && y+st->y >= 0 && y+st->y < WALL_HEIGHT){
                    for (i=0; i<3; ++i) {
                        output[x+st->x][y+st->y][i]=st->data[x][y][i]; 
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
                         struct stone * st, int line){
    uint8_t playfield_tmp [WALL_WIDTH][WALL_HEIGHT][3];
    int i, x, y, length;
    //FIX IT GEORG

    length = WALL_WIDTH;

    for (i=0; i<length; ++i) {
        playfield[i][line][0]=0;
        playfield[i][line][1]=0;
        playfield[i][line][2]=0;
    }
    paint_field(playfield, st);
    send_field(playfield, st);
    usleep(SLEEP_TIME);

    //playfield um eins nach gravity
    memcpy(playfield_tmp, playfield, WALL_SIZE * 3);
    for (y=0; y<WALL_HEIGHT; ++y) {
        for (x=0; x<WALL_WIDTH; ++x) {
            playfield[x][y][0]=playfield_tmp[x][y-1][0];
            playfield[x][y][1]=playfield_tmp[x][y-1][1];
            playfield[x][y][2]=playfield_tmp[x][y-1][2];
        }
    }
    //FIX IT GEORG
/*
    for (i=0; i<3; ++i) {
        playfield [0][0][i]=0;
        playfield [0][1][i]=0;
        playfield [0][2][i]=0;
        playfield [0][3][i]=0;
        playfield [0][4][i]=0;
    }
*/
    paint_field(playfield, st);
    send_field(playfield, st);
    usleep(SLEEP_TIME);
}

int check_for_ready_line(uint8_t playfield[WALL_WIDTH][WALL_HEIGHT][3]){

    int x, y, line;
/*
    if (GRAVITY_X != 0) {
        width = WALL_WIDTH;
        height = WALL_HEIGHT;
        x1 = &x;
        y1 = &y;
    } else {
        width = WALL_HEIGHT;
        height = WALL_WIDTH;
        x1 = &y;
        y1 = &x;
    }
*/
 
    for (y=0; y<WALL_HEIGHT; ++y) {
        line=0;
        for (x=0; x<WALL_WIDTH; ++x) {
            //if (DEBUG) fprintf(stderr, " x:%d   y: %d\n\r", line,i);
            if(playfield[x][y][0] != 0 ||
               playfield[x][y][1] != 0 ||
               playfield[x][y][2] != 0)
                line++;
        }
        if (line == WALL_WIDTH)
            return y;
    }

    return -1;
}



int check_for_touch(uint8_t playfield[WALL_WIDTH][WALL_HEIGHT][3],
                    struct stone * st) {

    int x, y;
    for (y=0; y<st->height; ++y) {
        for (x=0; x<st->width; ++x) {
    	    if(st->data[x][y][0] > 0 || st->data[x][y][1] > 0 || st->data[x][y][2] > 0) {
                if (y+st->y >= WALL_HEIGHT){
                    if (DEBUG) fprintf(stderr, "WALL ");
                    return 1;
                }
	    }
	}
    }
    
    for (y=0; y<st->height; ++y) {
        for (x=0; x<st->width; ++x) {
	    if(st->data[x][y][0] != 0 || st->data[x][y][1] != 0 || st->data[x][y][2] != 0){
		if(st->x+x >= 0 && st->x+x < WALL_WIDTH && st->y+y >= 0 && st->y+y < WALL_HEIGHT){
                    if (DEBUG) fprintf(stderr, "X:%d Y:%d\n\r", st->x+x, st->y+y);
                    if(playfield[st->x+x][st->y+y][0] != 0 ||
                       playfield[st->x+x][st->y+y][1] != 0 ||
                       playfield[st->x+x][st->y+y][2] != 0){
                        if (DEBUG) fprintf(stderr, "BRICK ON X:%d Y:%d ", st->x, st->y);
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

    struct stone st;
    struct stone st_tmp;
    int stonex;
    int stoney;

    int i, x, y;
    int line_to_remove;
    int err;
    
    stonex=STONE_X_START;
    stoney=STONE_Y_START;
    
    memset(playfield, 0, WALL_SIZE*3);
    memset(st.data, 0, MAX_STONE_SIZE*STONE_SIZE*3);
    memset(st_tmp.data, 0, MAX_STONE_SIZE*STONE_SIZE*3);
    
    /*
    playfield [0][4][2]=255;
    playfield [4][4][2]=255;
    playfield [5][4][2]=255;
    playfield [6][4][1]=255;
    playfield [7][4][0]=255;
    playfield [8][4][0]=255;
    playfield [7][4][1]=255;*/

  
    playfield [0][0][2]=255;
    playfield [0][1][2]=255;
    playfield [0][2][2]=255;
    playfield [0][4][1]=255;
    
    playfield [1][0][0]=255;
    playfield [1][1][0]=255;
    playfield [1][4][1]=255;  

    while(1) {
        
        rotate90clock((uint8_t*)&st.data[0][0], (uint8_t*)&st_tmp.data[0][0],
                      st.width, st.height);
        st_tmp.height = st.width;
        st_tmp.width = st.height;
        memcpy(&st.data[0][0], st_tmp.data[0][0], st.width * st.height * 3);
        st.width = st_tmp.width;
        st.height = st_tmp.height;

	if (DEBUG) fprintf(stderr, "WHILE BEGIN\n\r");
        
//	stoney=stoney+1;
	
	if (err = check_for_touch(playfield, &st)){
	    
	    if (DEBUG) fprintf(stderr, "TOUCHED WITH RETURN: %d\n\r", err);

	    for (y=0; y<STONE_SIZE; ++y) {
		for (x=0; x<STONE_SIZE; ++x) {
		    if(st.data[x][y][0] > 0 ||
                       st.data[x][y][1] > 0 ||
                       st.data[x][y][2] > 0){
			playfield[x+st.x][y+st.y-1][0]=st.data[x][y][0];
			playfield[x+st.x][y+st.y-1][1]=st.data[x][y][1];
			playfield[x+st.x][y+st.y-1][2]=st.data[x][y][2];
		    }
		}
	    }
	    st.x=STONE_X_START;
	    st.y=STONE_Y_START;
	}
	
	while ((line_to_remove = check_for_ready_line(playfield)) != -1){
            if (DEBUG) fprintf(stderr, "LINE READY %d\n\r", line_to_remove);
            move_playfield_down(playfield, &st, line_to_remove);
	}

        paint_field(playfield, &st);
        send_field(playfield, &st);
        usleep(SLEEP_TIME);
    }
    
exit:
    system ("/bin/stty echo cooked");

    return 0;
}
