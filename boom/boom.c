#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>

#define FIELD_WIDTH         24
#define FIELD_HEIGHT        24

#define FIELD_TYPE_EMPTY    0
#define FIELD_TYPE_HARDWALL 1
#define FIELD_TYPE_WALL     2

#define FIELD_SPECIAL_NONE  0
#define FIELD_SPECIAL_FIRE  1
#define FIELD_SPECIAL_BOMB  2

#define DEFAULT_BOMB_TICKS  15
#define BEGIN_FIRE          2
#define BEGIN_BOMBS         1

#define PLAYERS             2
#define GHOST_MODE          0
#define SHOW_SPECIAL_FIELDS 0
#define ITEM_MAX_RAND       20
#define FILL_RANDOMIZED     2
#define MONSTERS            7
#define MONSTER_SPEED       10

#define KEY_UP_P1    'd'
#define KEY_DOWN_P1  'a'
#define KEY_LEFT_P1  'w'
#define KEY_RIGHT_P1 's'
#define KEY_BOMB_P1  'q'
#define KEY_UP_P2    'l'
#define KEY_DOWN_P2  'j'
#define KEY_LEFT_P2  'i'
#define KEY_RIGHT_P2 'k'
#define KEY_BOMB_P2  'u'
#define KEY_QUIT     0x3 // Ctrl+c

#define DELAY        100000

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
    int alive;
};

struct bomb
{
    int x;
    int y;
    int ticks;
    int length;
    int current_length;
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
    else if (x >= FIELD_WIDTH) {
        x = FIELD_WIDTH - 1;
        mx = 0;
    }
    if (y < 0) {
        y = 0;
        my = 0;
    }
    else if (y >= FIELD_HEIGHT) {
        y = FIELD_HEIGHT - 1;
        my = 0;
    }

    pos = y * FIELD_WIDTH + x;

    *dx = mx;
    *dy = my;

    return pos;
}

void draw_field(struct playfield * f, struct player * pl,
                struct bomb * bomb, int num_bombs,
                struct monster * mons, int num_mons)
{
    uint8_t field[FIELD_HEIGHT * FIELD_WIDTH * 3];
    int x, y, i, j, k;

    for (y=0; y<FIELD_WIDTH; y++)
    {
        for (x=0; x<FIELD_HEIGHT; x++)
        {
            int pos = (y * FIELD_WIDTH + x);
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
                    field[fieldpos+0] = 200;
                    field[fieldpos+1] = 0;
                    field[fieldpos+2] = 0;
                    break;
                case FIELD_SPECIAL_BOMB:
                    field[fieldpos+0] = 0;
                    field[fieldpos+1] = 200;
                    field[fieldpos+2] = 0;
                    break;
                }
            }
        }
    }

    for (i=0; i<num_bombs; i++)
    {
        int fieldpos = (bomb[i].y * FIELD_WIDTH + bomb[i].x) * 3;
        if (bomb[i].ticks > 0) {
            if (bomb[i].ticks % 2 == 0) {
                field[fieldpos+0] = 0;
                field[fieldpos+1] = 40;
                field[fieldpos+2] = 0;
            } else {
                field[fieldpos+0] = 0;
                field[fieldpos+1] = 60;
                field[fieldpos+2] = 0;
            }
        }
    }

    for (i=0; i<PLAYERS; i++)
    {
        int fieldpos = (pl[i].y * FIELD_WIDTH + pl[i].x) * 3;
        if (pl[i].alive) {
            field[fieldpos+0] = 0;
            field[fieldpos+1] = 0;
            field[fieldpos+2] = 255;
        } else if (GHOST_MODE) {
            field[fieldpos+0] = 0;
            field[fieldpos+1] = 0;
            field[fieldpos+2] = 50;
        }
    }

    for (i=0; i<num_mons; i++)
    {
        int fieldpos = (mons[i].y * FIELD_WIDTH + mons[i].x) * 3;
        if (mons[i].alive) {
            field[fieldpos+0] = 180;
            field[fieldpos+1] = 0;
            field[fieldpos+2] = 180;
        }
    }

    for (i=0; i<num_bombs; i++)
    {
        int fieldpos = (bomb[i].y * FIELD_WIDTH + bomb[i].x) * 3;
        if (bomb[i].ticks <= 0) {
            for (j=0; j<4; j++) {
                int dx, dy;
                int pos = get_newpos_from_i(bomb[i].x, bomb[i].y, &dx, &dy, j);
                if (f[pos].type != FIELD_TYPE_HARDWALL)
                {
                    for (k=0; k<bomb[i].current_length; k++) {
                        int nx = bomb[i].x + k * dx;
                        int ny = bomb[i].y + k * dy;
                        if (nx >= 0 && nx < FIELD_WIDTH &&
                            ny >= 0 && ny < FIELD_HEIGHT) {
                            int npos = (ny * FIELD_WIDTH + nx) * 3;
                            field[npos+0] = 255;
                            field[npos+1] = 0;
                            field[npos+2] = 0;
                        }
                    }
                }
            }
        }
    }

    write(STDOUT_FILENO, field, FIELD_HEIGHT * FIELD_WIDTH * 3);
}

