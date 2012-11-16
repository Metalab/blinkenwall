#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/types.h>
#include <dirent.h>
#include "joystick.h"

struct controller_handle
{
    int n_joysticks;
    int joystick_fd[MAX_JOYSTICKS];
    int max_fd;
};

struct js_event {
	unsigned int time;	    /* event timestamp in milliseconds */
	short value;            /* value */
	unsigned char type;     /* event type */
	unsigned char number;   /* axis/button number */
};

struct controller_handle * open_controller(int types)
{
    struct dirent * dp;
    DIR * d;
    struct controller_handle * handle =
        malloc(sizeof(struct controller_handle));

    if (!handle)
        return NULL;

    handle->n_joysticks = 0;
    handle->max_fd = 0;

    if (types & CONTROLLER_TYPE_JOYSTICK) {
        d = opendir(JOYSTICK_DEVICES_DIR);
        if (d) {
            while (dp = readdir(d)) {
                if (!strncmp(dp->d_name, JOYSTICK_DEVICES_FILTER,
                             strlen(JOYSTICK_DEVICES_FILTER))) {
                    char path[1024];
                    int fd;
                    strcpy(path, JOYSTICK_DEVICES_DIR);
                    strcat(path, "/");
                    strcat(path, dp->d_name);
                    fd = open(path, O_RDONLY);
                    if (fd > handle->max_fd)
                    {
                        handle->max_fd = fd;
                        handle->joystick_fd[handle->n_joysticks++] = fd;
                    }
                }
            }
        }
    }

    if (!handle->n_joysticks) {
        free(handle);
        handle = NULL;
    }

    return handle;
}

struct command read_command(struct controller_handle * handle,
                            int max_delay)
{
    fd_set readfds;
    struct timeval tv;
    struct timeval * tv_p;
    int ret;
    int i;

    struct command c;

    c.controller = -1;
    c.number = 0;

    FD_ZERO(&readfds);
    for (i=0; i<handle->n_joysticks; i++) {
        FD_SET(handle->joystick_fd[i], &readfds);
    }
    if (max_delay > 0) {
        tv.tv_sec = max_delay / 1000;
        tv.tv_usec = (max_delay % 1000) * 1000;
        tv_p = &tv;
    } else {
        tv_p = NULL;
    }

    ret = select(handle->max_fd+1, &readfds, NULL, NULL, tv_p);
    if (ret > 0) {
        for (i=0; i<handle->n_joysticks; i++) {
            if (FD_ISSET(handle->joystick_fd[i], &readfds)) {
                struct js_event jse;
                int len = read(handle->joystick_fd[i], &jse, sizeof(jse));
                if (len == sizeof(jse)) {
                    c.controller = i;
                    c.number = jse.number;
                    c.type = jse.type;
                    c.value = jse.value;
                }
            }
        }
    }

    return c;
}

void close_controller(struct controller_handle * handle)
{
    int i;
    for (i=0; i<handle->n_joysticks; i++) {
        close(handle->joystick_fd[i]);
    }
    free(handle);
}
