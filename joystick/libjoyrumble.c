/*
 * *
 * * libjoyrumble.c
 * *
 * * version 0.1.3
 * *
 * * by Cleber de Mattos Casali <clebercasali@yahoo.com.br>
 * *
 * *
 * * This program is distributed under the terms of the GNU LGPL license:
 * *
 * * http://www.gnu.org/copyleft/lesser.html
 * *
 * * This program is distributed in the hope that it will be useful,
 * * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * * See the GNU Lesser General Public License for more details.
 * *
 * * The purpose of this program is to provide game / emulator programmers
 * * an extremely simple way to access the joystick "rumble" feature.
 * *
 * * Only simple joystick / joypad rumble effect is suported. There are no
 * * plans to support any advanced force feedback effects.
 * *
 * * As of now it's only for Linux. There are plans for a Windows version
 * * in the future.
 * *
 * * This program is loosely inspired by "rumble.c" written by Stephen M.Cameron,
 * * wich is a modified version of the "fftest.c" program from the "joystick"
 * * package, written by Johan Deneux.
 * *
 * * You are free to use the library in your free or commercial games.
 * * Please give me some credit if you do.
 * *
 * */
 
/*
 * *
 * * To create the shared library (libjoyrumble.so), compile it with:
 * *
 * * gcc -pthread  -c -fPIC libjoyrumble.c -o libjoyrumble.o    
 * * gcc -pthread  -shared -Wl,-soname,libjoyrumble.so -o libjoyrumble.so  libjoyrumble.o
 * *
 * *
 * * To create the command-line tool (joyrumble), compile it with:
 * *
 * * gcc -pthread libjoyrumble.c -o joyrumble
 * *
 * * If you're running on x86_64 (64 bits), and want to compile the library for x86 (32 bits), add the "-m32" flag to gcc.
 * *
 * */

/* Uncomment this if you want to add debug messages. */

#define DEBUG 1


/*  Includes */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <dirent.h>
#include <fcntl.h>
#include <pthread.h>
#include <sys/ioctl.h>
#include <linux/input.h>
#include <errno.h>

/* test_bit  : Courtesy of Johan Deneux */
#define BITS_PER_LONG (sizeof(long) * 8)
#define OFF(x)  ((x)%BITS_PER_LONG)
#define BIT(x)  (1UL<<OFF(x))
#define LONG(x) ((x)/BITS_PER_LONG)
#define test_bit(bit, array)    ((array[LONG(bit)] >> OFF(bit)) & 1)


/* 8 joypads should be enough for everyone */
#define MAXJOY 8


/* Shared variables */
static unsigned long features[4];
static struct ff_effect effects[MAXJOY];
static struct input_event stop[MAXJOY];
static struct input_event play[MAXJOY];
static char *eventfile[MAXJOY];
static int hasrumble[MAXJOY];
static int int_joynumber;
static int int_strong[MAXJOY];
static int int_weak[MAXJOY];
static int int_duration[MAXJOY];
static int mtlock=0;
static int initialized=0;
static int lastjoy=-1;


/* Check if a file exists */
int file_exists(char *filename)
{
	        FILE *i;
		  i = fopen(filename, "r");

		  if (i == NULL)
			    {
				        return 0;
					  }
		   
		    fclose(i);
		      return 1;
		       
}


/* Check if a directory exists */
int dir_exists(char *filename)
{
	        DIR *i ;
		        i =opendir(filename);
			       
			        if (i == NULL)
					        {
							        return 0;
								        }
				       
				        closedir(i);
					        return 1;
}