void populate_playfield(struct playfield * pf,
                        struct monster * mons, int num_mons)
{
    int x, y, i;

    for (y=0; y<FIELD_WIDTH; y++)
    {
        for (x=0; x<FIELD_HEIGHT; x++)
        {
            int pos = (y * FIELD_WIDTH + x);
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
            default:
                pf[pos].special = FIELD_SPECIAL_NONE;
            }
        }
    }

    pf[0].type = FIELD_TYPE_EMPTY;
    pf[1].type = FIELD_TYPE_EMPTY;
    pf[FIELD_WIDTH].type = FIELD_TYPE_EMPTY;
    pf[FIELD_WIDTH+1].type = FIELD_TYPE_EMPTY;

    pf[FIELD_WIDTH-1].type = FIELD_TYPE_EMPTY;
    pf[FIELD_WIDTH-2].type = FIELD_TYPE_EMPTY;
    pf[FIELD_WIDTH*2-1].type = FIELD_TYPE_EMPTY;
    pf[FIELD_WIDTH*2-2].type = FIELD_TYPE_EMPTY;

    pf[(FIELD_HEIGHT-1)*FIELD_WIDTH].type = FIELD_TYPE_EMPTY;
    pf[(FIELD_HEIGHT-1)*FIELD_WIDTH+1].type = FIELD_TYPE_EMPTY;
    pf[(FIELD_HEIGHT-2)*FIELD_WIDTH].type = FIELD_TYPE_EMPTY;
    pf[(FIELD_HEIGHT-2)*FIELD_WIDTH+1].type = FIELD_TYPE_EMPTY;

    pf[FIELD_HEIGHT*FIELD_WIDTH-1].type = FIELD_TYPE_EMPTY;
    pf[FIELD_HEIGHT*FIELD_WIDTH-2].type = FIELD_TYPE_EMPTY;
    pf[FIELD_HEIGHT*FIELD_WIDTH-3].type = FIELD_TYPE_EMPTY;
    pf[FIELD_HEIGHT*(FIELD_WIDTH-1)-1].type = FIELD_TYPE_EMPTY;
    pf[FIELD_HEIGHT*(FIELD_WIDTH-1)-2].type = FIELD_TYPE_EMPTY;
    pf[FIELD_HEIGHT*(FIELD_WIDTH-1)-3].type = FIELD_TYPE_EMPTY;

    for (y=1; y<FIELD_WIDTH; y+=2)
    {
        for (x=1; x<FIELD_HEIGHT; x+=2)
        {
            int pos = (y * FIELD_WIDTH + x);
            pf[pos].type = FIELD_TYPE_HARDWALL;
        }
    }

    if (FILL_RANDOMIZED) {
        for (i=0; i<num_mons; i++){
            while(1){
                int pos;
                x=rand() % FIELD_WIDTH;
                y=rand() % FIELD_HEIGHT;
                pos = (y * FIELD_WIDTH + x);
                if(pf[pos].type != FIELD_TYPE_EMPTY ||
                   pos == 0 || pos == 1 || pos == FIELD_WIDTH || pos == FIELD_WIDTH+1 ||
                   pos == FIELD_WIDTH-1 || pos == FIELD_WIDTH-2 ||
                   pos == FIELD_WIDTH*2-1 || pos == FIELD_WIDTH*2-2 ||
                   pos == (FIELD_HEIGHT-1)*FIELD_WIDTH ||
                   pos == (FIELD_HEIGHT-1)*FIELD_WIDTH+1 ||
                   pos == (FIELD_HEIGHT-2)*FIELD_WIDTH ||
                   pos == (FIELD_HEIGHT-2)*FIELD_WIDTH+1 ||
                   pos == FIELD_HEIGHT*FIELD_WIDTH-1 ||
                   pos == FIELD_HEIGHT*FIELD_WIDTH-2 ||
                   pos == FIELD_HEIGHT*FIELD_WIDTH-3 ||
                   pos == FIELD_HEIGHT*(FIELD_WIDTH-1)-1 ||
                   pos == FIELD_HEIGHT*(FIELD_WIDTH-1)-2 ||
                   pos == FIELD_HEIGHT*(FIELD_WIDTH-1)-3) {
                    continue;
                } else {
                    mons[i].x = x;
                    mons[i].y = y;
                    mons[i].speed = MONSTER_SPEED;
                    mons[i].alive = 1;
                    break;
                }
            }
        }
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
    tv.tv_usec = DELAY;
        
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
    struct playfield * field = malloc(FIELD_HEIGHT * FIELD_WIDTH *
                                      sizeof(struct playfield));

    struct player pl[4];
    struct monster m[MONSTERS];
    struct bomb bm[(3 * FIELD_WIDTH * FIELD_HEIGHT) / 4];
    int frame = 0;
    int n_bombs = 0;
    int n_monsters = MONSTERS;
    int mx[4], my[4];
    int i, j, k, l;

    srand(time(NULL));

    for (i=0; i<PLAYERS; i++) {
        mx[i] = 0;
        my[i] = 0;
        pl[i].fire = BEGIN_FIRE;
        pl[i].max_bombs = BEGIN_BOMBS;
        pl[i].cur_bombs = 0;
        pl[i].alive = 1;
    }


    m[0].x = FIELD_WIDTH-1;
    m[0].y = 0;
    m[0].speed = 10;
    m[0].alive = 1;

    pl[0].x = 0;
    pl[0].y = 0;
    pl[1].x = FIELD_WIDTH - 1 - ((FIELD_WIDTH+1) % 2);
    pl[1].y = FIELD_HEIGHT - 1;
    pl[2].x = FIELD_WIDTH - 1 - ((FIELD_WIDTH+1) % 2);
    pl[2].y = 0;
    pl[3].x = ((FIELD_HEIGHT+1) % 2);
    pl[3].y = FIELD_HEIGHT - 1;

    populate_playfield(field, m, n_monsters);

    system ("/bin/stty -echo raw");

    while(1) {
        char cmd;
        int p;
        cmd = read_command();
        switch (cmd) {
        case KEY_UP_P1:
        case KEY_DOWN_P1:
        case KEY_LEFT_P1:
        case KEY_RIGHT_P1:
        case KEY_BOMB_P1:
            p = 0;
            break;
        case KEY_UP_P2:
        case KEY_DOWN_P2:
        case KEY_LEFT_P2:
        case KEY_RIGHT_P2:
        case KEY_BOMB_P2:
            p = 1;
            break;
        case KEY_QUIT:
            goto exit;
        default:
            break;
        }

        switch (cmd) {
        case KEY_UP_P1:
        case KEY_UP_P2:
            mx[p] = 0;
            my[p] = -1;
            break;
        case KEY_DOWN_P1:
        case KEY_DOWN_P2:
            mx[p] = 0;
            my[p] = 1;
            break;
        case KEY_LEFT_P1:
        case KEY_LEFT_P2:
            mx[p] = -1;
            my[p] = 0;
            break;
        case KEY_RIGHT_P1:
        case KEY_RIGHT_P2:
            mx[p] = 1;
            my[p] = 0;
            break;
        case KEY_BOMB_P1:
        case KEY_BOMB_P2:
            if (pl[p].alive && pl[p].cur_bombs < pl[p].max_bombs &&
                !is_bomb(pl[p].x, pl[p].y, bm, n_bombs)) {
                bm[n_bombs].x = pl[p].x;
                bm[n_bombs].y = pl[p].y;
                bm[n_bombs].ticks = DEFAULT_BOMB_TICKS;
                bm[n_bombs].length = pl[p].fire;
                bm[n_bombs].current_length = 0;
                bm[n_bombs].from_player = p;
                pl[p].cur_bombs++;
                n_bombs++;
            }
            break;
        default:
            break;     
        }
        for (i=0; i<n_bombs; i++)
        {
            int fieldpos = (bm[i].y * FIELD_WIDTH + bm[i].x) * 3;
            if (bm[i].ticks == 0 &&
                bm[i].current_length == bm[i].length) {
                for (j=0; j<4; j++) {
                    int dx, dy;
                    int pos = get_newpos_from_i(bm[i].x, bm[i].y, &dx, &dy, j);
                    if (field[pos].type != FIELD_TYPE_HARDWALL)
                    {
                        for (k=0; k<bm[i].current_length; k++) {
                            int nx = bm[i].x + k * dx;
                            int ny = bm[i].y + k * dy;
                            if (nx >= 0 && nx < FIELD_WIDTH &&
                                ny >= 0 && ny < FIELD_HEIGHT) {
                                int npos = ny * FIELD_WIDTH + nx;
                                for (l=0; l<PLAYERS; l++) {
                                    if (pl[l].x == nx && pl[l].y == ny) {
                                        pl[l].alive = 0;
                                    }
                                }
                                field[npos].type = FIELD_TYPE_EMPTY;
                            }
                        }
                    }
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
                if (bm[i].current_length < bm[i].length)
                    bm[i].current_length++;
            } else {
                if (frame % 3 == 0)
                    bm[i].ticks--;
            }
        }

        for (i=0; i<PLAYERS; i++) {
            int nx, ny, npos;
            nx = pl[i].x + mx[i];
            ny = pl[i].y + my[i];
            npos = ny * FIELD_WIDTH + nx;
            if (nx >= 0 && nx < FIELD_WIDTH &&
                ny >= 0 && ny < FIELD_HEIGHT &&
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
                        default:
                            break;
                        }
                        field[npos].special = FIELD_SPECIAL_NONE;
                    }
                    pl[i].x = nx;
                    pl[i].y = ny;
                }
            }
            mx[i] = 0;
            my[i] = 0;
        }

        for (i=0; i<n_monsters; i++) {
            if (frame % m[i].speed == 0) {
                int dx, dy, nx, ny, npos;
                int dir = rand() % 4;
                npos = get_newpos_from_i(m[i].x, m[i].y, &dx, &dy, dir);
                nx = m[i].x + dx;
                ny = m[i].y + dy;
                if (nx >= 0 && nx < FIELD_WIDTH &&
                    ny >= 0 && ny < FIELD_HEIGHT &&
                    field[npos].type == FIELD_TYPE_EMPTY) {
                    if (!is_bomb(nx, ny, bm, n_bombs)) {
                        m[i].x = nx;
                        m[i].y = ny;
                    }
                }
            }
        }

        draw_field(field, pl, bm, n_bombs, m, n_monsters);
        frame++;
        usleep(80000);
    }

exit:
    free(field);

    printf("\n\r");
    system ("/bin/stty echo cooked");

    return 0;
}
