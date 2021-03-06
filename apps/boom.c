#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <math.h>
#include <time.h>
#include "common.h"
#include "input.h"

#define MAX_DISTANCE        33.941125497

#define FIELD_TYPE_EMPTY    0
#define FIELD_TYPE_HARDWALL 1
#define FIELD_TYPE_WALL     2

#define FIELD_SPECIAL_NONE  0
#define FIELD_SPECIAL_FIRE  1
#define FIELD_SPECIAL_BOMB  2
#define FIELD_SPECIAL_RED   3

#define DEFAULT_BOMB_TICKS  15
#define BEGIN_FIRE          2
#define BEGIN_BOMBS         1

#define PLAYERS             2
#define GHOST_MODE          0
#define SHOW_SPECIAL_FIELDS 0
#define ITEM_MAX_RAND       20
#define FILL_RANDOMIZED     0
#define MONSTERS            6
#define MONSTER_SPEED       10

#define DELAY               66666

struct playfield
{
    int type;
    int special;
};

struct player
{
    int x;
    int y;
    int fire;
    int max_bombs;
    int cur_bombs;
    int has_red_bombs;
    int alive;
    int last_move;
};

struct bomb
{
    int x;
    int y;
    int ticks;
    int is_red;
    int length;
    int current_length;
    int max_length[4];
    int from_player;
};

struct monster
{
    int x;
    int y;
    int speed;
    int alive;
};

int get_newpos_from_i(int x, int y, int * dx, int * dy, int i)
{
    int mx, my, pos;
    switch (i) {
    case 0:
        mx = 1;
        my = 0;
        break;
    case 1:
        mx = -1;
        my = 0;
        break;
    case 2:
        mx = 0;
        my = 1;
        break;
    case 3:
        mx = 0;
        my = -1;
        break;
    default:
        break;
    }

    x = x + mx;
    y = y + my;
    if (x < 0) {
        x = 0;
        mx = 0;
    }
    else if (x >= WIDTH) {
        x = WIDTH - 1;
        mx = 0;
    }
    if (y < 0) {
        y = 0;
        my = 0;
    }
    else if (y >= HEIGHT) {
        y = HEIGHT - 1;
        my = 0;
    }

    pos = y * WIDTH + x;

    *dx = mx;
    *dy = my;

    return pos;
}

void draw_field(struct playfield * f, struct player * pl,
                struct bomb * bomb, int num_bombs,
                struct monster * mons, int num_mons)
{
    uint8_t field[HEIGHT * WIDTH * 3];
    int playerpos;
    int x, y, i, j, k;

    for (y=0; y<WIDTH; y++)
    {
        for (x=0; x<HEIGHT; x++)
        {
            int pos = (y * WIDTH + x);
            int fieldpos = pos * 3;

            switch (f[pos].type) {
            case FIELD_TYPE_EMPTY:
                field[fieldpos+0] = 0;
                field[fieldpos+1] = 0;
                field[fieldpos+2] = 0;
                break;
            case FIELD_TYPE_HARDWALL:
                field[fieldpos+0] = 255;
                field[fieldpos+1] = 255;
                field[fieldpos+2] = 255;
                break;
            case FIELD_TYPE_WALL:
                field[fieldpos+0] = 92;
                field[fieldpos+1] = 35;
                field[fieldpos+2] = 0;
                break;
            default:
                break;
            }

            if (f[pos].type == FIELD_TYPE_EMPTY ||
                (SHOW_SPECIAL_FIELDS && f[pos].type == FIELD_TYPE_WALL)) {
                switch (f[pos].special) {
                case FIELD_SPECIAL_FIRE:
                    field[fieldpos+0] = 219;
                    field[fieldpos+1] = 81;
                    field[fieldpos+2] = 0;
                    break;
                case FIELD_SPECIAL_BOMB:
                    field[fieldpos+0] = 48;
                    field[fieldpos+1] = 48;
                    field[fieldpos+2] = 48;
                    break;
                case FIELD_SPECIAL_RED:
                    field[fieldpos+0] = 243;
                    field[fieldpos+1] = 0;
                    field[fieldpos+2] = 0;
                    break;
                }
            }
        }
    }

