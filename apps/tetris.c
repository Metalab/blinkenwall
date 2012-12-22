// Printf & Websocket tetris

//TODO: Play from left to right

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <sys/time.h>
#include "common.h"

#define MAX_STONE_SIZE 5

#define NUM_STONES 7

#define DEBUG 0

typedef struct stone {
    uint8_t data[MAX_STONE_SIZE][MAX_STONE_SIZE][3];
    int x;
    int y;
} stone_t;

struct tetris_config {
    int delay;
};

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

void paint_field(uint8_t playfield[WIDTH][HEIGHT][3],
                 struct stone * st) {
    char field[WIDTH][HEIGHT];
    int x, y;

    for (y=0; y<HEIGHT; ++y) {
        for (x=0; x<WIDTH; ++x) {
            if(playfield[x][y][0] != 0 ||
               playfield[x][y][1] != 0 ||
               playfield[x][y][2] != 0){
                field[x][y]='#';
            } else {
                field[x][y]='.';
            }
        }
    }

    for (y=0; y<MAX_STONE_SIZE; ++y) {
        for (x=0; x<MAX_STONE_SIZE; ++x) {
    	    if(st->data[x][y][0] > 0 ||
               st->data[x][y][1] > 0 ||
               st->data[x][y][2] > 0){
                if (x+st->x >= 0 && x+st->x < WIDTH &&
                    y+st->y >= 0 && y+st->y < HEIGHT)
                    field[x+st->x][y+st->y]='*';
            }
        }
    }
    	    
    for (y=0; y<HEIGHT; ++y) {
        for (x=0; x<WIDTH; ++x) {
            fprintf(stderr, "%c", field[WIDTH-x-1][y]);
        }
        fprintf(stderr, "\n\r");
    }
    fprintf(stderr, "\n\r");
}

void send_field(uint8_t playfield[WIDTH][HEIGHT][3],
                struct stone * st){
    uint8_t output[WIDTH][HEIGHT][3];

    int i, x, y;
    
    for (y=0; y<HEIGHT; ++y) {
        for (x=0; x<WIDTH; ++x) {
            for (i=0; i<3; ++i) {
                output[x][y][i]=playfield[x][y][i];
            }
        }
    }
    
    for (y=0; y<MAX_STONE_SIZE; ++y) {
        for (x=0; x<MAX_STONE_SIZE; ++x) {
            if(st->data[x][y][0] > 0 || st->data[x][y][1] > 0 || st->data[x][y][2] > 0){
                if (x+st->x >= 0 && x+st->x < WIDTH && y+st->y >= 0 && y+st->y < HEIGHT){
                    for (i=0; i<3; ++i) {
                        output[x+st->x][y+st->y][i]=st->data[x][y][i]; 
                    }
                }
            }
        }
    }

    write(STDOUT_FILENO, output[0][0], DISP_BUF_SIZE);
}

