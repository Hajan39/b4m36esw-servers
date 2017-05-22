#include <sys/types.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <iostream>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "server.h"

/*
 * Built using this great tutorial.
 * https://www.gnu.org/software/libmicrohttpd/tutorial.html
 */

struct connection_info_struct {
    int connectiontype;
    int content_length;
    char *data;
};

/**
 * Sends HTTP response with given content and status.
 *
 * @param connection    The connection.
 * @param content       Content of the response.
 * @param status        Status of the response.
 * @return
 */
static int send_response(struct MHD_Connection *connection, const char *content, unsigned int status) {
    int ret;
    struct MHD_Response *response;

    if (content != NULL) {
        response = MHD_create_response_from_buffer(strlen(content), (void *) content, MHD_RESPMEM_PERSISTENT);
    } else {
        response = MHD_create_response_from_buffer(0, NULL, MHD_RESPMEM_PERSISTENT);
    }
    if (!response) return MHD_NO;

    ret = MHD_queue_response(connection, status, response);
    MHD_destroy_response(response);

    return ret;
}

/**
 * Cleans up memory after a HTTP request has been handled.
 *
 * @param cls
 * @param connection
 * @param con_cls
 * @param toe
 */
static void request_completed(void *cls, struct MHD_Connection *connection,
                              void **con_cls, enum MHD_RequestTerminationCode toe) {
    struct connection_info_struct *con_info = (connection_info_struct *) *con_cls;

    if (NULL == con_info) return;

    if (con_info->connectiontype == POST) {
        if (con_info->data) free(con_info->data);
    }

    free(con_info);
    *con_cls = NULL;
}

/**
 * Answers the HTTP connection.
 *
 * @param cls
 * @param connection
 * @param url
 * @param method
 * @param version
 * @param upload_data
 * @param upload_data_size
 * @param con_cls
 * @return
 */
static int answer_to_connection(void *cls, struct MHD_Connection *connection,
                                const char *url, const char *method,
                                const char *version, const char *upload_data,
                                size_t *upload_data_size, void **con_cls) {


    // If this is new request, create connection info structure
    if (*con_cls == NULL) {
        //std::cout << method << " " << url << std::endl;
        struct connection_info_struct *con_info;

        con_info = (connection_info_struct *) malloc(sizeof(struct connection_info_struct));
        if (con_info == NULL) return MHD_NO;

        con_info->data = NULL;
        con_info->content_length = 0;

        if (0 == strcmp(method, "POST")) {
            con_info->connectiontype = POST;
            con_info->data = (char *) malloc(POSTBUFFERSIZE);
            memset(con_info->data, 0, POSTBUFFERSIZE);
        } else {
            con_info->connectiontype = GET;
        }

        *con_cls = (void *) con_info;

        return MHD_YES;
    }

    if (0 == strcmp(method, "GET") && 0 == strcmp(url, "/esw/myserver/count")) {
        return handle_get(connection);
    }

    if (0 == strcmp(method, "POST") && 0 == strcmp(url, "/esw/myserver/data")) {
        return handle_post(connection, upload_data, upload_data_size, con_cls);
    }

    return send_response(connection, "Bad Request", MHD_HTTP_BAD_REQUEST);
}

/**
 * Handles GET request.
 *
 * @param connection The open connection.
 * @return
 */
static int handle_get(MHD_Connection *connection) {
    int word_count = count_unique_words();
    char answerstring[MAXANSWERSIZE];
    snprintf(answerstring, MAXANSWERSIZE, "%d", word_count);
    return send_response(connection, answerstring, MHD_HTTP_OK);
}

/**
 * Handles POST request.
 *
 * @param connection The open connection.
 * @param upload_data The uploaded data.
 * @param upload_data_size The uploaded data size.
 * @param con_cls Connection info.
 * @return
 */
static int handle_post(MHD_Connection *connection, const char *upload_data, size_t *upload_data_size, void **con_cls) {
    struct connection_info_struct *con_info = (connection_info_struct *) *con_cls;

    if (*upload_data_size != 0) {
        for (int i = con_info->content_length, j = 0; j < *upload_data_size; ++i, ++j) {
            con_info->data[i] = upload_data[j];
        }
        con_info->content_length += *upload_data_size;

        *upload_data_size = 0;

        return MHD_YES;

    } else if (con_info->data != NULL) {
        decompress_and_parse(con_info->data, con_info->content_length);
        return send_response(connection, NULL, MHD_HTTP_NO_CONTENT);
    }
}

/**
 * Starts the libmicrohttpd server.
 *
 * @return 0 if everything went okay, 1 if server could not be started.
 */
int start_server() {
    struct MHD_Daemon *daemon = MHD_start_daemon(MHD_USE_THREAD_PER_CONNECTION, PORT, NULL, NULL, &answer_to_connection,
                                                 NULL, MHD_OPTION_NOTIFY_COMPLETED, &request_completed, NULL,
                                                 MHD_OPTION_END);
    if (NULL == daemon) return 1;

    getchar(); // Wait for the user to press something

    MHD_stop_daemon(daemon);

    return 0;
}