    for (i=0; i<num_bombs; i++)
    {
        int fieldpos = (bomb[i].y * WIDTH + bomb[i].x) * 3;
        if (bomb[i].ticks > 0) {
            if (bomb[i].ticks % 2 == 0) {
                if (bomb[i].is_red) {
                    field[fieldpos+0] = 121;
                    field[fieldpos+1] = 0;
                    field[fieldpos+2] = 0;
                } else {
                    field[fieldpos+0] = 0;
                    field[fieldpos+1] = 0;
                    field[fieldpos+2] = 0;
                }
            } else {
                if (bomb[i].is_red) {
                    field[fieldpos+0] = 243;
                    field[fieldpos+1] = 0;
                    field[fieldpos+2] = 0;
                } else {
                    field[fieldpos+0] = 48;
                    field[fieldpos+1] = 48;
                    field[fieldpos+2] = 48;
                }
            }
        }
    }

    // Player colors

    if (pl[0].alive) {
        playerpos = (pl[0].y * WIDTH + pl[0].x) * 3;
        field[playerpos+0] = 0;
        field[playerpos+1] = 0;
        field[playerpos+2] = 211;
    }
    if (pl[1].alive) {
        playerpos = (pl[1].y * WIDTH + pl[1].x) * 3;
        field[playerpos+0] = 70;
        field[playerpos+1] = 255;
        field[playerpos+2] = 70;
    }
    if (pl[2].alive) {
        playerpos = (pl[2].y * WIDTH + pl[2].x) * 3;
        field[playerpos+0] = 211;
        field[playerpos+1] = 0;
        field[playerpos+2] = 0;
    }
    if (pl[3].alive) {
        playerpos = (pl[3].y * WIDTH + pl[3].x) * 3;
        field[playerpos+0] = 146;
        field[playerpos+1] = 146;
        field[playerpos+2] = 146;
    }

    //TODO: Colors for ghost mode

    for (i=0; i<num_mons; i++)
    {
        int fieldpos = (mons[i].y * WIDTH + mons[i].x) * 3;
        if (mons[i].alive) {
            field[fieldpos+0] = 213;
            field[fieldpos+1] = 60;
            field[fieldpos+2] = 139;
        }
    }

    for (i=0; i<num_bombs; i++)
    {
        if (bomb[i].ticks <= 0) {
            for (j=0; j<4; j++) {
                int dx, dy;
                int pos = get_newpos_from_i(bomb[i].x, bomb[i].y, &dx, &dy, j);
                if (f[pos].type != FIELD_TYPE_HARDWALL)
                {
                    int count_to = bomb[i].current_length < bomb[i].max_length[j] ?
                        bomb[i].current_length : bomb[i].max_length[j];
                    for (k=0; k<count_to; k++) {
                        int nx = bomb[i].x + k * dx;
                        int ny = bomb[i].y + k * dy;
                        if (nx >= 0 && nx < WIDTH &&
                            ny >= 0 && ny < HEIGHT) {
                            int npos = (ny * WIDTH + nx) * 3;
                            field[npos+0] = 243;
                            field[npos+1] = 113;
                            field[npos+2] = 0;
                        }
                    }
                }
            }
        }
    }

    write(STDOUT_FILENO, field, HEIGHT * WIDTH * 3);
}

void populate_playfield(struct playfield * pf,
                        struct monster * mons, int num_mons)
{
    int x, y, i;

    for (y=0; y<WIDTH; y++)
    {
        for (x=0; x<HEIGHT; x++)
        {
            int pos = (y * WIDTH + x);
            int special = rand() % ITEM_MAX_RAND;
            if (FILL_RANDOMIZED && rand() % FILL_RANDOMIZED == 0) {
                pf[pos].type = FIELD_TYPE_EMPTY;
            } else {
                pf[pos].type = FIELD_TYPE_WALL;
            }
            switch (special) {
            case 0:
                pf[pos].special = FIELD_SPECIAL_FIRE;
                break;
            case 1:
                pf[pos].special = FIELD_SPECIAL_BOMB;
                break;
            case 2:
                pf[pos].special = FIELD_SPECIAL_RED;
                break;
            default:
                pf[pos].special = FIELD_SPECIAL_NONE;
            }
        }
    }

    pf[0].type = FIELD_TYPE_EMPTY;
    pf[1].type = FIELD_TYPE_EMPTY;
    pf[WIDTH].type = FIELD_TYPE_EMPTY;
    pf[WIDTH+1].type = FIELD_TYPE_EMPTY;

    pf[WIDTH-1].type = FIELD_TYPE_EMPTY;
    pf[WIDTH-2].type = FIELD_TYPE_EMPTY;
    pf[WIDTH*2-1].type = FIELD_TYPE_EMPTY;
    pf[WIDTH*2-2].type = FIELD_TYPE_EMPTY;

