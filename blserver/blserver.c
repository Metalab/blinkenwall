/* Metalab Blinkenwall websocket server
 * Listens on websocket, opens a program specified by resource,
 * pipes commands to stdin and the program's output to
 * the blinkenwall
 * 
 * Code by Georg <georg.lippitsch@gmx.at> and Wolfgang <wsys@gmx.at>
 */

#define BW_PIPE_PATH "/var/blinkenwall/bw_pipe"
#define BW_BINARY_PATH "/usr/local/lib/blinkenwall"
#define BW_CMD_QUEUE_SIZE 10

#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <assert.h>
#include "../libwbl/libwbl.h"
#include "../blink/blink.h"

int main(int argc, char * argv[])
{
    BwlSocketContext * sc;
    char * resource;
    int i;

    /** Open socket and listen for incoming connections */

    sc = bw_socket_open();

    if (!sc) {
        fprintf(stderr, "Error opening socket\n");
        return 1;
    }

    /** Wait for first connection */

    while (1) {
        int con;
        con = bw_wait_for_connections(sc, &resource);
        if (con >= 0) {
            int cmd;
            char * filename;
            int fstr_len;
            struct stat statbuf;
            int fd1[2];
            int fd2[2];
            int fd_read;
            int fd_write;
            pid_t pid;

            char * uuid;
            int current_connection;

            fd_set readfds;
            fd_set writefds;
            int fd_max = 0;
            struct timeval tv;

            char * cmd_queue[BW_CMD_QUEUE_SIZE];
            int cmdq_read = 0;
            int cmdq_write = 0;
            int cmds_queued = 0;

            int result;

            int fd_bwpipe;

            if (!resource || strlen(resource) == 0) {
            fail:
                free(resource);
                bw_connection_close(sc, con);
                continue;
            }
            
            /** Execute binary specified by resource */

            fstr_len = strlen(BW_BINARY_PATH) + strlen(resource) + 2;
            filename = malloc(fstr_len);
            strcpy(filename, BW_BINARY_PATH);
            strcat(filename, "/");
            strcat(filename, resource);
            if (lstat(filename, &statbuf) != 0)
                goto fail;
            if (!(statbuf.st_mode & S_IXUSR))
                goto fail;

            pipe(fd1);
            pipe(fd2);

            pid = fork();

            if (pid == 0) { // Child process
                close(fd1[1]);
                close(fd2[0]);
                dup2(fd1[0], STDIN_FILENO);
                dup2(fd2[1], STDOUT_FILENO);
                close(fd1[0]);
                close(fd2[1]);
                execl(filename, resource, (char*)0);
            }
            free(filename);
            free(resource);
            close(fd1[0]);
            close(fd2[1]);
            fd_read = fd2[0];
            fd_write = fd1[1];

            fd_bwpipe = open(BW_PIPE_PATH, O_WRONLY);

            for(;;) {
                int cmd_avail = 0;
                char * resource2;
                int new_connection;

                /** Poll for new connection */

                if ((new_connection =
                     bw_wait_for_connections_timeout(sc, &resource2, 0)) >= 0) {
                    if (strcmp(resource, resource2) != 0) {
                        bw_connection_close(sc, new_connection);
                    }
                }

                /** Look if there is something to read or write */

                FD_ZERO(&readfds);
                FD_ZERO(&writefds);

                FD_SET(fd_read, &readfds);
                    fd_max = fd_read;
                for (i=0; i<BW_MAX_CONNECTIONS; ++i) {
                    if (sc->fd_client[i] >= 0) {
                        FD_SET(sc->fd_client[i], &readfds);
                        if (sc->fd_client[i] > fd_max)
                            fd_max = sc->fd_client[i];
                    }
                }
                if (cmds_queued > 0) {
                    FD_SET(fd_write, &writefds);
                    if (fd_write > fd_max)
                        fd_max = fd_write;
                }

                tv.tv_sec = BW_DEFAULTTIMEOUT;
                tv.tv_usec = 0;

                result = select(fd_max+1, &readfds, &writefds, NULL, &tv);

                if (result <= 0) {
                    for (i=0; i<BW_MAX_CONNECTIONS; ++i) {
                        if (sc->fd_client[i] >= 0) {
                            bw_connection_close(sc, i);
                        }
                    }
                    break;
                }

                /** Perform reads / writes */

                /** Send commands from command queue to child process */

                if (FD_ISSET(fd_write, &writefds)) {
                    if (cmds_queued > 0) {
                        char * cmdstr =
                            cmd_queue[cmdq_read++ % BW_CMD_QUEUE_SIZE];
                        write(fd_write, cmdstr, strlen(cmdstr));
                        free(cmdstr);
                        cmds_queued--;
                    }
                }

                /** Read command from network and put into queue */

                for (i=0; i<BW_MAX_CONNECTIONS; ++i) {
                    if (FD_ISSET(sc->fd_client[i], &readfds)) {
                        cmd_avail = 1;
                        break;
                    }
                }
                if (cmd_avail) {
                    char * cmdstr;
                    
                    cmd = bw_get_cmd_block(sc, &current_connection, &uuid);

                    if (cmd == BW_CMD_DISCONNECT) {
                        bw_connection_close(sc, current_connection);
                        continue;
                    }

                    assert(current_connection >= 0 &&
                           current_connection < 10);
                    
                    if (cmds_queued < BW_CMD_QUEUE_SIZE) {
                        cmdstr = malloc(strlen(uuid) + 5);
                        sprintf(cmdstr, "%c %d %s", (char)cmd,
                                current_connection, uuid);
   
                        cmd_queue[cmdq_write++ % BW_CMD_QUEUE_SIZE] = cmdstr;
                        cmds_queued++;
                    }
                }

                /** Send date from child process to wall */

                if (FD_ISSET(fd_read, &readfds)) {
                    uint8_t rgb_data[BW_WALL_SIZE];

                    read(fd_read, rgb_data, BW_WALL_SIZE);
                    write(fd_bwpipe, rgb_data, BW_WALL_SIZE);
                }
            }

            close(fd_bwpipe);

            if (fd_write >= 0)
                close(fd_write);
            if (fd_read >= 0)
                close(fd_read);

            free(resource);
        }
    }

    bw_socket_close(sc);

    return 0;
}
