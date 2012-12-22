#include <stdio.h>
#include <string.h>
#include "ini.h"
#include "config.h"

static const char * config_files[] = {
    "blinken.conf",
    "/usr/local/etc/blinken.conf",
    "/etc/blinken.conf"
};

#define MATCH(s, n) strcmp(section, s) == 0 && strcmp(name, n) == 0

struct bl_config {
    char * section;
    void * user;
    int (*handler)(void* user, const char* name,
                   const char* value);
};

static int local_handler(void* user, const char* section,
                   const char* name, const char* value)
{
    struct bl_config * pconfig = (struct bl_config *)user;

    if (!strcmp(pconfig->section, section)) {
        return pconfig->handler(pconfig->user, name, value);
    }

    return 1;
}

int read_config(int (*handler)(void* user, const char* name,
                               const char* value),
                void * user, char * application) {
    int i;
    int ini_read = 0;
    int n_confs = sizeof(config_files) / sizeof(char *);
    struct bl_config conf;

    conf.section = application;
    conf.user = user;
    conf.handler = handler;

    for (i=0; i<n_confs; i++) {
        if (ini_parse(config_files[i], local_handler, &conf) == 0) {
            ini_read = 1;
            break;
        }
    }

    if (!ini_read) {
        fprintf(stderr,
                "No blinken config found ...\n");
        return 0;
    }

    return 1;
}
