// Blinkenwall snake

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/time.h>
#include <unistd.h>
#include "common.h"

#define CMD_QUEUE_SIZE 2

struct coord {
    int x;
    int y;
} coord_t;

struct snake_config {
    int delay;
    int rotate_x;
    int rotate_y;
};

struct coord new_food(struct coord * snake, int snake_length) {
    struct coord food;
    int i, food_in_snake;
    while(1) {
        food.x = rand() % WIDTH;
        food.y = rand() % HEIGHT;
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

void paint_field(struct coord * snake,
                 int snake_length,
                 struct coord food) {
    char field[WIDTH][HEIGHT];
    int i, x, y;

    for (y=0; y<HEIGHT; ++y) {
        for (x=0; x<WIDTH; ++x) {
            field[x][y] = '.';
        }
    }

    field[food.x][food.y] = '&';

    field[snake[0].x][snake[0].y] = 'O';
    for (i=1; i<snake_length; ++i)
        field[snake[i].x][snake[i].y] = '#';

    for (y=0; y<HEIGHT; ++y) {
        for (x=0; x<WIDTH; ++x) {
            fprintf(stderr, "%c", field[x][y]);
        }
        fprintf(stderr, "\n\r");
    }
    fprintf(stderr, "\n\r");
}

void send_field(struct coord * snake, int snake_length,
                struct coord food) {
    uint8_t field[DISP_BUF_SIZE];
    int i;

    memset(field, 0, DISP_BUF_SIZE);

    field[POS3(food.x, food.y)] = 255;

    field[POS3(snake[0].x, snake[0].y)+2] = 255;
    for (i=1; i<snake_length; ++i)
        field[POS3(snake[i].x, snake[i].y)+1] = 255;

    write(STDOUT_FILENO, field, DISP_BUF_SIZE);
}

/* still use a 3-big because it makes stuff easier. writing to the same color
 * components as the rgb version */
void send_field_grayscale(struct coord * snake, int snake_length,
                struct coord food) {
    uint8_t field[DISP_BUF_SIZE];
    int i;

    memset(field, 0, DISP_BUF_SIZE);

    field[POS3(food.x, food.y)] = 0x31;

    field[POS3(snake[0].x, snake[0].y)+2] = 0x31;
    for (i=1; i<snake_length-1; ++i)
        field[POS3(snake[i].x, snake[i].y)+1] = 0x21;
    field[POS3(snake[snake_length-1].x, snake[snake_length-1].y)+1] = 0x11;

    write(STDOUT_FILENO, field, DISP_BUF_SIZE);
}

int config_handler(void * user,
                   const char * name,
                   const char * value) {
    struct snake_config * sc = (struct snake_config *)user;

    if (!strcmp(name, "delay")) {
        sc->delay = atoi(value);
    }
    else if (!strcmp(name, "rotate-horizontal")) {
        sc->rotate_x = atoi(value);
    }
    else if (!strcmp(name, "rotate-vertical")) {
        sc->rotate_y = atoi(value);
    }

    return 1;
}

int main(int argc, char * argv[]) {
    struct coord snake[DISP_SIZE];
    struct coord dir;
    struct coord food;
    struct controller_handle * ch;
    struct command cmd;
    struct bl_timer * timer;
    struct snake_config conf;

    int i;
    int snake_length;

    int cmd_queue[CMD_QUEUE_SIZE];
    int cmdq_read = 0;
    int cmdq_write = 0;
    int cmdq_last = 0;
    int n_cmdq = 0;

    conf.delay = 100000;
    conf.rotate_x = 0;
    conf.rotate_y = 0;

    // Read configuration
    if (!read_config(config_handler, &conf, "snake")) {
        fprintf(stderr, "Error reading configuration\n");
        return 1;
    }

    srand (time(NULL));

    memset(snake, 0, DISP_SIZE * sizeof(struct coord));

    snake[0].x = WIDTH / 2;
    snake[0].y = HEIGHT / 2;
    snake[0].x = WIDTH / 2 + 1;
    snake[0].y = HEIGHT / 2;
    snake_length = 2;

    dir.x = -1;
    dir.y = 0;

    food = new_food(snake, snake_length);

    ch = open_controller(CONTROLLER_TYPE_JOYSTICK |
                         CONTROLLER_TYPE_STDIN);
    if (!ch) {
        fprintf(stderr, "No joysticks/controllers found.\n");
        return -1;
    }

    timer = bl_timer_create();

    while(1) {
        int n_wait = conf.delay - bl_timer_elapsed(timer);
        if (n_wait < 1)
            n_wait = 1;

        cmd = read_command(ch, n_wait);

        if (((cmd.number >= 'a' && cmd.number <= 'w') ||
            (cmd.number >= 8 && cmd.number <= 11)) &&
            cmd.number != cmdq_last &&
            (cmd.value == 1 || cmd.value > 10000) &&
            !n_cmdq < CMD_QUEUE_SIZE) {
            fprintf(stderr, "Cmd: %d\r\n", cmd.number);

            cmd_queue[cmdq_write++ % CMD_QUEUE_SIZE] =
                cmd.number;
            n_cmdq++;
            cmdq_last = cmd.number;
        }

        if (bl_timer_elapsed(timer) < conf.delay)
            continue;

        if (n_cmdq > 0) {
            switch(cmd_queue[cmdq_read++ % CMD_QUEUE_SIZE]) {
            case KEY_UP:
            case KEY_UP_K1:
                if (dir.y == 0) {
                    dir.x = 0;
                    dir.y = -1;
                }
                break;
            case KEY_DOWN:
            case KEY_DOWN_K1:
                if (dir.y == 0) {
                    dir.x = 0;
                    dir.y = 1;
                }
                break;
            case KEY_LEFT:
            case KEY_LEFT_K1:
                if (dir.x == 0) {
                    dir.x = -1;
                    dir.y = 0;
                }
                break;
            case KEY_RIGHT:
            case KEY_RIGHT_K1:
                if (dir.x == 0) {
                    dir.x = 1;
                    dir.y = 0;
                }
                break;
            case KEY_QUIT:
            case KEY_QUIT_K:
                goto exit;
            default:
                break;
            }

            n_cmdq--;
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

        if (conf.rotate_x) {
            if (snake[0].x < 0)
                snake[0].x = WIDTH - 1;
            else if (snake[0].x >= WIDTH)
                snake[0].x = 0;
        }

        if (conf.rotate_y) {
            if (snake[0].y < 0)
                snake[0].y = HEIGHT - 1;
            else if (snake[0].y >= HEIGHT)
                snake[0].y = 0;
        }

        if (snake[0].x < 0 || snake[0].x >= WIDTH ||
            snake[0].y < 0 || snake[0].y >= HEIGHT) {
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

        bl_timer_start(timer);
    }

exit:
    bl_timer_free(timer);
    close_controller(ch);
    return 0;

}
