#ifndef CONFIG_H
#define CONFIG_H

// Global width / height of display

#define WIDTH            24
#define HEIGHT           24

#define DISP_SIZE        WIDTH * HEIGHT
#define DISP_BUF_SIZE    DISP_SIZE * 3

/** Reads config.
    \param conf config to write into
    \return 1 on success, 0 on failure
*/

int read_config(int (*handler)(void* user, const char* name,
                               const char* value),
                void * user, char * application);

#endif
