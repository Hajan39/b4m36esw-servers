#ifndef SERVER_SERVER_H
#define SERVER_SERVER_H

#include <microhttpd.h>
#include "dictionary.h"

#define PORT 8322

#define POSTBUFFERSIZE  1048576
#define MAXANSWERSIZE   512

#define GET             0
#define POST            1

int start_server();
static int handle_get(MHD_Connection *);
static int handle_post(MHD_Connection *, const char *, size_t *, void **);

#endif //SERVER_SERVER_H