    pf[(HEIGHT-1)*WIDTH].type = FIELD_TYPE_EMPTY;
    pf[(HEIGHT-1)*WIDTH+1].type = FIELD_TYPE_EMPTY;
    pf[(HEIGHT-2)*WIDTH].type = FIELD_TYPE_EMPTY;
    pf[(HEIGHT-2)*WIDTH+1].type = FIELD_TYPE_EMPTY;

    pf[HEIGHT*WIDTH-1].type = FIELD_TYPE_EMPTY;
    pf[HEIGHT*WIDTH-2].type = FIELD_TYPE_EMPTY;
    pf[HEIGHT*WIDTH-3].type = FIELD_TYPE_EMPTY;
    pf[HEIGHT*(WIDTH-1)-1].type = FIELD_TYPE_EMPTY;
    pf[HEIGHT*(WIDTH-1)-2].type = FIELD_TYPE_EMPTY;
    pf[HEIGHT*(WIDTH-1)-3].type = FIELD_TYPE_EMPTY;

    for (y=1; y<WIDTH; y+=2)
    {
        for (x=1; x<HEIGHT; x+=2)
        {
            int pos = (y * WIDTH + x);
            pf[pos].type = FIELD_TYPE_HARDWALL;
        }
    }

    for (i=0; i<num_mons; i++){
        while(1){
            int pos;
            x=rand() % WIDTH;
            y=rand() % HEIGHT;
            pos = (y * WIDTH + x);
            if(pf[pos].type == FIELD_TYPE_HARDWALL ||
               pos == 0 || pos == 1 || pos == WIDTH || pos == WIDTH+1 ||
               pos == WIDTH-1 || pos == WIDTH-2 ||
               pos == WIDTH*2-1 || pos == WIDTH*2-2 ||
               pos == (HEIGHT-1)*WIDTH ||
               pos == (HEIGHT-1)*WIDTH+1 ||
               pos == (HEIGHT-2)*WIDTH ||
               pos == (HEIGHT-2)*WIDTH+1 ||
               pos == HEIGHT*WIDTH-1 ||
               pos == HEIGHT*WIDTH-2 ||
               pos == HEIGHT*WIDTH-3 ||
               pos == HEIGHT*(WIDTH-1)-1 ||
               pos == HEIGHT*(WIDTH-1)-2 ||
               pos == HEIGHT*(WIDTH-1)-3) {
                continue;
            } else {
                mons[i].x = x;
                mons[i].y = y;
                mons[i].speed = MONSTER_SPEED;
                mons[i].alive = 1;
                pf[pos].type = FIELD_TYPE_EMPTY;
                break;
            }
        }
    }
}

int is_bomb(int x, int y, struct bomb * bm, int n_bombs)
{
    int j;
    int is_bomb = 0;
    for (j=0; j<n_bombs; j++) {
        if (bm[j].x == x &&
            bm[j].y == y) {
            is_bomb = 1;
        }
    }

    return is_bomb;
}

