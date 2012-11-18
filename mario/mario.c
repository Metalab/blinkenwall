#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <SDL/SDL.h>
#include "joystick.h"


#define SCREEN_WIDTH        24
#define SCREEN_HEIGHT       24

#define MAX_LEVEL_WIDTH	    24
#define MAX_LEVEL_HEIGHT    24


#define LEVEL_COLOUR_RED    0
#define LEVEL_COLOUR_GREEN  1
#define LEVEL_COLOUR_BLUE   2
#define LEVEL_CALC	    3


#define PARSE_KEYS_EVERY	    100 //ms
#define DRAW_SCREEN_EVERY	    100 //ms
#define CALC_GRAVITY_EVERY	    300 //ms
#define CALC_MOVE_EVERY	   	    100 //ms

//LEVEL STONES
//0-49		AIR
#define LEVEL_STONE_AIR_START	0
#define LEVEL_STONE_AIR_STOP	49

//50-99		HARD underground
#define LEVEL_STONE_HARD_START	50
#define LEVEL_STONE_HARD_STOP	99
//100-149	Spezial Stones
//150-199	Change to Level Stones

#define KEY_UP        8
#define KEY_DOWN     10
#define KEY_LEFT     11
#define KEY_RIGHT     9
#define KEY_BOMB     18
#define KEY_QUIT     16

struct s_player
{
    int x;		//actual X position
    int y;		//actual Y position
    uint8_t size;	//marios size (small, big, fire-mario)
    int8_t gravity_x;	//gravity direction X
    int8_t gravity_y;	//gravity direction Y
    int8_t next_x_dir;	//next X dir (not checked if possible)
    uint8_t jumping;	//is < 1 if mario is jumping
    
};

struct s_level
{
	int width;
	int height;
	uint8_t data[MAX_LEVEL_WIDTH][MAX_LEVEL_HEIGHT];
};

//this function check if mario is on the same position as a hard underground
//return 0 = mario over "air"
//return 1 = mario over hard wall
uint8_t check_mario_over_wall(struct s_level * level, struct s_player * player){
	if ( level[LEVEL_CALC].data[player[0].x][player[0].y]>=LEVEL_STONE_HARD_START
	     && level[LEVEL_CALC].data[player[0].x][player[0].y]<=LEVEL_STONE_HARD_STOP ){
		return 1;
	}else{
		return 0;
	}
}

//this function check if mario standing on a hard underground
//return 0 = mario standing on "air"
//return 1 = mario standing on hard wall
uint8_t check_mario_stand_wall(struct s_level * level, struct s_player * player){
	if ( level[LEVEL_CALC].data[player[0].x][player[0].y+1]>=LEVEL_STONE_HARD_START
	     && level[LEVEL_CALC].data[player[0].x][player[0].y+1]<=LEVEL_STONE_HARD_STOP ){
		return 1;
	}else{
		return 0;
	}
}

//this function check if mario can move to the next x dir
//return 0 = mario goes in "air"
//return 1 = mario goes in hard wall
uint8_t check_mario_move_wall(struct s_level * level, struct s_player * player){
	if ( level[LEVEL_CALC].data[player[0].x+player[0].next_x_dir][player[0].y]>=LEVEL_STONE_HARD_START
	     && level[LEVEL_CALC].data[player[0].x+player[0].next_x_dir][player[0].y]<=LEVEL_STONE_HARD_STOP ){
		return 1;
	}else{
		return 0;
	}
}


//this function check if mario is under a hard underground
//return 0 = mario is under "air"
//return 1 = mario is under hard wall
uint8_t check_mario_under_wall(struct s_level * level, struct s_player * player){
	if ( level[LEVEL_CALC].data[player[0].x][player[0].y-1]>=LEVEL_STONE_HARD_START
	     && level[LEVEL_CALC].data[player[0].x][player[0].y-1]<=LEVEL_STONE_HARD_STOP ){
		return 1;
	}else{
		return 0;
	}
}