void move_playfield_down(uint8_t playfield[WIDTH][HEIGHT][3],
                         struct stone * st, int line){
    uint8_t playfield_tmp [WIDTH][HEIGHT][3];
    int i, x, y, length;

    length = WIDTH;

    for (i=0; i<length; ++i) {
        playfield[i][line][0]=0;
        playfield[i][line][1]=0;
        playfield[i][line][2]=0;
    }
    paint_field(playfield, st);
    send_field(playfield, st);
    usleep(500000);

    //playfield um eins nach gravity
    memcpy(playfield_tmp, playfield, DISP_BUF_SIZE);
    for (y=line; y>0; --y) {
        for (x=0; x<WIDTH; ++x) {
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

int check_for_ready_line(uint8_t playfield[WIDTH][HEIGHT][3]){

    int x, y, line;
 
    for (y=HEIGHT-1; y>=0; --y) {
        line=0;
        for (x=0; x<WIDTH; ++x) {
            if(playfield[x][y][0] != 0 ||
               playfield[x][y][1] != 0 ||
               playfield[x][y][2] != 0)
                line++;
        }
        if (line == WIDTH)
            return y;
    }

    return -1;
}

int check_for_touch(uint8_t playfield[WIDTH][HEIGHT][3],
                    struct stone * st) {
    int x, y;
    for (y=0; y<MAX_STONE_SIZE; ++y) {
        for (x=0; x<MAX_STONE_SIZE; ++x) {
    	    if(st->data[x][y][0] > 0 || st->data[x][y][1] > 0 || st->data[x][y][2] > 0) {
                if (y+st->y >= HEIGHT){
                    if (DEBUG) fprintf(stderr, "WALL ");
                    return 1;
                }
            }
        }
    }

    for (y=0; y<MAX_STONE_SIZE; ++y) {
        for (x=0; x<MAX_STONE_SIZE; ++x) {
            if(st->data[x][y][0] != 0 || st->data[x][y][1] != 0 || st->data[x][y][2] != 0) {
                if(st->x+x >= 0 && st->x+x < WIDTH &&
                   st->y+y >= 0 && st->y+y < HEIGHT){
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

int allow_movement(uint8_t playfield[WIDTH][HEIGHT][3],
                   struct stone * st) {
    int x, y;
    for (y=0; y<MAX_STONE_SIZE; ++y) {
        for (x=0; x<MAX_STONE_SIZE; ++x) {
            if(st->data[x][y][0] > 0 ||
               st->data[x][y][1] > 0 ||
               st->data[x][y][2] > 0) {
                if (x+st->x < 0 || x+st->x >= WIDTH)
                    return 0;
                if (st->x+x >= 0 && st->x+x < WIDTH &&
                    st->y+y >= 0 && st->y+y < HEIGHT) {
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

int config_handler(void * user,
                   const char * name,
                   const char * value) {
    struct tetris_config * tc = (struct tetris_config *)user;

    if (strcmp(value, "delay")) {
        tc->delay = atoi(value);
    }

    return 1;
}

int main(int argc, char * argv[]) {
    uint8_t playfield [WIDTH][HEIGHT][3];

    struct stone st;
    struct stone st_tmp;

    struct tetris_config conf;

    struct controller_handle * ch;
    struct command cmd;

    struct bl_timer * timer;

    int x, y;
    int line_to_remove;

    int stone_next;

    int points = 0;
    int delay;

    conf.delay = 300000;

    // Read configuration
    if (!read_config(config_handler, &conf, "tetris")) {
        fprintf(stderr, "Error reading configuration\n");
        return 1;
    }

    delay = conf.delay;

    srand(time(NULL));

    memset(playfield[0][0], 0, DISP_BUF_SIZE);
    memset(st.data[0][0], 0, MAX_STONE_SIZE * MAX_STONE_SIZE*3);
    memset(st_tmp.data[0][0], 0, MAX_STONE_SIZE * MAX_STONE_SIZE*3);

    st = stones[rand() % NUM_STONES];
    stone_next = rand() % NUM_STONES;

    ch = open_controller(CONTROLLER_TYPE_JOYSTICK |
                         CONTROLLER_TYPE_STDIN);
    if (!ch) {
        fprintf(stderr, "No joysticks/controllers found.\n");
        return -1;
    }

    timer = bl_timer_create();

    while(1) {
        int touched;
        int key = 0;

        cmd = read_command(ch, delay);
        
        if (DEBUG) fprintf(stderr, "WHILE BEGIN\n\r");

        touched = 0;

        if (cmd.value > 0)
            key = cmd.number;

        if (key == KEY_B1 ||
            key == KEY_B1_K1) {
            st_tmp = rotate_stone(st);
        } else if (key == KEY_B2 ||
                   key == KEY_B2_K2) {
            int i;
            for (i=0; i<3; ++i)
                st_tmp = rotate_stone(st);
        } else {
            st_tmp = st;
        }
        if (allow_movement(playfield, &st_tmp))
            st = st_tmp;

        st_tmp = st;
        if (key == KEY_LEFT || key == KEY_LEFT_K1) {
            st_tmp.x++;
        } else if (key == KEY_RIGHT || key == KEY_RIGHT_K1) {
            st_tmp.x--;
        } else if (key == KEY_DOWN || key == KEY_DOWN_K1) {
            delay /= 10;
        } else if (key == KEY_QUIT || key == KEY_QUIT_K) {
            goto exit;
        } else {
            st_tmp = st;
        }

        if (allow_movement(playfield, &st_tmp)) {
            st = st_tmp;
        }

        if (bl_timer_elapsed(timer) > delay) {
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
                delay = conf.delay;
            }
	
            lines_removed = 0;
            while ((line_to_remove = check_for_ready_line(playfield)) != -1){
                if (DEBUG) fprintf(stderr, "LINE READY %d\n\r", line_to_remove);
                move_playfield_down(playfield, &st, line_to_remove);
                lines_removed++;
                points += (lines_removed * 100);
            }

            bl_timer_start(timer);
        }

        paint_field(playfield, &st);
        send_field(playfield, &st);
    }

exit:
    bl_timer_free(timer);
    close_controller(ch);
    return 0;
}