int main(int argc, char * argv[])
{
    struct playfield * field = malloc(HEIGHT * WIDTH *
                                      sizeof(struct playfield));

    struct player pl[4];
    struct monster mons[MONSTERS];
    struct bomb bm[(3 * WIDTH * HEIGHT) / 4];
    struct controller_handle * ch;
    struct command cmd;
    struct bl_timer * timer;
    int frame = 0;
    int n_bombs = 0;
    int n_monsters = MONSTERS;
    int mx[4], my[4];
    int last_ctype = 0;
    int p = 0;
    int i, j, k, l;

    ch = open_controller(CONTROLLER_TYPE_STDIN);

    if (!ch) {
        fprintf(stderr, "No joysticks/controllers found.\n");
        return -1;
    }

    srand(time(NULL));

    for (i=0; i<PLAYERS; i++) {
        mx[i] = 0;
        my[i] = 0;
        pl[i].fire = BEGIN_FIRE;
        pl[i].max_bombs = BEGIN_BOMBS;
        pl[i].cur_bombs = 0;
        pl[i].has_red_bombs = 0;
        pl[i].alive = 1;
        pl[i].last_move = 0;
    }

    for (i=0; i<MONSTERS; i++) {
        mons[i].x = WIDTH-1;
        mons[i].y = 0;
        mons[i].speed = 10;
        mons[i].alive = 1;
    }

    pl[0].x = 0;
    pl[0].y = 0;
    pl[1].x = WIDTH - 1 - ((WIDTH+1) % 2);
    pl[1].y = HEIGHT - 1;
    pl[2].x = WIDTH - 1 - ((WIDTH+1) % 2);
    pl[2].y = 0;
    pl[3].x = ((HEIGHT+1) % 2);
    pl[3].y = HEIGHT - 1;

    populate_playfield(field, mons, n_monsters);

    timer = bl_timer_create();

    while(1) {
        cmd = read_command(ch, DELAY);

        if (cmd.controller >= 0)
            p = cmd.controller;

        if ((cmd.type == 2 && cmd.number == KEY_QUIT) ||
            (cmd.type == IN_TYPE_STDIN && cmd.number == KEY_QUIT_K))
            goto exit;

        if (cmd.type > 0)
            last_ctype = cmd.type;

        if (cmd.controller >= 0) {
            switch (cmd.number) {
            case KEY_UP_K1:
            case KEY_UP_K2:
            case KEY_UP:
                if (cmd.value > 0) {
                    mx[p] = 0;
                    my[p] = -1;
                } else {
                    mx[p] = 0;
                    my[p] = 0;
                    pl[p].last_move = 0;
                }
                break;
            case KEY_DOWN_K1:
            case KEY_DOWN_K2:
            case KEY_DOWN:
                if (cmd.value > 0) {
                    mx[p] = 0;
                    my[p] = 1;
                } else {
                    mx[p] = 0;
                    my[p] = 0;
                    pl[p].last_move = 0;
                }
                break;
            case KEY_LEFT_K1:
            case KEY_LEFT_K2:
            case KEY_LEFT:
                if (cmd.value > 0) {
                    mx[p] = -1;
                    my[p] = 0;
                } else {
                    mx[p] = 0;
                    my[p] = 0;
                    pl[p].last_move = 0;
                }
                break;
            case KEY_RIGHT_K1:
            case KEY_RIGHT_K2:
            case KEY_RIGHT:
                if (cmd.value > 0) {
                    mx[p] = 1;
                    my[p] = 0;
                } else {
                    mx[p] = 0;
                    my[p] = 0;
                    pl[p].last_move = 0;
                }
                break;
            case KEY_B1_K1:
            case KEY_B1_K2:
            case KEY_B1:
                if (pl[p].alive && pl[p].cur_bombs < pl[p].max_bombs &&
                    !is_bomb(pl[p].x, pl[p].y, bm, n_bombs)) {
                    bm[n_bombs].x = pl[p].x;
                    bm[n_bombs].y = pl[p].y;
                    bm[n_bombs].ticks = DEFAULT_BOMB_TICKS;
                    bm[n_bombs].is_red = pl[p].has_red_bombs;
                    bm[n_bombs].length = pl[p].fire;
                    bm[n_bombs].max_length[0] = bm[n_bombs].length;
                    bm[n_bombs].max_length[1] = bm[n_bombs].length;
                    bm[n_bombs].max_length[2] = bm[n_bombs].length;
                    bm[n_bombs].max_length[3] = bm[n_bombs].length;
                    bm[n_bombs].current_length = 0;
                    bm[n_bombs].from_player = p;
                    pl[p].cur_bombs++;
                    n_bombs++;
                }
                break;
            default:
                break;     
            }
        }

        if (bl_timer_elapsed(timer) > DELAY) {

            for (i=0; i<n_monsters; i++) {
                if (frame % mons[i].speed == 0) {
                    int dx, dy, nx, ny, npos;
                    int dir = rand() % 4;
                    npos = get_newpos_from_i(mons[i].x, mons[i].y, &dx, &dy, dir);
                    nx = mons[i].x + dx;
                    ny = mons[i].y + dy;
                    if (nx >= 0 && nx < WIDTH &&
                        ny >= 0 && ny < HEIGHT &&
                        field[npos].type == FIELD_TYPE_EMPTY) {
                        if (!is_bomb(nx, ny, bm, n_bombs)) {
                            mons[i].x = nx;
                            mons[i].y = ny;
                        }
                    }
                }
            }

            for (i=0; i<PLAYERS; i++) {
                if (frame > pl[i].last_move + 1) {
                    int nx, ny, npos;
                    nx = pl[i].x + mx[i];
                    ny = pl[i].y + my[i];
                    npos = ny * WIDTH + nx;
                    if (nx >= 0 && nx < WIDTH &&
                        ny >= 0 && ny < HEIGHT &&
                        field[npos].type == FIELD_TYPE_EMPTY) {
                        if (!is_bomb(nx, ny, bm, n_bombs)) {
                            if (pl[i].alive &&
                                field[npos].special != FIELD_SPECIAL_NONE) {
                                switch (field[npos].special) {
                                case FIELD_SPECIAL_FIRE:
                                    pl[i].fire++;
                                    break;
                                case FIELD_SPECIAL_BOMB:
                                    pl[i].max_bombs++;
                                    break;
                                case FIELD_SPECIAL_RED:
                                    pl[i].has_red_bombs = 1;
                                    break;
                                default:
                                    break;
                                }
                                field[npos].special = FIELD_SPECIAL_NONE;
                            }

                            pl[i].x = nx;
                            pl[i].y = ny;
                        }
                    }
                    if (mx[i] != 0 || my[i] != 0) {
                        pl[i].last_move = frame;
                    }
                }
            }

            for (i=0; i<n_bombs; i++) {
                if (bm[i].current_length == bm[i].length) {
                    pl[bm[i].from_player].cur_bombs--;
                    n_bombs--;
                    for (j=i; j<n_bombs; j++) {
                        bm[j] = bm[j+1];
                    }
                }
            }

            for (i=0; i<n_bombs; i++)
            {
                if (bm[i].ticks == 0) {
                    if (bm[i].current_length < bm[i].length) {
                        bm[i].current_length++;
                    }
                } else {
                    if (frame % 3 == 0) {
                        bm[i].ticks--;
#ifdef RUMBLE
                        if (bm[i].ticks == 0) {
                            for (j=0; j<PLAYERS; j++) {
                                double power;
                                double dist = sqrt(pow((bm[i].x - pl[j].x), 2) +
                                                   pow((bm[i].y - pl[j].y), 2));
                                power = dist / MAX_DISTANCE;
                                if (power > 0.2) {
                                    joyrumble(j+1, 100, 100, 500);
                                }
                            }
                        }
#endif
                    }
                }

                if (bm[i].ticks == 0)
                {
                    for (j=0; j<4; j++) {
                        int dx, dy;
                        int pos = get_newpos_from_i(bm[i].x, bm[i].y, &dx, &dy, j);
                        if (field[pos].type != FIELD_TYPE_HARDWALL) {
                            for (k=0; k<bm[i].current_length; k++) {
                                int nx = bm[i].x + k * dx;
                                int ny = bm[i].y + k * dy;
                                if (nx >= 0 && nx < WIDTH &&
                                    ny >= 0 && ny < HEIGHT) {
                                    int npos = ny * WIDTH + nx;

                                    if (k >= bm[i].max_length[j])
                                        break;

                                    if (!bm[i].is_red &&
                                        field[npos].type == FIELD_TYPE_WALL) {
                                        bm[i].max_length[j] = k + 1;
                                    }

                                    for (l=0; l<PLAYERS; l++) {
                                        if (pl[l].x == nx && pl[l].y == ny) {
                                            pl[l].alive = 0;
                                        }
                                    }
                                    for (l=0; l<n_monsters; l++) {
                                        if (mons[l].x == nx && mons[l].y == ny) {
                                            mons[l].alive = 0;
                                        }
                                    }
                                    for (l=0; l<n_bombs; l++)
                                    {
                                        if (bm[l].x == nx && bm[l].y == ny &&
                                            bm[l].ticks > 1) {
                                            bm[l].ticks = 1;
                                        }
                                    }

                                    if (field[npos].type == FIELD_TYPE_EMPTY &&
                                        k == bm[i].current_length - 1) {
                                        field[npos].special = FIELD_SPECIAL_NONE;
                                    }
                                    field[npos].type = FIELD_TYPE_EMPTY;
                                }
                            }
                        }
                    }
                }
            }

            for (i=0; i<n_monsters; i++) {
                for (j=0; j<PLAYERS;j++) {
                    if (mons[i].alive &&
                        mons[i].x == pl[j].x &&
                        mons[i].y == pl[j].y)
                    {
                        pl[j].alive = 0;
                    }
                }
            }

            draw_field(field, pl, bm, n_bombs, mons, n_monsters);
            frame++;
            bl_timer_start(timer);

            if (last_ctype == IN_TYPE_STDIN) {
                mx[p] = 0;
                my[p] = 0;
                pl[p].last_move = 0;
            }
            last_ctype = 0;
        }
    }

exit:
    free(field);
    close_controller(ch);

    return 0;
}
