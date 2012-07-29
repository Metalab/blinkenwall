/* Metalab Blinkenwall websocket listener
 * Listens on TCP/IP port, receives commands using
 * the blinkenwall control protocol.
 * 
 * Code by Georg <georg.lippitsch@gmx.at> and Wolfgang <WSyS@gmx.at>
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <netinet/in.h>
#include "websocket.h"
#include "libwbl.h"

BwlSocketContext * bw_socket_open_port(int port)
{
    struct sockaddr_in local;
    int listensocket;
    int result;
    BwlSocketContext * ctx;
    int on;
    int i;

    ctx = malloc(sizeof(BwlSocketContext));
    if (!ctx) {
        fprintf(stderr, "[libwbl] Malloc failed!.\n");
        return NULL;
    }

    listensocket = socket(AF_INET, SOCK_STREAM, 0);
    if (listensocket < 0) {
        fprintf(stderr, "[libwbl] Create socket failed.\n");
        free(ctx);
        return NULL;
    }

    on = 1;
    setsockopt(listensocket, SOL_SOCKET,
               SO_REUSEADDR, &on, sizeof(on));

    local.sin_family = AF_INET;
    local.sin_addr.s_addr = INADDR_ANY;
    local.sin_port = htons(port);
    result = bind(listensocket,
                  (struct sockaddr*)&local,
                  sizeof(local));

    if (result < 0) {
        fprintf(stderr, "[libwbl] Bind failed.\n");
        close(listensocket);
        free(ctx);
        return NULL;
    }

    result = listen(listensocket, 1);
    if (result < 0) {
        fprintf(stderr, "[libwbl] Listen failed.\n");
        close(listensocket);
        free(ctx);
        return NULL;
    }

    ctx->fd_listen = listensocket;
    for(i=0; i<BW_MAX_CONNECTIONS; ++i)
        ctx->fd_client[0] = -1;
    ctx->num_connections = 0;

    return ctx;
}

BwlSocketContext * bw_socket_open()
{
    return bw_socket_open_port(BW_DEFAULTPORT);
}

void bw_socket_close(BwlSocketContext * sc)
{
    int i;

    for(i=0; i<BW_MAX_CONNECTIONS; ++i) {
        if (sc->fd_client[i] >= 0) {
            close(sc->fd_client[i]);
        }
    }

    if (sc->fd_listen >= 0)
        close(sc->fd_listen);

    free(sc);
}

int bw_wait_for_connections_timeout(BwlSocketContext * sc,
                                    char ** resource,
                                    int timeout) {
    fd_set acceptfds;
    struct timeval tv;
    int clientsocket;
    int result;
    struct sockaddr_in remote;
    unsigned int sockaddr_len;
    struct handshake hs;
    enum ws_frame_type frame_type = WS_INCOMPLETE_FRAME;
    size_t num_read = 0;
    size_t out_len = BW_READ_BUF_SIZE;
    int written = 0;
    int i;

    if (sc->fd_listen < 0) {
        fprintf(stderr, "[libwbl] Listen socket not opened.\n");
        return -1;
    }

    if (sc->num_connections >= BW_MAX_CONNECTIONS) {
        fprintf(stderr, "[libwbl] Maximum number of connections reached.\n");
        return -1;
    }

    FD_ZERO(&acceptfds);
    FD_SET(sc->fd_listen, &acceptfds);

    tv.tv_sec = timeout;
    tv.tv_usec = 0;

    result = select(sc->fd_listen+1,
                    &acceptfds, NULL, NULL, &tv);

    if (result <= 0)
        return -1;

    sockaddr_len = sizeof(remote);
    clientsocket = accept(sc->fd_listen,
                          (struct sockaddr*)&remote,
                          &sockaddr_len);

    if (clientsocket < 0) {
        fprintf(stderr, "[libwbl] Accept failed.\n");
        return -1;
    }

    nullhandshake(&hs);

    while(frame_type == WS_INCOMPLETE_FRAME) {
        int read = recv(clientsocket,
                        sc->buffer+num_read,
                        BW_READ_BUF_SIZE, 0);

        if (!read) {
            fprintf(stderr, "[libwbl] Recv failed.");
            return -1;
        }
        num_read += read;

        frame_type = ws_parse_handshake(sc->buffer,
                                num_read, &hs);

        if (frame_type == WS_INCOMPLETE_FRAME &&
            num_read == BW_READ_BUF_SIZE) {
            fprintf(stderr, "[libwbl] Buffer too small\n");
            return -1;
        } else if (frame_type == WS_ERROR_FRAME) {
            fprintf(stderr, "[libwbl] Error in incoming frame\n");
            return -1;
        }
    }

    if (resource) {
        int res_len = strlen(hs.resource) + 1;
        *resource = malloc(res_len);
        strcpy(*resource, hs.resource);
    }

    ws_get_handshake_answer(&hs, sc->buffer, &out_len);
    written = send(clientsocket, sc->buffer,
                   out_len, 0);

    if (written <= 0) {
        fprintf(stderr, "[libwbl] Send failed.\n");
        return -1;
    }
    if (written != out_len) {
        fprintf(stderr, "[libwbl] Written %d of %d\n",
                written, (int)out_len);
        return -1;
    }

    for (i=0; i<BW_MAX_CONNECTIONS; ++i) {
        if (sc->fd_client[i] < 0) {
            sc->fd_client[i] = clientsocket;
            sc->num_connections++;
            return i;
        }
    }

    return -1;
}

int bw_wait_for_connections(BwlSocketContext * sc,
                            char ** resource)
{
    return bw_wait_for_connections_timeout(sc, resource, 
                                           BW_DEFAULTTIMEOUT);
}

void bw_connection_close(BwlSocketContext * sc,
                         int connection_num)
{
    if (connection_num < 0 || connection_num >= BW_MAX_CONNECTIONS) {
        fprintf(stderr, "[libwbl] connection_num %d outside range\n",
                connection_num);
        return;
    }

    if (sc->fd_client[connection_num] < 0) {
        fprintf(stderr, "[libwbl] connection_num %d not open\n",
                connection_num);
        return;
    }

    close(sc->fd_client[connection_num]);
    sc->fd_client[connection_num] = -1;
}

int bw_get_cmd_block_timeout(BwlSocketContext * sc,
                             int * connection,
                             char ** uuid,
                             int timeout)
{
    size_t num_read = 0;
    enum ws_frame_type frame_type = WS_INCOMPLETE_FRAME;
    size_t data_len;
    size_t out_len = BW_READ_BUF_SIZE;
    uint8_t *data;
    fd_set recvfds;
    struct timeval tv;
    struct timeval * tv2;
    int result;
    int fd_max = 0;
    int i;

    if (sc->num_connections <= 0)
        return BW_CMD_NONE;

    FD_ZERO(&recvfds);
    for (i=0; i<BW_MAX_CONNECTIONS; ++i) {
        if (sc->fd_client[i] >= 0) {
            FD_SET(sc->fd_client[i], &recvfds);
            if (sc->fd_client[i] > fd_max)
                fd_max = sc->fd_client[i];
        }
    }

    if (timeout >= 0) {
        tv.tv_sec = timeout / 1000;
        tv.tv_usec = (timeout % 1000) * 1000;
        tv2 = &tv;
    } else {
        tv2 = NULL;
    }

    result = select(fd_max+1, &recvfds, NULL, NULL, tv2);

    if (result <= 0)
        return BW_CMD_NONE;

    for (i=0; i<BW_MAX_CONNECTIONS; ++i) {
        if (FD_ISSET(sc->fd_client[i], &recvfds)) {
            while (frame_type == WS_INCOMPLETE_FRAME) {
                int read;

                read = recv(sc->fd_client[i], sc->buffer+num_read,
                            BW_READ_BUF_SIZE-num_read, 0);
                if (read <= 0) {
                    return BW_CMD_DISCONNECT;
                }
                num_read += read;

                frame_type = ws_parse_input_frame(sc->buffer,
                                                  num_read,
                                                  &data, &data_len);

                if (frame_type == WS_INCOMPLETE_FRAME &&
                    num_read == BW_READ_BUF_SIZE) {
                    fprintf(stderr, "[libwbl] Buffer too small\n");
                    return BW_CMD_NONE;
                } else if (frame_type == WS_CLOSING_FRAME) {
                    send(sc->fd_client[i], "\xFF\x00", 2, 0);
                    return BW_CMD_DISCONNECT;
                } else if (frame_type == WS_ERROR_FRAME) {
                    fprintf(stderr, "[libwbl] Error in incoming frame\n");
                    return BW_CMD_NONE;
                } else if (frame_type == WS_TEXT_FRAME) {
                    out_len = BW_READ_BUF_SIZE;
                    char * pch;
                    uint8_t cmd;

                    frame_type = ws_make_frame(data, data_len, sc->buffer,
                                               &out_len, WS_TEXT_FRAME);
                    if (frame_type != WS_TEXT_FRAME) {
                        fprintf(stderr, "[libwbl] Make frame failed\n");
                        return BW_CMD_NONE;
                    }

                    sc->buffer[out_len-1] = '\0';

                    if (strncmp((char*)sc->buffer, "key", 3) == 0) {
                        fprintf(stderr, "[libwbl] Invalid command\n");
                        return BW_CMD_NONE;
                    }

                    strtok((char*)sc->buffer+1, " ");
                    pch = strtok(NULL, " ");
                    if (uuid) {
                        *uuid = pch;
                    }
                    pch = strtok(NULL, " ");

                    if (!pch) {
                        fprintf(stderr, "[libwbl] Error command\n");
                        return BW_CMD_NONE;
                    }

                    cmd = (uint8_t)strtoul(pch, NULL, 16);

                    if (cmd <= BW_CMD_NONE || cmd >= BW_CMD_LAST)
                    {
                        fprintf(stderr, "[libwbl] Invalid key\n");
                        return BW_CMD_NONE;
                    }

                    if (connection)
                        *connection = i;

                    return cmd;
                }
            }
        }
    }

    return BW_CMD_NONE;
}

int bw_get_cmd_block(BwlSocketContext * sc,
                             int * connection, 
                             char ** uuid)
{
    return bw_get_cmd_block_timeout(sc, connection, uuid, -1);
}
