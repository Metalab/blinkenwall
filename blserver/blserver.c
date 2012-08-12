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
#define BW_RUN_BEFORE_CONNECT "/usr/local/bin/blinkyrun start"
#define BW_RUN_AFTER_CONNECT "/usr/local/bin/blinkyrun stop"

#define BW_DEBUG 1

#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
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
#ifdef BW_DEBUG
        fprintf(stderr, "Waiting for connections\n");
#endif
        system(BW_RUN_BEFORE_CONNECT);
        con = bw_wait_for_connections(sc, &resource);
        system(BW_RUN_AFTER_CONNECT);
        if (con >= 0) {
#ifdef BW_DEBUG
            fprintf(stderr, "New connection\n");
#endif
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

#ifdef BW_DEBUG
            fprintf(stderr, "Resource: %s\n", resource);
#endif
            
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

            while(1) {
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

#ifdef BW_DEBUG
                fprintf(stderr, "select result: %d\n", result);
#endif

                if (result <= 0) {
#ifdef BW_DEBUG
                    fprintf(stderr, "Closing connection\n");
#endif
                    break;
                }

                /** Perform reads / writes */

                /** Send commands from command queue to child process */

                if (FD_ISSET(fd_write, &writefds)) {
                    if (cmds_queued > 0) {
                        char * cmdstr =
                            cmd_queue[cmdq_read++ % BW_CMD_QUEUE_SIZE];
#ifdef BW_DEBUG
                        fprintf(stderr, "Send command to child: %s\n", cmdstr);
#endif

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
                        sprintf(cmdstr, "%c %d %s\n", (char)cmd,
                                current_connection, uuid);
   
                        cmd_queue[cmdq_write++ % BW_CMD_QUEUE_SIZE] = cmdstr;
                        cmds_queued++;
                    }
                }

                /** Read data from child process,
                 *  send to wall and to client */

                if (FD_ISSET(fd_read, &readfds)) {
                    uint8_t rgb_data[BW_WALL_SIZE * 3];
#ifdef BW_DEBUG
                    fprintf(stderr, "Got data for wall.\n");
#endif
                    if (read(fd_read, rgb_data, BW_WALL_SIZE * 3) ==
                        BW_WALL_SIZE * 3) {
                        uint64_t cmddata = (uint64_t)rgb_data[0] | 
                            (uint64_t)rgb_data[1] << 8 |
                            (uint64_t)rgb_data[2] << 16 |
                            (uint64_t)rgb_data[3] << 24 |
                            (uint64_t)rgb_data[4] << 32 |
                            (uint64_t)rgb_data[5] << 40 |
                            (uint64_t)rgb_data[6] << 48 |
                            (uint64_t)rgb_data[7] << 56;
                        if (cmddata != bw_sendback_prefix) {
                            write(fd_bwpipe, rgb_data, BW_WALL_SIZE * 3);
                        } else {
#ifdef BW_DEBUG
                            fprintf(stderr, "Sending command back to client: %s\n",
                                    (char*)(rgb_data+8));
#endif
                            bw_send_back(sc, 0, (char*)(rgb_data+8));
                        }
                    } else {
#ifdef BW_DEBUG
                        fprintf(stderr, "Input pipe closed\n");
#endif
                        break;
                    }
                }
            }

            close(fd_bwpipe);

            if (fd_write >= 0)
                close(fd_write);
            if (fd_read >= 0)
                close(fd_read);

            for (i=0; i<BW_MAX_CONNECTIONS; ++i) {
                if (sc->fd_client[i] >= 0) {
                    bw_connection_close(sc, i);
                }
            }

            waitpid(pid, NULL, 0);
        }
    }

    bw_socket_close(sc);

    return 0;
}