/* Initialization. This should run automatically. */
void __attribute__ ((constructor)) joyrumble_init(void)
{
	       
	        #ifdef DEBUG
	                printf("INIT\n");
			        #endif
			       
			        /* event_fd is the handle of the event file */
			        int event_fd;
				       
				        int count=0, j=0;
					       
					        /* this will hold the path of files while we do some checks */
					        char tmp[128];
						       
						        for (count = 0; count < MAXJOY; count++)
								        {
										               
										                sprintf(tmp,"/dev/input/js%d",count);
												                /* Check if joystick device exists */
												                if (file_exists(tmp)==1){
															                        lastjoy=count;  
																		                        for (j = 0; j <= 99; j++)
																						                        {
																										                        /* Try to discover the corresponding event number */
																										                        sprintf(tmp,"/sys/class/input/js%d/device/event%d",count,j);
																													                        if (dir_exists(tmp)==1){
																																	                                /* Store the full path of the event file in the eventfile[] array */
																																	                                sprintf(tmp,"/dev/input/event%d",j);
																																					                                eventfile[count]=(char *)calloc(sizeof(tmp),sizeof(char));
																																									                                sprintf(eventfile[count], "%s", tmp);
																																													                                #ifdef DEBUG
																																													                                        printf("%s", eventfile[count]);
																																																		                                        printf("\n");
																																																							                                #endif
																																																							                                }
																																                        }
																					               
																					                } // else printf("Joystick does not exist!");
														               
														        }
							       
							        #ifdef DEBUG
							                printf("lastjoy=%d\n",lastjoy);
									        #endif

									        for (count = 0; count <= lastjoy ; count++)
											        {
													       
													        hasrumble[count]=0;
														               
														        /* Prepare the effects */
														        effects[count].type = FF_RUMBLE;
															        effects[count].u.rumble.strong_magnitude = 65535;
																        effects[count].u.rumble.weak_magnitude = 65535;
																	        effects[count].replay.length = 1000;
																		        effects[count].replay.delay = 0;
																			        effects[count].id = -1;

																				        /* Prepare the stop event */
																				        stop[count].type = EV_FF;
																					        stop[count].code = effects[count].id;
																						        stop[count].value = 0;
																							               
																							        /* Prepare the play event */
																							        play[count].type = EV_FF;
																								        play[count].code = effects[count].id;
																									        play[count].value = 1;

																										               
																										        /* Open event file to verify if the device and the drivers support rumble */
																										        event_fd = open(eventfile[count], O_RDWR);
																											       
																											               
																											        if (event_fd < 0)
																													               
																													                /* Can't acess the event file */
																													                hasrumble[count]=0;
																												       
																												        else{

																														                if (ioctl(event_fd, EVIOCGBIT(EV_FF, sizeof(unsigned long) * 4), features) == -1)
																																	                        /* This device can't rumble, or the drivers don't support it */
																																	                        hasrumble[count]=0;
																																               
																																                else{
																																			               
																																			                        if (test_bit(FF_RUMBLE, features))
																																							                                /* Success! This device can rumble! */
																																							                                hasrumble[count]=1;
																																						                }
																																		               
																																		        /* Close the event file */
																																		        if (event_fd>0)        
																																				                close(event_fd);
																																			       
																																			        }

																													        #ifdef DEBUG
																													                printf("%s", eventfile[count]);
																															       
																															                if (hasrumble[count]){
																																		                        printf(" can rumble.\n");
																																					                }
																																	                else{
																																				                        printf(" can't rumble.\n");
																																							                }
																																			        #endif
																																			       
																																			        }
										       
										        /* Initialization is complete */
										        initialized=1;
}



