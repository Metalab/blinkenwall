// Printf & Websocket tetris

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <sys/time.h>
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

int
timeval_subtract (result, x, y)
    struct timeval *result, *x, *y;
{
    /* Perform the carry for the later subtraction by updating y. */
    if (x->tv_usec < y->tv_usec) {
        int nsec = (y->tv_usec - x->tv_usec) / 1000000 + 1;
        y->tv_usec -= 1000000 * nsec;
        y->tv_sec += nsec;
    }
    if (x->tv_usec - y->tv_usec > 1000000) {
        int nsec = (x->tv_usec - y->tv_usec) / 1000000;
        y->tv_usec += 1000000 * nsec;
        y->tv_sec -= nsec;
    }

    /* Compute the time remaining to wait.
       tv_usec is certainly positive. */
    result->tv_sec = x->tv_sec - y->tv_sec;
    result->tv_usec = x->tv_usec - y->tv_usec;

    /* Return 1 if result is negative. */
    return x->tv_sec < y->tv_sec;
}

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

    fwrite(output[0][0], 1, WALL_WIDTH*WALL_HEIGHT*3, stdout);
    fflush(stdout);
}

void send_nextstone(nextstone)
{
    char sndbuf[WALL_WIDTH*WALL_HEIGHT*3];
    memset(sndbuf, 0, WALL_WIDTH*WALL_HEIGHT*3);
    memcpy(sndbuf, &bw_sendback_prefix, 8);
    sprintf(sndbuf+8, "tetris stone %d", nextstone);

    fwrite(sndbuf, 1, WALL_WIDTH*WALL_HEIGHT*3, stdout);
    fflush(stdout);    
}

