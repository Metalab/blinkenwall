// Blinkenwall snake

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define WALL_WIDTH  9
#define WALL_HEIGHT 5
#define WALL_SIZE WALL_WIDTH * WALL_HEIGHT

#define FRAME_DELAY 300000

#define KEY_UP    'w'
#define KEY_DOWN  's'
#define KEY_LEFT  'a'
#define KEY_RIGHT 'd'

typedef struct coord {
    int x;
    int y;
} coord_t;

struct coord new_food(struct coord * snake, int snake_length) {
    struct coord food;
    int i, food_in_snake;
    while(1) {
        food.x = rand() % WALL_WIDTH;
        food.y = rand() % WALL_HEIGHT;
        food_in_snake = 0;
        for (i=0; i<snake_length; ++i) {
            if (food.x == snake[i].x &&
                food.y == snake[i].y)
                food_in_snake = 1;
        }
        if (food_in_snake == 0)
            return food;
    }
}

void paint_field(struct coord * snake, int snake_length,
                 struct coord food) {
    char field[WALL_WIDTH][WALL_HEIGHT];
    int i, x, y;

    memset(field, '.', WALL_SIZE);

    field[food.x][food.y] = '&';

    field[snake[0].x][snake[0].y] = 'O';
    for (i=1; i<snake_length; ++i)
        field[snake[i].x][snake[i].y] = '#';

    for (y=0; y<WALL_HEIGHT; ++y) {
        for (x=0; x<WALL_WIDTH; ++x) {
            fprintf(stderr, "%c", field[x][y]);
        }
        fprintf(stderr, "\n\r");
    }
    fprintf(stderr, "\n\r");
}

void send_field(struct coord * snake, int snake_length,
                struct coord food) {
    uint8_t field[WALL_HEIGHT][WALL_WIDTH][3];
    int i, x, y;

    memset(field, 0, WALL_SIZE * 3);

    field[food.y][food.x][0] = 255;

    field[snake[0].y][snake[0].x][2] = 255;
    for (i=1; i<snake_length; ++i)
        field[snake[i].y][snake[i].x][1] = 255;

    fwrite(field, 1, WALL_WIDTH*WALL_HEIGHT*3, stdout);
    fflush(stdout);
}

int main(int argc, char * argv[]) {
    struct coord snake[WALL_SIZE];
    struct coord dir;
    struct coord food;
    int i;
    int snake_length;

    system ("/bin/stty -echo raw");

    srand (time(NULL));

    memset(snake, 0, WALL_SIZE);

    snake[0].x = WALL_WIDTH / 2;
    snake[0].y = WALL_HEIGHT / 2;
    snake[0].x = WALL_WIDTH / 2 + 1;
    snake[0].y = WALL_HEIGHT / 2;
    snake_length = 2;

    dir.x = -1;
    dir.y = 0;

    food = new_food(snake, snake_length);

    while(1) {
        fd_set readfds;
        struct timeval tv;
        int input;
        int retval = 0;

        FD_ZERO(&readfds);
        FD_SET(0, &readfds);
        tv.tv_sec = 0;
        tv.tv_usec = FRAME_DELAY;

        retval = select(1, &readfds, NULL, NULL, &tv);

        if (retval > 0) {
            int input = getchar();

            switch(input) {
            case KEY_UP:
                if (dir.y != 1) {
                    dir.x = 0;
                    dir.y = -1;
                }
                break;
            case KEY_DOWN:
                if (dir.y != -1) {
                    dir.x = 0;
                    dir.y = 1;
                }
                break;
            case KEY_LEFT:
                if (dir.x != 1) {
                    dir.x = -1;
                    dir.y = 0;
                }
                break;
            case KEY_RIGHT:
                if (dir.x != -1) {
                    dir.x = 1;
                    dir.y = 0;
                }
                break;
            default:
                break;
            }
        }

        if (snake[0].x == food.x &&
            snake[0].y == food.y) {
            snake_length++;
            snake[snake_length] = snake[snake_length-1];
            food = new_food(snake, snake_length);
        }

        for (i=snake_length-1; i>=1; --i)
            snake[i] = snake[i-1];
        snake[0].x += dir.x;
        snake[0].y += dir.y;

        if (snake[0].x < 0 || snake[0].x >= WALL_WIDTH ||
            snake[0].y < 0 || snake[0].y >= WALL_HEIGHT) {
            fprintf(stderr, "Wand du trottel\n\r");
            goto exit;
        }

        for (i=1; i<snake_length; ++i) {
            if (snake[0].x == snake[i].x &&
                snake[0].y == snake[i].y) {
                fprintf(stderr, "Nom nom nom\n\r");
                goto exit;
            }
        }

        send_field(snake, snake_length, food);
        paint_field(snake, snake_length, food);
    }

exit:
    system ("/bin/stty echo cooked");
    return 0;

}
