// Printf & Websocket tetris

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include "../libwbl/libwbl.h"

#define WALL_WIDTH 5
#define WALL_HEIGHT 9
#define WALL_SIZE WALL_WIDTH * WALL_HEIGHT

#define KEY_ROT_RIGHT 'e'
#define KEY_ROT_LEFT  'q'
#define KEY_LEFT      'a'
#define KEY_RIGHT     'd'
#define KEY_DOWN      's'
#define KEY_QUIT      0x3 // Ctrl+c

#define MAX_STONE_SIZE 5

#define NUM_STONES 7

#define SLEEP_TIME 100

#define DEBUG 0

typedef struct stone {
    uint8_t data[MAX_STONE_SIZE][MAX_STONE_SIZE][3];
    int x;
    int y;
} stone_t;

struct stone stones[NUM_STONES] =  { {
        { { {0,   0,   0}, {0,   0,   0}, {0,   0,   0}, {0,   0,   0}, {0,   0,   0} },
          { {0,   0,   0}, {0,   0,   0}, {0,   0,   0}, {0,   0,   0}, {0,   0,   0} },
          { {0, 255, 255}, {0, 255, 255}, {0, 255, 255}, {0, 255, 255}, {0,   0,   0} },
          { {0,   0,   0}, {0,   0,   0}, {0,   0,   0}, {0,   0,   0}, {0,   0,   0} },
          { {0,   0,   0}, {0,   0,   0}, {0,   0,   0}, {0,   0,   0}, {0,   0,   0} } },
        0, // Position X
        -5 // Position Y
    }, {
        { { {0,   0,   0}, {0,   0,   0}, {0,   0,   0}, {0,   0,   0}, {0,   0,   0} },
          { {0,   0,   0}, {0,   0, 255}, {0,   0,   0}, {0,   0,   0}, {0,   0,   0} },
          { {0,   0,   0}, {0,   0, 255}, {0,   0, 255}, {0,   0, 255}, {0,   0,   0} },
          { {0,   0,   0}, {0,   0,   0}, {0,   0,   0}, {0,   0,   0}, {0,   0,   0} },
          { {0,   0,   0}, {0,   0,   0}, {0,   0,   0}, {0,   0,   0}, {0,   0,   0} } }, 
        0, // Position X
        -2 // Position Y
    }, {
        { { {0,   0,   0}, {0,   0,   0}, {0,   0,   0}, {0,   0,   0}, {0,   0,   0} },
          { {0,   0,   0}, {0,   0,   0}, {0,   0,   0}, {255, 127, 0}, {0,   0,   0} },
          { {0,   0,   0}, {255, 127, 0}, {255, 127, 0}, {255, 127, 0}, {0,   0,   0} },
          { {0,   0,   0}, {0,   0,   0}, {0,   0,   0}, {0,   0,   0}, {0,   0,   0} },
          { {0,   0,   0}, {0,   0,   0}, {0,   0,   0}, {0,   0,   0}, {0,   0,   0} } }, 
        0, // Position X
        -2 // Position Y
    }, {
        { { {0,   0,   0}, {0,   0,   0}, {0,   0,   0}, {0,   0,   0}, {0,   0,   0} },
          { {0,   0,   0}, {255, 255, 0}, {255, 255, 0}, {0,   0,   0}, {0,   0,   0} },
          { {0,   0,   0}, {255, 255, 0}, {255, 255, 0}, {0,   0,   0}, {0,   0,   0} },
          { {0,   0,   0}, {0,   0,   0}, {0,   0,   0}, {0,   0,   0}, {0,   0,   0} },
          { {0,   0,   0}, {0,   0,   0}, {0,   0,   0}, {0,   0,   0}, {0,   0,   0} } }, 
        0, // Position X
        -2 // Position Y
    }, {
        { { {0,   0,   0}, {0,   0,   0}, {0,   0,   0}, {0,   0,   0}, {0,   0,   0} },
          { {0,   0,   0}, {0,   0,   0}, {0,   255, 0}, {0,   255, 0}, {0,   0,   0} },
          { {0,   0,   0}, {0,   255, 0}, {0,   255, 0}, {0,   0,   0}, {0,   0,   0} },
          { {0,   0,   0}, {0,   0,   0}, {0,   0,   0}, {0,   0,   0}, {0,   0,   0} },
          { {0,   0,   0}, {0,   0,   0}, {0,   0,   0}, {0,   0,   0}, {0,   0,   0} } }, 
        0, // Position X
        -2 // Position Y
    }, {
        { { {0,   0,   0}, {0,   0,   0}, {0,   0,   0}, {0,   0,   0}, {0,   0,   0} },
          { {0,   0,   0}, {0,   0,   0}, {128, 0, 128}, {0,   0,   0}, {0,   0,   0} },
          { {0,   0,   0}, {128, 0, 128}, {128, 0, 128}, {128, 0, 128}, {0,   0,   0} },
          { {0,   0,   0}, {0,   0,   0}, {0,   0,   0}, {0,   0,   0}, {0,   0,   0} },
          { {0,   0,   0}, {0,   0,   0}, {0,   0,   0}, {0,   0,   0}, {0,   0,   0} } }, 
        0, // Position X
        -2 // Position Y
    }, {
        { { {0,   0,   0}, {0,   0,   0}, {0,   0,   0}, {0,   0,   0}, {0,   0,   0} },
          { {0,   0,   0}, {255, 0,   0}, {255, 0,   0}, {0,   0,   0}, {0,   0,   0} },
          { {0,   0,   0}, {0,   0,   0}, {255, 0,   0}, {255, 0,   0}, {0,   0,   0} },
          { {0,   0,   0}, {0,   0,   0}, {0,   0,   0}, {0,   0,   0}, {0,   0,   0} },
          { {0,   0,   0}, {0,   0,   0}, {0,   0,   0}, {0,   0,   0}, {0,   0,   0} } }, 
        0, // Position X
        -2 // Position Y
    } };

