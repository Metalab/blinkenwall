/* Metalab Blinkenwall websocket listener
 * Listens on TCP/IP port, receives commands using
 * the blinkenwall control protocol.
 * 
 * Code by Georg <georg.lippitsch@gmx.at> and Wolfgang <wsys@wsys.at>
 */

#include <stdint.h>

#ifndef BW_LIBWBL_H
#define BW_LIBWBL_H

#define BW_DEFAULTPORT    15633
#define BW_DEFAULTTIMEOUT 300
#define BW_READ_BUF_SIZE  2048
#define BW_CTRL_RESOURCE  "/blinkenwallcontrol"

#define BW_CMD_NONE             0x0
#define BW_CMD_UP_PRESSED       0x1
#define BW_CMD_DOWN_PRESSED     0x2
#define BW_CMD_LEFT_PRESSED     0x3
#define BW_CMD_RIGHT_PRESSED    0x4
#define BW_CMD_BUTTON1_PRESSED  0x5
#define BW_CMD_BUTTON2_PRESSED  0x6
#define BW_CMD_UP_RELEASED      0x7
#define BW_CMD_DOWN_RELEASED    0x8
#define BW_CMD_LEFT_RELEASED    0x9
#define BW_CMD_RIGHT_RELEASED   0xA
#define BW_CMD_BUTTON1_RELEASED 0xB
#define BW_CMD_BUTTON2_RELEASED 0xC
#define BW_CMD_LAST             0xD

typedef struct BwlSocketContext {
    int fd_listen; ///< File descriptor of listen socket
    int fd_accept; ///< File descriptor of connected socket
    int connected;
    uint8_t buffer[BW_READ_BUF_SIZE];
} BwlSocketContext;

/**
 * Initializes BwlSocketContext and opens the losten socket
 * @param port TCP/IP port to listen on
 * @return a pointer to a new BwlSocketContext,
 * or NULL on failure.
 */
BwlSocketContext * bw_socket_open_port(int port);

/**
 * Initializes BwlSocketContext and opens the losten socket
 * @return a pointer to a new BwlSocketContext,
 * or NULL on failure.
 */
BwlSocketContext * bw_socket_open();

/**
 * Closes the socket and destroys (frees) the
 * BwlSocketContext.
 * @param socket_context BwlSocketContext to close
 */
void bw_socket_close(BwlSocketContext * socket_context);

/**
 * Listens on port for connections, blocks until a client
 * makes a new connection.
 * @param timeout maximum time to wait for connections, in seconds
 * @return 0 on success, -1 on failure or timeout
 * or NULL on failure.
 */
int bw_wait_for_connections(BwlSocketContext * socket_context,
                            int timeout);

/**
 * Blocks until a new control command is received,
 * and returns the command.
 * @return 0 on success, -1 on failure
 */
int bw_get_cmd_block(BwlSocketContext * socket_context);

/**
 * Blocks until a new control command is received,
 * or until timeout has elapsed.
 * @param timeout maximum time to block, in milliseconds
 * @return the command received, one of the BW_CMD_* defines,
 or BW_CMD_NONE if the timeout has occured.
 */
int bw_get_cmd_block_timeout(BwlSocketContext * socket_context,
                             int timeout);

#endif
