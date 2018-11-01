/*
 * Copyright (C) 2017 Sam Kumar <samkumar@berkeley.edu>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    net_sock_tcp    TCP connections
 * @ingroup     net_sock
 * @brief       Connection submodule for TCP connections
 * @{
 *
 * @file
 * @brief   TCP connection definitions
 *
 * @author  Sam Kumar <samkumar@berkeley.edu>
 */
#ifndef NET_SOCK_TCP_FREEBSD_H_
#define NET_SOCK_TCP_FREEBSD_H_

#include <stdint.h>
#include <stdlib.h>

#include "cib.h"
#include "net/sock.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Forward declaration of @ref sock_tcp_freebsd_t to allow for external definition.
 */
struct sock_tcp_freebsd;

/**
 * @brief   Implementation-specific type of a TCP connection object
 */
typedef struct sock_tcp_freebsd sock_tcp_freebsd_t;

/**
 * @brief   Creates a new TCP connection object
 *
 * @param[out] conn     Preallocated connection object. Must fill the size of the stack-specific
 *                      connection desriptor.
 * @param[in] addr      The local network layer address for @p conn.
 * @param[in] addr_len  The length of @p addr. Must be fitting for the @p family.
 * @param[in] family    The family of @p addr (see @ref net_af).
 * @param[in] port      The local TCP port for @p conn.
 *
 * @return  0 on success.
 * @return  any other negative number in case of an error. For portability implementations should
 *          draw inspiration of the errno values from the POSIX' bind() function specification.
 */
int sock_tcp_freebsd_create(sock_tcp_freebsd_t *conn, const void *addr, size_t addr_len, int family,
                            uint16_t port);

/**
 * @brief   Closes a TCP connection
 *
 * @param[in,out] conn  A TCP connection object.
 */
void sock_tcp_freebsd_close(sock_tcp_freebsd_t *conn);

/**
 * @brief   Gets the local address of a TCP connection
 *
 * @param[in] conn  A TCP connection object.
 * @param[out] addr The local network layer address. Must have space for any address of
 *                  the connection's family.
 * @param[out] port The local TCP port.
 *
 * @return  length of @p addr on success.
 * @return  any other negative number in case of an error. For portability implementations should
 *          draw inspiration of the errno values from the POSIX' getsockname() function
 *          specification.
 */
int sock_tcp_freebsd_getlocaladdr(sock_tcp_freebsd_t *conn, void *addr, uint16_t *port);

/**
 * @brief   Gets the address of the connected peer of a TCP connection
 *
 * @param[in] conn  A TCP connection object.
 * @param[out] addr The network layer address of the connected peer. Must have space for any
 *                  address of the connection's family.
 * @param[out] port The TCP port of the connected peer.
 *
 * @return  length of @p addr on success.
 * @return  any other negative number in case of an error. For portability implementations should
 *          draw inspiration of the errno values from the POSIX' getpeername() function
 *          specification.
 */
int sock_tcp_freebsd_getpeeraddr(sock_tcp_freebsd_t *conn, void *addr, uint16_t *port);

/**
 * @brief   Connects to a remote TCP peer
 *
 * @param[in] conn      A TCP connection object.
 * @param[in] addr      The remote network layer address for @p conn.
 * @param[in] addr_len  Length of @p addr.
 * @param[in] port      The remote TCP port for @p conn.
 *
 * @return  0 on success.
 * @return  any other negative number in case of an error. For portability implementations should
 *          draw inspiration of the errno values from the POSIX' connect() function specification.
 */
int sock_tcp_freebsd_connect(sock_tcp_freebsd_t *conn, const void *addr, size_t addr_len, uint16_t port);

/**
 * @brief   Marks connection to listen for a connection request by a remote TCP peer
 *
 * @param[in] conn      A TCP connection object.
 * @param[in] queue_len Maximum length of the queue for connection requests.
 *                      An implementation may choose to silently adapt this value to its needs
 *                      (setting it to a minimum or maximum value). Any negative number must be
 *                      set at least to 0.
 *
 * @return  0 on success.
 * @return  any other negative number in case of an error. For portability implementations should
 *          draw inspiration of the errno values from the POSIX' listen() function specification.
 */
int sock_tcp_freebsd_listen(sock_tcp_freebsd_t *conn, int queue_len);

/**
 * @brief   Receives and handles TCP connection requests from other peers
 *
 * @param[in] conn      A TCP connection object.
 * @param[out] out_conn A new TCP connection object for the established connection.
 *
 * @return  0 on success.
 * @return  any other negative number in case of an error. For portability implementations should
 *          draw inspiration of the errno values from the POSIX' accept() function specification.
 */
int sock_tcp_freebsd_accept(sock_tcp_freebsd_t *conn, sock_tcp_freebsd_t *out_conn);

/**
 * @brief   Receives a TCP message
 *
 * @param[in] conn      A TCP connection object.
 * @param[out] data     Pointer where the received data should be stored.
 * @param[in] max_len   Maximum space available at @p data.
 *
 * @note    Function may block.
 *
 * @return  The number of bytes received on success.
 * @return  0, if no received data is available, but everything is in order.
 * @return  any other negative number in case of an error. For portability, implementations should
 *          draw inspiration of the errno values from the POSIX' recv(), recvfrom(), or recvmsg()
 *          function specification.
 */
int sock_tcp_freebsd_recv(sock_tcp_freebsd_t *conn, void *data, size_t max_len);

/**
 * @brief   Sends a TCP message
 *
 * @param[in] conn  A TCP connection object.
 * @param[in] data  Pointer where the received data should be stored.
 * @param[in] len   Maximum space available at @p data.
 *
 * @note    Function may block.
 *
 * @return  The number of bytes send on success.
 * @return  any other negative number in case of an error. For portability, implementations should
 *          draw inspiration of the errno values from the POSIX' send(), sendfrom(), or sendmsg()
 *          function specification.
 */
int sock_tcp_freebsd_send(sock_tcp_freebsd_t *conn, const void *data, size_t len);

#include "tcp_freebsd.h"
#include "cond.h"

/*
 * @brief    TCP FREEBSD sock type
 * @internal
 */
struct sock_tcp_freebsd {
    //gnrc_netreg_entry_t netreg_entry; // to follow the inheritance

    ipv6_addr_t local_addr;
    uint16_t local_port;

    mutex_t lock;
    int pending_ops;
    cond_t pending_cond;
    union {
        struct {
            int asock;
            bool is_connecting;
            bool got_fin;
            cond_t connect_cond;
            cond_t receive_cond;
            cond_t send_cond;
        } active;
        struct {
            int psock;
            cond_t accept_cond;

            /* Circular buffer for accept queue. */
            cib_t accept_cib;
            struct sock_tcp_freebsd_accept_queue_entry* accept_queue;
        } passive;
    } sfields; /* specific fields */
    int errstat;
    bool hasactive;
    bool haspassive;
};

#ifdef __cplusplus
}
#endif

#endif /* NET_CONN_TCP_H_ */
/** @} */
