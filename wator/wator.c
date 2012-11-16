#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>

#define TYPE_WATER        0
#define ANIMAL_TYPE_SHARK 1
#define ANIMAL_TYPE_FISH  2

#define FISH_BREED_TIME   5
#define SHARK_BREED_TIME  10
#define SHARK_ENERGY      10
#define SHARK_EAT_ENERGY  5
#define SHARK_MOVE_ENERGY 1

#define FIELD_WIDTH       24
#define FIELD_HEIGHT      24

#define COLOR_WATER_R     0
#define COLOR_WATER_G     0
#define COLOR_WATER_B     0

#define COLOR_FISH_R      0
#define COLOR_FISH_G      255
#define COLOR_FISH_B      0

#define COLOR_SHARK_R     0
#define COLOR_SHARK_G     0
#define COLOR_SHARK_B     255

struct animal
{
    int type;
    int age;
    int energy;
};

void draw_field(struct animal * sea)
{
    uint8_t field[FIELD_HEIGHT * FIELD_WIDTH * 3];
    int x, y;

    for (y=0; y<FIELD_WIDTH; y++)
    {
        for (x=0; x<FIELD_HEIGHT; x++)
        {
            int pos = (y * FIELD_WIDTH + x);
            int fieldpos = pos * 3;
            switch(sea[pos].type) {
            case TYPE_WATER:
                field[fieldpos+0] = COLOR_WATER_R;
                field[fieldpos+1] = COLOR_WATER_G;
                field[fieldpos+2] = COLOR_WATER_B;
                break;
            case ANIMAL_TYPE_FISH:
                field[fieldpos+0] = COLOR_FISH_R;
                field[fieldpos+1] = COLOR_FISH_G;
                field[fieldpos+2] = COLOR_FISH_B;
                break;
            case ANIMAL_TYPE_SHARK:
                field[fieldpos+0] = COLOR_SHARK_R;
                field[fieldpos+1] = COLOR_SHARK_G;
                field[fieldpos+2] = COLOR_SHARK_B;
                break;
            default:
                break;
            }
        }
    }

    write(STDOUT_FILENO, field, FIELD_HEIGHT * FIELD_WIDTH * 3);
}

void fill_sea_randomized(struct animal * sea)
{
    int x, y, pos;

    for (y=0; y<FIELD_WIDTH; y++)
    {
        for (x=0; x<FIELD_HEIGHT; x++)
        {
            pos = y * FIELD_WIDTH + x;
            sea[pos].type = rand() % 20;
            if (sea[pos].type > ANIMAL_TYPE_FISH)
                sea[pos].type = ANIMAL_TYPE_FISH;
            if (sea[pos].type == ANIMAL_TYPE_FISH)
                sea[pos].age = rand() % FISH_BREED_TIME;
            else if (sea[pos].type == ANIMAL_TYPE_SHARK)
                sea[pos].age = rand() % SHARK_BREED_TIME;
            sea[pos].energy = SHARK_ENERGY;
        }
    }
}