struct stone rotate_stone(struct stone st) {
    struct stone new_st;
    int x, y;

    memset(new_st.data, 0, MAX_STONE_SIZE * MAX_STONE_SIZE * 3);

    for (y=0; y<MAX_STONE_SIZE; ++y) {
        for (x=0; x<MAX_STONE_SIZE; ++x) {
            new_st.data[y][MAX_STONE_SIZE-x-1][0] = st.data[x][y][0];
            new_st.data[y][MAX_STONE_SIZE-x-1][1] = st.data[x][y][1];
            new_st.data[y][MAX_STONE_SIZE-x-1][2] = st.data[x][y][2];
        }
    }
    new_st.x = st.x;
    new_st.y = st.y;

    return new_st;
}

void paint_field(uint8_t playfield[WALL_WIDTH][WALL_HEIGHT][3],
                 struct stone * st) {
    char field[WALL_WIDTH][WALL_HEIGHT];
    int x, y;

    memset(field, '.', WALL_SIZE);

    for (y=0; y<WALL_HEIGHT; ++y) {
        for (x=0; x<WALL_WIDTH; ++x) {
            if(playfield[x][y][0] != 0 ||
               playfield[x][y][1] != 0 ||
               playfield[x][y][2] != 0){
                field[x][y]='#';
            }
        }
    }

    for (y=0; y<MAX_STONE_SIZE; ++y) {
        for (x=0; x<MAX_STONE_SIZE; ++x) {
    	    if(st->data[x][y][0] > 0 ||
               st->data[x][y][1] > 0 ||
               st->data[x][y][2] > 0){
                if (x+st->x >= 0 && x+st->x < WALL_WIDTH &&
                    y+st->y >= 0 && y+st->y < WALL_HEIGHT)
                    field[x+st->x][y+st->y]='*';
            }
        }
    }
    	    
    for (y=0; y<WALL_HEIGHT; ++y) {
        for (x=0; x<WALL_WIDTH; ++x) {
            fprintf(stderr, "%c", field[WALL_WIDTH-x-1][y]);
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
    
    for (y=0; y<MAX_STONE_SIZE; ++y) {
        for (x=0; x<MAX_STONE_SIZE; ++x) {
            if(st->data[x][y][0] > 0 || st->data[x][y][1] > 0 || st->data[x][y][2] > 0){
                if (x+st->x >= 0 && x+st->x < WALL_WIDTH && y+st->y >= 0 && y+st->y < WALL_HEIGHT){
                    for (i=0; i<3; ++i) {
                        output[x+st->x][y+st->y][i]=st->data[x][y][i]; 
                    }
                }
            }
        }
    }

    uint8_t * rotated_tmp = (uint8_t*)&output[0][0];
    for (i=0; i<WALL_WIDTH*WALL_HEIGHT*3; ++i) {
        putchar(*rotated_tmp++);
    }
    fflush(stdout);
}

void move_playfield_down(uint8_t playfield[WALL_WIDTH][WALL_HEIGHT][3],
                         struct stone * st, int line){
    uint8_t playfield_tmp [WALL_WIDTH][WALL_HEIGHT][3];
    int i, x, y, length;

    length = WALL_WIDTH;

    for (i=0; i<length; ++i) {
        playfield[i][line][0]=0;
        playfield[i][line][1]=0;
        playfield[i][line][2]=0;
    }
    paint_field(playfield, st);
    send_field(playfield, st);
    usleep(SLEEP_TIME * 1000);

    //playfield um eins nach gravity
    memcpy(playfield_tmp, playfield, WALL_SIZE * 3);
    for (y=1; y<WALL_HEIGHT; ++y) {
        for (x=0; x<WALL_WIDTH; ++x) {
            playfield[x][y][0]=playfield_tmp[x][y-1][0];
            playfield[x][y][1]=playfield_tmp[x][y-1][1];
            playfield[x][y][2]=playfield_tmp[x][y-1][2];
        }
    }

    for (i=0; i<length; ++i) {
        playfield [i][0][0]=0;
        playfield [i][0][1]=0;
        playfield [i][0][2]=0;
    }

    paint_field(playfield, st);
    send_field(playfield, st);
    usleep(SLEEP_TIME * 1000);
}

int check_for_ready_line(uint8_t playfield[WALL_WIDTH][WALL_HEIGHT][3]){

    int x, y, line;
 
    for (y=0; y<WALL_HEIGHT; ++y) {
        line=0;
        for (x=0; x<WALL_WIDTH; ++x) {
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
    for (y=0; y<MAX_STONE_SIZE; ++y) {
        for (x=0; x<MAX_STONE_SIZE; ++x) {
    	    if(st->data[x][y][0] > 0 || st->data[x][y][1] > 0 || st->data[x][y][2] > 0) {
                if (y+st->y >= WALL_HEIGHT){
                    if (DEBUG) fprintf(stderr, "WALL ");
                    return 1;
                }
            }
        }
    }
    
    for (y=0; y<MAX_STONE_SIZE; ++y) {
        for (x=0; x<MAX_STONE_SIZE; ++x) {
            if(st->data[x][y][0] != 0 || st->data[x][y][1] != 0 || st->data[x][y][2] != 0) {
                if(st->x+x >= 0 && st->x+x < WALL_WIDTH &&
                   st->y+y >= 0 && st->y+y < WALL_HEIGHT){
                    if(playfield[st->x+x][st->y+y][0] != 0 ||
                       playfield[st->x+x][st->y+y][1] != 0 ||
                       playfield[st->x+x][st->y+y][2] != 0) {
                        if (DEBUG) fprintf(stderr, "BRICK ON X:%d Y:%d ", st->x, st->y);
                        return 5;
                    }
                }	
            }
        }
    }
    
    return 0;
  
}

int allow_rotate(struct stone * st) {
    int x, y;
    for (y=0; y<MAX_STONE_SIZE; ++y) {
        for (x=0; x<MAX_STONE_SIZE; ++x) {
            if(st->data[x][y][0] > 0 ||
               st->data[x][y][1] > 0 ||
               st->data[x][y][2] > 0) {
                if (x+st->x < 0 || x+st->x >= WALL_WIDTH)
                    return 0;
            }
        }
    }
    return 1;
}

int read_key(int delay) {
    fd_set readfds;
    struct timeval tv;
    int retval;

    FD_ZERO(&readfds);
    FD_SET(0, &readfds);
    tv.tv_sec = 0;
    tv.tv_usec = delay;

    retval = select(1, &readfds, NULL, NULL, &tv);

    if (retval > 0)
        return getchar();
    else
        return 0;
}

int main(int argc, char * argv[]) {
    uint8_t playfield [WALL_WIDTH][WALL_HEIGHT][3];

    struct stone st;
    struct stone st_tmp;

    int i, x, y;
    int line_to_remove;
    int frame = 0;
    int delay = SLEEP_TIME;

    BwlSocketContext * sc;

    //system ("/bin/stty -echo raw");

    sc = bw_socket_open();
    if (!sc) {
        fprintf(stderr, "Error opening socket\n");
        return 1;
    }

    if (bw_wait_for_connections(sc) != 0) {
        fprintf(stderr, "Error when waiting for connection\n");
        return 1;
    }

    srand(time(NULL));
 
    memset(playfield, 0, WALL_SIZE * 3);
    memset(st.data, 0, MAX_STONE_SIZE * MAX_STONE_SIZE*3);
    memset(st_tmp.data, 0, MAX_STONE_SIZE * MAX_STONE_SIZE*3);

    st = stones[rand() % NUM_STONES];

    while(1) {
        struct stone moved_stone;
        struct stone rotated_stone;
        int touched;
        int key;
        int paint = 0;

        //key = read_key(delay);
        key = bw_get_cmd_block_timeout(sc, NULL, delay);
        
        if (DEBUG) fprintf(stderr, "WHILE BEGIN\n\r");

        touched = 0;

/*
        if (key == KEY_ROT_LEFT) {
            rotated_stone = rotate_stone(st);
        } else if (key == KEY_ROT_RIGHT) {
            int i;
            for (i=0; i<3; ++i)
                rotated_stone = rotate_stone(st);
        } else if (key == KEY_LEFT) {
            rotated_stone.x++;
        } else if (key == KEY_RIGHT) {
            rotated_stone.x--;
        } else if (key == KEY_DOWN) {
            delay /= 10;
        } else if (key == KEY_QUIT) {
            goto exit;
        } else {
            rotated_stone = st;
        }
*/

        switch(key) {
        case BW_CMD_BUTTON1_PRESSED:
            rotated_stone = rotate_stone(st);
            break;
        case BW_CMD_BUTTON2_PRESSED:
            for (i=0; i<3; ++i)
                rotated_stone = rotate_stone(st);
            break;
        case BW_CMD_LEFT_PRESSED:
            rotated_stone.x++;
            break;
        case BW_CMD_RIGHT_PRESSED:
            rotated_stone.x--;
            break;
        case BW_CMD_DOWN_PRESSED:
            delay /= 10;
            break;
        case  BW_CMD_DISCONNECT:
            goto exit;
        default:
            rotated_stone = st;
        }
        
        if (allow_rotate(&rotated_stone)) {
            paint = 1;
            if (!check_for_touch(playfield, &rotated_stone))
                st = rotated_stone;
            else
                touched = 1;
        }
        
        if ((frame++ % (1000 / SLEEP_TIME)) == 0) {
            
            moved_stone = st;
            moved_stone.y++;
            
            if (!check_for_touch(playfield, &moved_stone))
                st = moved_stone;
            else
                touched = 1;

            if (touched) {
                if (DEBUG) fprintf(stderr, "TOUCHED WITH RETURN\n\r");

                for (y=0; y<MAX_STONE_SIZE; ++y) {
                    for (x=0; x<MAX_STONE_SIZE; ++x) {
                        if(st.data[x][y][0] > 0 || st.data[x][y][1] > 0 || st.data[x][y][2] > 0) {
                            if (y+st.y < 0){
                                fprintf(stderr, "Aus is\n\r");
                                goto exit;
                            }
                        }
                    }
                }

                for (y=0; y<MAX_STONE_SIZE; ++y) {
                    for (x=0; x<MAX_STONE_SIZE; ++x) {
                        if(st.data[x][y][0] > 0 ||
                           st.data[x][y][1] > 0 ||
                           st.data[x][y][2] > 0) {
                            playfield[x+st.x][y+st.y][0]=st.data[x][y][0];
                            playfield[x+st.x][y+st.y][1]=st.data[x][y][1];
                            playfield[x+st.x][y+st.y][2]=st.data[x][y][2];
                        }
                    }
                }
                st = stones[rand() % NUM_STONES];
                delay = SLEEP_TIME;
            }
	
            while ((line_to_remove = check_for_ready_line(playfield)) != -1){
                if (DEBUG) fprintf(stderr, "LINE READY %d\n\r", line_to_remove);
                move_playfield_down(playfield, &st, line_to_remove);
            }
            paint = 1;
        }

        if (paint) {
            paint_field(playfield, &st);
            send_field(playfield, &st);
        }
    }
    
exit:
    printf("\n\r");
    //system ("/bin/stty echo cooked");
    bw_socket_close(sc);

    return 0;
}