/*
char read_command()
{

    fd_set readfds;
    struct timeval tv;
    int retval = 0;
    char cmd_buf[1024];

        
    FD_ZERO(&readfds);
    FD_SET(0, &readfds);
    tv.tv_sec = 0;
    tv.tv_usec = 0;
        
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
*/

void parse_keys(struct controller_handle * ch, struct s_level * level,
                struct s_player * player){


	int input;
    struct command cmd;

	//fprintf(stderr, "start read command: %d\n",SDL_GetTicks());
	cmd = read_command(ch, 0);
	//fprintf(stderr, "end   read command: %d\n\n",SDL_GetTicks());

	switch(cmd.number) {
		case KEY_UP:
			if(player[0].y > 0){
				if (check_mario_stand_wall(level,player)){
					player[0].jumping=5;
				}
			}
			break;
/*		case 's':
			if(player[0].y < MAX_LEVEL_HEIGHT-1){
				player[0].y++;
			}
			break;
*/
		case KEY_LEFT:
			if(player[0].x > 0){
				player[0].next_x_dir=-1;
			}
			break;
		case KEY_RIGHT:
			if(player[0].x < MAX_LEVEL_WIDTH-1){
				player[0].next_x_dir=1;
			}
			break;
		default:
			break;
        }

}


//this function set the complete level to 0
void clean_level(struct s_level * level){

	uint8_t i;
	int x, y;
	for (i=0;i<4;i++){
		for (y=0; y<MAX_LEVEL_HEIGHT; y++){
			for (x=0; x<MAX_LEVEL_WIDTH; x++){
				level[i].data[x][y]=0;
				level[i].data[x][y]=0;
				level[i].data[x][y]=0;
			}
		}
		level[i].width=0;
		level[i].height=0;
	}
}