/* This will run internally, as a thread */
void *joyrumble_internal(void *arg)
{

	        /* Lock our thread to make it safe for multithreading */
	        mtlock=1;

		        int joy=int_joynumber-1;
			        int event_fd;
				       
				        #ifdef DEBUG
				                printf("1\n");
						        #endif
						       
						        #ifdef DEBUG
						                printf("%s", eventfile[joy]);
								                printf("\n");
										                printf("Joy#:%d\n",joy);
												                printf("Strong:%d\n",int_strong[joy]);
														                printf("Weak:%d\n",int_weak[joy]);
																                printf("Duration:%d\n",int_duration[joy]);
																		        #endif
																		       
																		        /* Open the event file */
																		        event_fd = open(eventfile[joy], O_RDWR);


																			        /* Stop the effect if it's playing */
																			        stop[joy].code =  effects[joy].id;
																				       
																				        if (write(event_fd, (const void*) &stop[joy], sizeof(stop[joy])) == -1){
																						        #ifdef DEBUG
																						                printf("error stopping effect");
																								        #endif
																								        }
																					       
																					       
																					        /* Modify effect data to create a new effect */
																					        effects[joy].u.rumble.strong_magnitude = int_strong[joy]*65535/100;
																						        effects[joy].u.rumble.weak_magnitude = int_weak[joy]*65535/100;
																							        effects[joy].replay.length = int_duration[joy];
																								        effects[joy].id = -1;   /* ID must be set to -1 for every new effect */

																									        /* Send the effect to the driver */
																									        if (ioctl(event_fd, EVIOCSFF, &effects[joy]) == -1) {
																											                #ifdef DEBUG
																											                        fprintf(stderr, "%s: failed to upload effect: %d\n",
																																                                eventfile[joy], strerror(errno));
																														                #endif
																														        }
																										       
																										        /* Play the effect */
																										        play[joy].code = effects[joy].id;
																											       
																											        if (write(event_fd, (const void*) &play[joy], sizeof(play[joy])) == -1){
																													                #ifdef DEBUG
																													                        printf("error playing effect");
																																                #endif
																																        }
																												       
																												        /* It should be safe to unlock our thread now */
																												        mtlock=0;
																													       
																													        /* Sleep until the effect finishes playing */
																													        /* Don't worry. This is done in a separate thread and will not pause your program. */
																													        usleep((int_duration[joy]+50)*1000);

																														        #ifdef DEBUG
																														                printf("Done.\n");
																																        #endif

																																        /* All done. Close the event file. */
																																        close(event_fd);

}


/* joyrumble ( joystick number (1-8), strong motor intensity (0-100), weak motor intensity (0-100), duration in miliseconds ) */
/* This is the only function that needs to be visible externally. */
/* Notice the joystick number starts from 1. */
/* 1=/dev/input/js0 , 2=/dev/input/js1 , and so on. */
extern int joyrumble(int joynumber, int strong, int weak, int duration)
{
	       
	        int joy=joynumber-1;
		       
		        /* Copy the arguments to the shared variables, so our thread can access them. */
		        int_strong[joy]=strong;
			        int_weak[joy]=weak;
				        int_duration[joy]=duration;

					       
					        #ifdef DEBUG
					                printf("init=%d\n",initialized);
							        #endif
							       
							        /* Check if the program is initialized */
							        if (initialized==0) {
									                #ifdef DEBUG
									                        printf("go init!\n");
												                #endif
												                joyrumble_init();
														        }

								       
								        /* If this device doesn't support rumble, just quit */
								        if (hasrumble[joy]==0){
										                #ifdef DEBUG
										                        printf("NO RUMBLE");
													                #endif
													                return -1;
															        }
									       
									        /* Sleep while our thread is locked */
									        do {
											                usleep(1000);
													        } while (mtlock!=0);

										        int_joynumber=joy+1;
										       
										        /* Launch the thread */
										        pthread_t tid;
											        if ( pthread_create( &tid, NULL, joyrumble_internal, NULL) != 0 ){
													                #ifdef DEBUG
													                        printf("error: thread not created\n");
																                #endif
																                return -2;
																		        }
												       
												       
}


/* The "joyrumble" command-line tool */
int main(int argc, char *argv[]){
	       
	  if (argc < 5)
		      {
			                printf("JOYRUMBLE\nUsage: joyrumble [ joystick (1-8) ]  [strong motor magnitude (%%) ]  [weak motor magnitude (%%) ]  [duration (miliseconds) ]\n");
					        return 0;
						    }

	   
	      int arg_joy=atoi(argv[1]);
	          short arg_strong=atoi(argv[2]);
		      short arg_weak=atoi(argv[3]);
		          int arg_time=atoi(argv[4]);
			     
			          if (arg_joy < 1)
					                  arg_joy=1;

				          if (arg_joy > MAXJOY)
						                  arg_joy=MAXJOY;
					         
					          if (arg_time < 0)
							                  arg_time=0;
						         
						          if (arg_weak < 0)
								                  arg_weak=0;
							         
							          if (arg_weak > 100)
									                  arg_weak=100;
								         
								          if (arg_strong < 0)
										                  arg_strong=0;
									         
									          if (arg_strong > 100)
											                  arg_strong=100;


										          joyrumble ( arg_joy, arg_strong, arg_weak, arg_time);
											         
											          usleep((arg_time+100)*1000);
												         
												         
												          return 0;
													         
}