int get_newpos_from_i(int x, int y, int i)
{
    int mx, my, pos;
    switch(i) {
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
    if (x < 0)
        x = FIELD_WIDTH - 1;
    else if (x >= FIELD_WIDTH)
        x = 0;
    if (y < 0)
        y = FIELD_WIDTH - 1;
    else if (y >= FIELD_HEIGHT)
        y = 0;
    pos = y * FIELD_WIDTH + x;
}

void move(struct animal * sea, struct animal * sea2)
{
    int i, x, y;
    int sharks = 0;
    memset(sea2, 0, FIELD_HEIGHT * FIELD_WIDTH * sizeof(struct animal));
    for (y=0; y<FIELD_WIDTH; y++)
    {
        for (x=0; x<FIELD_HEIGHT; x++)
        {
            int oldpos, pos, px, py, dirs = 0;
            int dir[4];
            oldpos = y * FIELD_WIDTH + x;
            if (sea[oldpos].type == ANIMAL_TYPE_FISH)
            {
                sea[oldpos].age++;
                for (i=0; i<4; i++) {
                    pos = get_newpos_from_i(x, y, i);
                    if (sea[pos].type == TYPE_WATER &&
                        sea2[pos].type == TYPE_WATER) {
                        dir[dirs++] = i;
                    }
                }
                if (dirs > 0)
                {
                    int newdir = rand() % dirs;
                    pos = get_newpos_from_i(x, y, dir[newdir]);
                    sea2[pos] = sea[oldpos];
                    if (sea[oldpos].age >= FISH_BREED_TIME) {
                        sea2[oldpos] = sea[oldpos];
                        //sea2[pos].age = rand() % MAX_BIRTH_AGE;
                        //sea2[oldpos].age = rand() % MAX_BIRTH_AGE;
                        sea2[pos].age = 0;
                        sea2[oldpos].age = 0;
                    }
                         
                } else {
                    sea2[oldpos] = sea[oldpos];
                }
            }
        }
    }
    for (y=0; y<FIELD_WIDTH; y++)
    {
        for (x=0; x<FIELD_HEIGHT; x++)
        {
            int oldpos, pos, px, py, dirs = 0;
            int dir[4];
            oldpos = y * FIELD_WIDTH + x;
            if (sea[oldpos].type == ANIMAL_TYPE_SHARK &&
                sea[oldpos].energy > 0) {
                sharks++;
                sea[oldpos].age++;
                sea[oldpos].energy -= SHARK_MOVE_ENERGY;
                for (i=0; i<4; i++) {
                    pos = get_newpos_from_i(x, y, i);
                    if (sea[pos].type != ANIMAL_TYPE_SHARK &&
                        sea2[pos].type != ANIMAL_TYPE_SHARK) {
                        dir[dirs++] = i;
                    }
                }
                if (dirs > 0)
                {
                    int newdir = rand() % dirs;
                    pos = get_newpos_from_i(x, y, dir[newdir]);
                    if (sea[pos].type == ANIMAL_TYPE_FISH)
                        sea[oldpos].energy += SHARK_EAT_ENERGY;
                    sea2[pos] = sea[oldpos];
                    if (sea[oldpos].age >= SHARK_BREED_TIME) {
                        sea[oldpos].energy /= 2;
                        sea2[pos].energy /= 2;
                        sea2[oldpos] = sea[oldpos];
                        //sea2[pos].age = rand() % MAX_BIRTH_AGE;
                        //sea2[oldpos].age = rand() % MAX_BIRTH_AGE;
                        //sea2[pos].energy = SHARK_ENERGY;
                        sea2[pos].age = 0;
                        sea2[oldpos].age = 0;
                    }
                } else {
                    sea2[oldpos] = sea[oldpos];
                }
            }
        }
    }
    fprintf(stderr, "Sharks: %d\n", sharks);
}

int check_all_fish(struct animal * sea)
{
    int x, y, allfish = 1;
    for (y=0; y<FIELD_WIDTH; y++)
    {
        for (x=0; x<FIELD_HEIGHT; x++)
        {
            int pos = y * FIELD_WIDTH + x;
            if (sea[pos].type != ANIMAL_TYPE_FISH)
                allfish = 0;
        }
    }
    return allfish;
}

int check_all_water(struct animal * sea)
{
    int x, y, allwater = 1;
    for (y=0; y<FIELD_WIDTH; y++)
    {
        for (x=0; x<FIELD_HEIGHT; x++)
        {
            int pos = y * FIELD_WIDTH + x;
            if (sea[pos].type != TYPE_WATER)
                allwater = 0;
        }
    }
    return allwater;
}

int main(int argc, char * argv[])
{
    struct animal * sea = malloc(FIELD_HEIGHT * FIELD_WIDTH * sizeof(struct animal));
    struct animal * sea2 = malloc(FIELD_HEIGHT * FIELD_WIDTH * sizeof(struct animal));

    srand(time(NULL));

    memset(sea, 0, FIELD_HEIGHT * FIELD_WIDTH * sizeof(struct animal));
    memset(sea2, 0, FIELD_HEIGHT * FIELD_WIDTH * sizeof(struct animal));
    fill_sea_randomized(sea);

    while(1)
    {
        move(sea, sea2);
        draw_field(sea);
        memcpy(sea, sea2, FIELD_HEIGHT * FIELD_WIDTH * sizeof(struct animal));
        usleep(250000);
        if (check_all_fish(sea)) {
            int pos = (FIELD_HEIGHT * FIELD_WIDTH) / 2;
            sea[pos].type = ANIMAL_TYPE_SHARK;
            sea[pos].age = 0;
            sea[pos].energy = SHARK_ENERGY;
        } else if (check_all_fish(sea)) {
            check_all_fish(sea);
        }
    }

    free(sea2);
    free(sea);

    return 0;
}