void send_points(int points)
{
    char sndbuf[WALL_WIDTH*WALL_HEIGHT*3];
    memset(sndbuf, 0, WALL_WIDTH*WALL_HEIGHT*3);
    memcpy(sndbuf, &bw_sendback_prefix, 8);
    sprintf(sndbuf+8, "tetris points %d", points);

    fwrite(sndbuf, 1, WALL_WIDTH*WALL_HEIGHT*3, stdout);
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
    usleep(500000);

    //playfield um eins nach gravity
    memcpy(playfield_tmp, playfield, WALL_SIZE * 3);
    for (y=line; y>0; --y) {
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
    usleep(500000);
}

int check_for_ready_line(uint8_t playfield[WALL_WIDTH][WALL_HEIGHT][3]){

    int x, y, line;
 
    for (y=WALL_HEIGHT-1; y>=0; --y) {
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

int allow_movement(uint8_t playfield[WALL_WIDTH][WALL_HEIGHT][3],
                   struct stone * st) {
    int x, y;
    for (y=0; y<MAX_STONE_SIZE; ++y) {
        for (x=0; x<MAX_STONE_SIZE; ++x) {
            if(st->data[x][y][0] > 0 ||
               st->data[x][y][1] > 0 ||
               st->data[x][y][2] > 0) {
                if (x+st->x < 0 || x+st->x >= WALL_WIDTH)
                    return 0;
                if (st->x+x >= 0 && st->x+x < WALL_WIDTH &&
                    st->y+y >= 0 && st->y+y < WALL_HEIGHT) {
                    if(playfield[st->x+x][st->y+y][0] != 0 ||
                       playfield[st->x+x][st->y+y][1] != 0 ||
                       playfield[st->x+x][st->y+y][2] != 0)
                        return 0;
                }
            }
        }
    }
    return 1;
}

char read_command(delay)
{
    fd_set readfds;
    struct timeval tv;
    int retval = 0;
    char cmd_buf[1024];
        
    FD_ZERO(&readfds);
    FD_SET(0, &readfds);
    tv.tv_sec = 0;
    tv.tv_usec = delay;
        
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
    uint8_t playfield [WALL_WIDTH][WALL_HEIGHT][3];

    struct stone st;
    struct stone st_tmp;

    int x, y;
    int line_to_remove;
    int delay = 300000;

    int stone_next;

    int points = 0;

    struct timeval tv;
    struct timeval tv2;
    struct timeval tv_res;

    //BwlSocketContext * sc;

    system ("/bin/stty -echo raw");

/*
    sc = bw_socket_open();
    if (!sc) {
        fprintf(stderr, "Error opening socket\n");
        return 1;
    }
*/

/*
    if (bw_wait_for_connections(sc) != 0) {
        fprintf(stderr, "Error when waiting for connection\n");
        return 1;
    }
*/

    srand(time(NULL));

    memset(playfield, 0, WALL_SIZE * 3);
    memset(st.data, 0, MAX_STONE_SIZE * MAX_STONE_SIZE*3);
    memset(st_tmp.data, 0, MAX_STONE_SIZE * MAX_STONE_SIZE*3);

    st = stones[rand() % NUM_STONES];
    stone_next = rand() % NUM_STONES;
    send_nextstone(stone_next);

    send_points(0);

    gettimeofday(&tv, NULL);

    while(1) {
        int touched;
        int key;

        key = read_command(delay);
        //key = bw_get_cmd_block_timeout(sc, NULL, delay);
        
        if (DEBUG) fprintf(stderr, "WHILE BEGIN\n\r");

        touched = 0;

        if (key == KEY_ROT_LEFT) {
            st_tmp = rotate_stone(st);
        } else if (key == KEY_ROT_RIGHT) {
            int i;
            for (i=0; i<3; ++i)
                st_tmp = rotate_stone(st);
        } else {
            st_tmp = st;
        }
        if (allow_movement(playfield, &st_tmp))
            st = st_tmp;

        st_tmp = st;
        if (key == KEY_LEFT) {
            st_tmp.x++;
        } else if (key == KEY_RIGHT) {
            st_tmp.x--;
        } else if (key == KEY_DOWN) {
            delay /= 10;
        } else if (key == KEY_QUIT) {
            goto exit;
        } else {
            st_tmp = st;
        }
        if (allow_movement(playfield, &st_tmp)) {
            st = st_tmp;
        }

/*
        switch(key) {
        case BW_CMD_BUTTON2_PRESSED:
            rotated_stone = rotate_stone(st);
            break;
        case BW_CMD_BUTTON1_PRESSED:
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
*/

        gettimeofday(&tv2, NULL);

        timeval_subtract(&tv_res, &tv2, &tv);

        if (tv_res.tv_usec > delay) {
            int lines_removed;

            st_tmp = st;
            st_tmp.y++;
            
            if (!check_for_touch(playfield, &st_tmp))
                st = st_tmp;
            else
                touched = 1;

            if (touched) {
                if (DEBUG) fprintf(stderr, "TOUCHED WITH RETURN\n\r");

                for (y=MAX_STONE_SIZE-1; y>=0; --y) {
                    for (x=0; x<MAX_STONE_SIZE; ++x) {
                        if(st.data[x][y][0] > 0 || st.data[x][y][1] > 0 || st.data[x][y][2] > 0) {
                            if (y+st.y < 0){
                                fprintf(stderr, "Aus is\n\r");
                                goto exit;
                            }
                        }
                    }
                }

                for (y=MAX_STONE_SIZE-1; y>=0; --y) {
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
                st = stones[stone_next];
                stone_next = rand() % NUM_STONES;
                send_nextstone(stone_next);
                delay = 300000;
            }
	
            lines_removed = 0;
            while ((line_to_remove = check_for_ready_line(playfield)) != -1){
                if (DEBUG) fprintf(stderr, "LINE READY %d\n\r", line_to_remove);
                move_playfield_down(playfield, &st, line_to_remove);
                lines_removed++;
                points += (lines_removed * 100);
            }
            if (lines_removed > 0) {
                send_points(points);
            }

            gettimeofday(&tv, NULL);
        }

        paint_field(playfield, &st);
        send_field(playfield, &st);
    }

exit:
    printf("\n\r");
    system ("/bin/stty echo cooked");
    //bw_socket_close(sc);

    return 0;
}