//this function loads the level from the level file
void load_level(struct s_level * level, int level_nr){

	clean_level(level);
	level[LEVEL_COLOUR_RED].width=24;
	level[LEVEL_COLOUR_RED].height=24;
	level[LEVEL_COLOUR_GREEN].width=24;
	level[LEVEL_COLOUR_GREEN].height=24;
	level[LEVEL_COLOUR_BLUE].width=24;
	level[LEVEL_COLOUR_BLUE].height=24;
        level[LEVEL_CALC].width=24;
	level[LEVEL_CALC].height=24;
	uint8_t i;
	for (i=0; i<MAX_LEVEL_WIDTH; i++ ){
		level[LEVEL_CALC].data[i][MAX_LEVEL_HEIGHT-1]=52;
		level[LEVEL_COLOUR_GREEN].data[i][MAX_LEVEL_HEIGHT-1]=255;
	}
	
	
	//stufen rechts
	level[LEVEL_CALC].data[23][22]=52;
	level[LEVEL_COLOUR_GREEN].data[23][22]=255;
	level[LEVEL_CALC].data[23][21]=52;
	level[LEVEL_COLOUR_GREEN].data[23][21]=255;
	level[LEVEL_CALC].data[23][20]=52;
	level[LEVEL_COLOUR_GREEN].data[23][20]=255;
	
	level[LEVEL_CALC].data[22][22]=52;
	level[LEVEL_COLOUR_GREEN].data[22][22]=255;
	level[LEVEL_CALC].data[22][21]=52;
	level[LEVEL_COLOUR_GREEN].data[22][21]=255;
	
	level[LEVEL_CALC].data[21][22]=52;
	level[LEVEL_COLOUR_GREEN].data[21][22]=255;
	
	
	
	
	//first floor left
	for (i=0; i<MAX_LEVEL_WIDTH/4; i++ ){
		level[LEVEL_CALC].data[i][MAX_LEVEL_HEIGHT-4]=52;
		level[LEVEL_COLOUR_GREEN].data[i][MAX_LEVEL_HEIGHT-4]=100;
		level[LEVEL_COLOUR_RED].data[i][MAX_LEVEL_HEIGHT-4]=170;
	}
	
	//second floor left
	for (i=MAX_LEVEL_WIDTH/4-1; i<MAX_LEVEL_WIDTH/4 + MAX_LEVEL_WIDTH/4 - 1; i++ ){
		level[LEVEL_CALC].data[i][MAX_LEVEL_HEIGHT-8]=52;
		level[LEVEL_COLOUR_GREEN].data[i][MAX_LEVEL_HEIGHT-8]=100;
		level[LEVEL_COLOUR_RED].data[i][MAX_LEVEL_HEIGHT-8]=170;
	}
	
	//third floor left
	for (i=0; i<MAX_LEVEL_WIDTH/4; i++ ){
		level[LEVEL_CALC].data[i][MAX_LEVEL_HEIGHT-12]=52;
		level[LEVEL_COLOUR_GREEN].data[i][MAX_LEVEL_HEIGHT-12]=100;
		level[LEVEL_COLOUR_RED].data[i][MAX_LEVEL_HEIGHT-12]=170;
	}
	
	//fourth floor left
	for (i=MAX_LEVEL_WIDTH/4-1; i<MAX_LEVEL_WIDTH/4 + MAX_LEVEL_WIDTH/4 - 1; i++ ){
		level[LEVEL_CALC].data[i][MAX_LEVEL_HEIGHT-16]=52;
		level[LEVEL_COLOUR_GREEN].data[i][MAX_LEVEL_HEIGHT-16]=100;
		level[LEVEL_COLOUR_RED].data[i][MAX_LEVEL_HEIGHT-16]=170;
	}
	
	//fifth floor left
	for (i=0; i<MAX_LEVEL_WIDTH/4; i++ ){
		level[LEVEL_CALC].data[i][MAX_LEVEL_HEIGHT-20]=52;
		level[LEVEL_COLOUR_GREEN].data[i][MAX_LEVEL_HEIGHT-20]=100;
		level[LEVEL_COLOUR_RED].data[i][MAX_LEVEL_HEIGHT-20]=170;
	}
	
	//Sun
	level[LEVEL_COLOUR_RED].data[MAX_LEVEL_WIDTH][0]=255;
	level[LEVEL_COLOUR_GREEN].data[MAX_LEVEL_WIDTH][0]=255;
	level[LEVEL_COLOUR_RED].data[MAX_LEVEL_WIDTH][1]=255;
	level[LEVEL_COLOUR_GREEN].data[MAX_LEVEL_WIDTH][1]=255;
	level[LEVEL_COLOUR_RED].data[MAX_LEVEL_WIDTH][2]=255;
	level[LEVEL_COLOUR_GREEN].data[MAX_LEVEL_WIDTH][2]=255;
	
	level[LEVEL_COLOUR_RED].data[MAX_LEVEL_WIDTH-1][0]=255;
	level[LEVEL_COLOUR_GREEN].data[MAX_LEVEL_WIDTH-1][0]=255;
	level[LEVEL_COLOUR_RED].data[MAX_LEVEL_WIDTH-1][1]=255;
	level[LEVEL_COLOUR_GREEN].data[MAX_LEVEL_WIDTH-1][1]=255;
	
	level[LEVEL_COLOUR_RED].data[MAX_LEVEL_WIDTH-2][0]=255;
	level[LEVEL_COLOUR_GREEN].data[MAX_LEVEL_WIDTH-2][0]=255;
	
	
	//Wolke
	level[LEVEL_COLOUR_RED].data[MAX_LEVEL_WIDTH/2][MAX_LEVEL_HEIGHT/4]=170;
	level[LEVEL_COLOUR_GREEN].data[MAX_LEVEL_WIDTH/2][MAX_LEVEL_HEIGHT/4]=170;
	level[LEVEL_COLOUR_BLUE].data[MAX_LEVEL_WIDTH/2][MAX_LEVEL_HEIGHT/4]=255;
	level[LEVEL_COLOUR_RED].data[MAX_LEVEL_WIDTH/2][MAX_LEVEL_HEIGHT/4-1]=170;
	level[LEVEL_COLOUR_GREEN].data[MAX_LEVEL_WIDTH/2][MAX_LEVEL_HEIGHT/4-1]=170;
	level[LEVEL_COLOUR_BLUE].data[MAX_LEVEL_WIDTH/2][MAX_LEVEL_HEIGHT/4-1]=255;	
	
	level[LEVEL_COLOUR_RED].data[MAX_LEVEL_WIDTH/2+1][MAX_LEVEL_HEIGHT/4]=170;
	level[LEVEL_COLOUR_GREEN].data[MAX_LEVEL_WIDTH/2+1][MAX_LEVEL_HEIGHT/4]=170;
	level[LEVEL_COLOUR_BLUE].data[MAX_LEVEL_WIDTH/2+1][MAX_LEVEL_HEIGHT/4]=255;
	level[LEVEL_COLOUR_RED].data[MAX_LEVEL_WIDTH/2+1][MAX_LEVEL_HEIGHT/4-1]=170;
	level[LEVEL_COLOUR_GREEN].data[MAX_LEVEL_WIDTH/2+1][MAX_LEVEL_HEIGHT/4-1]=170;
	level[LEVEL_COLOUR_BLUE].data[MAX_LEVEL_WIDTH/2+1][MAX_LEVEL_HEIGHT/4-1]=255;
	level[LEVEL_COLOUR_RED].data[MAX_LEVEL_WIDTH/2+1][MAX_LEVEL_HEIGHT/4+1]=170;
	level[LEVEL_COLOUR_GREEN].data[MAX_LEVEL_WIDTH/2+1][MAX_LEVEL_HEIGHT/4+1]=170;
	level[LEVEL_COLOUR_BLUE].data[MAX_LEVEL_WIDTH/2+1][MAX_LEVEL_HEIGHT/4+1]=255;
	
	level[LEVEL_COLOUR_RED].data[MAX_LEVEL_WIDTH/2+2][MAX_LEVEL_HEIGHT/4]=170;
	level[LEVEL_COLOUR_GREEN].data[MAX_LEVEL_WIDTH/2+2][MAX_LEVEL_HEIGHT/4]=170;
	level[LEVEL_COLOUR_BLUE].data[MAX_LEVEL_WIDTH/2+2][MAX_LEVEL_HEIGHT/4]=255;
	level[LEVEL_COLOUR_RED].data[MAX_LEVEL_WIDTH/2+2][MAX_LEVEL_HEIGHT/4-1]=170;
	level[LEVEL_COLOUR_GREEN].data[MAX_LEVEL_WIDTH/2+2][MAX_LEVEL_HEIGHT/4-1]=170;
	level[LEVEL_COLOUR_BLUE].data[MAX_LEVEL_WIDTH/2+2][MAX_LEVEL_HEIGHT/4-1]=255;
	
	level[LEVEL_COLOUR_RED].data[MAX_LEVEL_WIDTH/2+3][MAX_LEVEL_HEIGHT/4]=170;
	level[LEVEL_COLOUR_GREEN].data[MAX_LEVEL_WIDTH/2+3][MAX_LEVEL_HEIGHT/4]=170;
	level[LEVEL_COLOUR_BLUE].data[MAX_LEVEL_WIDTH/2+3][MAX_LEVEL_HEIGHT/4]=255;
}


