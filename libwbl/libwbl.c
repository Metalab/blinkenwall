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

    ctx = malloc(sizeof(BwlSocketContext));
    if (!ctx) {
        fprintf(stderr, "Malloc failed!.\n");
        return NULL;
    }

    listensocket = socket(AF_INET, SOCK_STREAM, 0);
    if (listensocket < 0) {
        fprintf(stderr, "Create socket failed.\n");
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
        fprintf(stderr, "Bind failed.\n");
        close(listensocket);
        free(ctx);
        return NULL;
    }

    result = listen(listensocket, 1);
    if (result < 0) {
        fprintf(stderr, "Listen failed.\n");
        close(listensocket);
        free(ctx);
        return NULL;
    }

    ctx->fd_listen = listensocket;
    ctx->fd_accept = -1;
    ctx->connected = 0;

    return ctx;
}

BwlSocketContext * bw_socket_open()
{
    return bw_socket_open_port(BW_DEFAULTPORT);
}

void bw_socket_close(BwlSocketContext * socket_context)
{
    if (socket_context->fd_listen >= 0)
        close(socket_context->fd_listen);

    if (socket_context->fd_accept >= 0)
        close(socket_context->fd_accept);

    free(socket_context);
}

int bw_wait_for_connections_timeout(BwlSocketContext * sc,
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
        fprintf(stderr, "Accept failed.\n");
        return -1;
    }

    sc->fd_accept = clientsocket;

    nullhandshake(&hs);

    while(frame_type == WS_INCOMPLETE_FRAME) {
        int read = recv(clientsocket,
                        sc->buffer+num_read,
                        BW_READ_BUF_SIZE, 0);

        if (!read) {
            fprintf(stderr, "Recv failed, closing.");
            return -1;
        }
        num_read += read;

        frame_type = ws_parse_handshake(sc->buffer,
                                num_read, &hs);

        if (frame_type == WS_INCOMPLETE_FRAME &&
            num_read == BW_READ_BUF_SIZE) {
            fprintf(stderr, "Buffer too small\n");
            return -1;
        } else if (frame_type == WS_ERROR_FRAME) {
            fprintf(stderr, "Error in incoming frame\n");
            return -1;
        }
    }

    if (strcmp(hs.resource, BW_CTRL_RESOURCE) != 0) {
        fprintf(stderr, "Resource is wrong:%s\n", hs.resource);
        return -1;
    }

    ws_get_handshake_answer(&hs, sc->buffer, &out_len);
    written = send(clientsocket, sc->buffer,
                   out_len, 0);

    if (written <= 0) {
        fprintf(stderr, "Send failed.\n");
        return -1;
    }
    if (written != out_len) {
        fprintf(stderr, "Written %d of %d\n",
                written, (int)out_len);
        return -1;
    }

    sc->connected = 1;

    return 0;
}

int bw_wait_for_connections(BwlSocketContext * sc)
{
    return bw_wait_for_connections_timeout(sc,
                                           BW_DEFAULTTIMEOUT);
}

int bw_get_cmd_block(BwlSocketContext * sc, char ** uuid)
{
    size_t num_read = 0;
    enum ws_frame_type frame_type = WS_INCOMPLETE_FRAME;
    size_t data_len;
    size_t out_len = BW_READ_BUF_SIZE;
    uint8_t *data;

    if (!sc->connected)
        return BW_CMD_NONE;

    while (frame_type == WS_INCOMPLETE_FRAME) {
        int read;

        read = recv(sc->fd_accept, sc->buffer+num_read,
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
            fprintf(stderr, "Buffer too small\n");
            return BW_CMD_NONE;
        } else if (frame_type == WS_CLOSING_FRAME) {
            send(sc->fd_accept, "\xFF\x00", 2, 0);
            return BW_CMD_DISCONNECT;
        } else if (frame_type == WS_ERROR_FRAME) {
            fprintf(stderr, "Error in incoming frame\n");
            return BW_CMD_NONE;
        } else if (frame_type == WS_TEXT_FRAME) {
            out_len = BW_READ_BUF_SIZE;
            char * pch;
            uint8_t cmd;

            frame_type = ws_make_frame(data, data_len, sc->buffer,
                                       &out_len, WS_TEXT_FRAME);
            if (frame_type != WS_TEXT_FRAME) {
                fprintf(stderr, "Make frame failed\n");
                return BW_CMD_NONE;
            }

            sc->buffer[out_len-1] = '\0';

            if (strncmp((char*)sc->buffer, "key", 3) == 0) {
                fprintf(stderr, "Invalid command\n");
                return BW_CMD_NONE;
            }

            strtok((char*)sc->buffer+1, " ");
            pch = strtok(NULL, " ");
            if (uuid) {
                *uuid = pch;
            }
            pch = strtok(NULL, " ");

            if (!pch) {
                fprintf(stderr, "Error command\n");
                return BW_CMD_NONE;
            }

            cmd = (uint8_t)strtoul(pch, NULL, 16);

            if (cmd <= BW_CMD_NONE || cmd >= BW_CMD_LAST)
            {
                fprintf(stderr, "Invalid key\n");
                return BW_CMD_NONE;
            }

            return cmd;
        }
    }

    return BW_CMD_NONE;
}

int bw_get_cmd_block_timeout(BwlSocketContext * sc,
                             char ** uuid,
                             int timeout)
{
    fd_set recvfds;
    struct timeval tv;
    int result;

    FD_ZERO(&recvfds);
    FD_SET(sc->fd_accept, &recvfds);

    tv.tv_sec = timeout / 1000;
    tv.tv_usec = (timeout % 1000) * 1000;

    result = select(sc->fd_accept+1,
                    &recvfds, NULL, NULL, &tv);

    if (result <= 0)
        return BW_CMD_NONE;

    return bw_get_cmd_block(sc, uuid);
}
