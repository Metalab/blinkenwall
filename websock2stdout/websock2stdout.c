#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <netinet/in.h>
#include "websocket.h"

#define BW_PORT 15632
#define BW_READ_BUF_SIZE 2048

int client_worker(int clientsocket)
{
    static uint8_t buffer[BW_READ_BUF_SIZE];
    static uint8_t buffer2[BW_READ_BUF_SIZE];
    size_t num_read = 0;
    size_t out_len = BW_READ_BUF_SIZE;
    int written = 0;
    int stdout_len = 0;
    enum ws_frame_type frame_type = WS_INCOMPLETE_FRAME;
    struct handshake hs;
    char * pch;
    size_t data_len;
    uint8_t *data;

    nullhandshake(&hs);

    while(frame_type == WS_INCOMPLETE_FRAME) {
        int read = recv(clientsocket,
                        buffer+num_read,
                        BW_READ_BUF_SIZE, 0);
        if (!read) {
            fprintf(stderr, "Recv failed, closing.");
            return -1;
        }
        num_read += read;

        frame_type = ws_parse_handshake(buffer,
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

    if (strcmp(hs.resource, "/raw") != 0) {
        fprintf(stderr, "Resource is wrong:%s\n", hs.resource);
        return -1;
    }
    
    ws_get_handshake_answer(&hs, buffer, &out_len);
    written = send(clientsocket, buffer, out_len, 0);
    
    if (written <= 0) {
        fprintf(stderr, "Send failed.\n");
        return -1;
    }
    if (written != out_len) {
        fprintf(stderr, "Written %d of %d\n",
                written, (int)out_len);
        return -1;
    }

    // connect success!
    // read incoming packet write them to stdout

    num_read = 0;
    frame_type = WS_INCOMPLETE_FRAME;

    while (frame_type == WS_INCOMPLETE_FRAME) {
        int read = recv(clientsocket, buffer+num_read,
                        BW_READ_BUF_SIZE-num_read, 0);
        if (read <= 0) {
            fprintf(stderr, "Recv failed.\n");
            return -1;
        }
        num_read += read;

        frame_type = ws_parse_input_frame(buffer,
                                          num_read,
                                          &data, &data_len);

        if (frame_type == WS_INCOMPLETE_FRAME &&
            num_read == BW_READ_BUF_SIZE) {
            fprintf(stderr, "Buffer too small\n");
            return -1;
        }
        else if (frame_type == WS_CLOSING_FRAME) {
            send(clientsocket, "\xFF\x00", 2, 0); // send closing frame
            break;
        } else if (frame_type == WS_ERROR_FRAME) {
            fprintf(stderr, "Error in incoming frame\n");
            return -1;
        } else if (frame_type == WS_TEXT_FRAME) {
            out_len = BW_READ_BUF_SIZE;
            frame_type = ws_make_frame(data, data_len, buffer,
                                       &out_len, WS_TEXT_FRAME);
            if (frame_type != WS_TEXT_FRAME) {
                fprintf(stderr, "Make frame failed\n");
                return -1;
            }

            stdout_len = 0;

            buffer[out_len-1] = '\0';
            pch = strtok((char*)buffer+1, " ");
            while (pch != NULL)
            {
                uint8_t byte;
                byte = (uint8_t)strtoul(pch, NULL, 16);

                buffer2[stdout_len++] = byte;
                pch = strtok(NULL, " ");
            }

            write(STDOUT_FILENO, buffer2, stdout_len);

            num_read = 0;
            frame_type = WS_INCOMPLETE_FRAME;
        }
    }

    return 0;
}

int main(int argc, char** argv)
{
    int result;
    int on;
    struct sockaddr_in local;
    int listensocket = socket(AF_INET, SOCK_STREAM, 0);
    on = 1;
    setsockopt(listensocket, SOL_SOCKET,
               SO_REUSEADDR, &on, sizeof(on));

    if (listensocket < 0) {
        fprintf(stderr, "Create socket failed.\n");
        return -1;
    }

    local.sin_family = AF_INET;
    local.sin_addr.s_addr = INADDR_ANY;
    local.sin_port = htons(BW_PORT);
    result = bind(listensocket, (struct sockaddr*)&local,
                  sizeof(local));

    if (result < 0) {
        fprintf(stderr, "Bind failed.\n");
        return -1;
    }

    result = listen(listensocket, 1);
    if (result < 0) {
        fprintf(stderr, "Listen failed.\n");
        return -1;
    }

    while(1) {
	struct sockaddr_in remote;
        unsigned int sockaddr_len = sizeof(remote);
        int clientsocket = accept(listensocket,
                                  (struct sockaddr*)&remote,
                                  &sockaddr_len);
        if (clientsocket < 0) {
            fprintf(stderr, "Accept failed.\n");
            return -1;
        }
        
        client_worker(clientsocket);

        close(clientsocket);
    }

    close(listensocket);

    return 0;
}