//this Function sends the screen around the mario to stdout
void draw_screen(struct s_level * level, struct s_player * player){

        uint8_t field[SCREEN_HEIGHT * SCREEN_WIDTH * 3];
	int x, y, pos, fieldpos;
	
	for (y=0; y<SCREEN_HEIGHT; y++){
		for (x=0; x<SCREEN_WIDTH; x++){
			pos = (y * SCREEN_WIDTH + x);
			fieldpos = pos * 3;
			field[fieldpos+0] = level[LEVEL_COLOUR_RED].data[x][y];
			field[fieldpos+1] = level[LEVEL_COLOUR_GREEN].data[x][y];
			field[fieldpos+2] = level[LEVEL_COLOUR_BLUE].data[x][y];
		}
	}
	
	
	pos = (player[0].y * SCREEN_WIDTH + player[0].x);
	fieldpos = pos * 3;
	field[fieldpos+0] = 255;
	field[fieldpos+1] = 0;
	field[fieldpos+2] = 0;

        write(STDOUT_FILENO, field, SCREEN_HEIGHT * SCREEN_WIDTH * 3);


}


//this function read ticks from enywhere and return this
uint32_t read_frame(){
	uint32_t ticks=SDL_GetTicks();
	ticks=ticks;
	return ticks;

}


//this function calculates and execute the gravity
void calc_gravity(struct s_level * level, struct s_player * player){
	
	//fall
	if (!check_mario_stand_wall(level,player) &&
	    player[0].jumping==0 &&
	    player[0].y < MAX_LEVEL_HEIGHT-1){
		player[0].y = player[0].y + player[0].gravity_y;
	}
	
	
	//jump
	if (player[0].jumping>0 &&
	    !check_mario_under_wall(level, player)){
		player[0].y = player[0].y -1;
		player[0].jumping--;
	}
	
	//stop jumping if mario is under wall
	if (check_mario_under_wall(level, player)){
		player[0].jumping=0;
	}

}

//this function calculates and execute the moves
void calc_move(struct s_level * level, struct s_player * player){
	if (!check_mario_move_wall(level,player)){
		player[0].x = player[0].x + player[0].next_x_dir;
		player[0].next_x_dir=0;
	}
	
	
}

//main
int main(int argc, char * argv[]){

	struct s_player * player = malloc(sizeof(struct s_player));
	struct s_level * level = malloc(sizeof(struct s_level)*4);
	//struct s_level * level = malloc(MAX_LEVEL_WIDTH * MAX_LEVEL_HEIGHT + (sizeof(int) * 2) * 4);
    struct controller_handle * ch;
	
	uint32_t framenow, next_parse_keys_time = 0 , next_calc_gravity_time = 0, next_draw_screen_time = 0, next_calc_move_time = 0;

    ch = open_controller(CONTROLLER_TYPE_JOYSTICK);

	load_level(level, 1);
	//draw_screen(level, player);

	//fprintf(stderr, "%dx%d\n",level[0].width ,level[0].height);

	player[0].x=12;
	player[0].y=12;
	player[0].gravity_y=1;

	while(1){
		framenow=read_frame();

		if ( framenow >= next_parse_keys_time + PARSE_KEYS_EVERY){
			next_parse_keys_time=next_parse_keys_time+PARSE_KEYS_EVERY;
			parse_keys(ch, level, player);
		}
		
		if ( framenow >= next_draw_screen_time + DRAW_SCREEN_EVERY){
			next_draw_screen_time=next_draw_screen_time+DRAW_SCREEN_EVERY;
			draw_screen(level, player);
		}
		
		if ( framenow >= next_calc_gravity_time + CALC_GRAVITY_EVERY){
			next_calc_gravity_time=next_calc_gravity_time+CALC_GRAVITY_EVERY;
			calc_gravity(level, player);
		}
		
		if ( framenow >= next_calc_move_time + CALC_MOVE_EVERY){
			next_calc_move_time=next_calc_move_time+CALC_MOVE_EVERY;
			calc_move(level, player);
		}
		//fprintf(stderr, "%d\n",read_frame());
		//fprintf(stderr, "0:1-Green = %d\n",level[LEVEL_COLOUR_GREEN].data[0][1]);


		//usleep(100);

	}

    close_controller(ch);

	return 0;

}
