/** A very descriptively-named header file **/
#ifndef PORTABLE_NET_H
#define PORTABLE_NET_H

#include "shoot.h"

#ifdef _WIN32
#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0600
#endif
#include <winsock2.h>
#include <ws2tcpip.h>

#ifndef AI_ALL
#define AI_ALL 0x0100
#endif

#else

#ifndef __USE_XOPEN2K
#define __USE_XOPEN2K
#endif
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>

#endif

#ifdef _WIN32
#define ISVALIDSOCKET(sock) ((sock) != INVALID_SOCKET)
#define CLOSESOCKET(sock) closesocket(sock)
#else
#define ISVALIDSOCKET(sock) ((sock) >= 0)
#define CLOSESOCKET(sock) close(sock)
#define SOCKET int
#endif

#ifdef _WIN32
#define NET_STARTUP WSADATA d; verify(!WSAStartup(MAKEWORD(2, 2), &d), "WSAStartup failure");
#define NET_SHUTDOWN WSACleanup();
#else
#define NET_STARTUP
#define NET_SHUTDOWN
#endif

static void netPrintAddressIP(struct sockaddr *address, socklen_t length)
{
    int err;
    char address_buf[100], server_buf[100];
    err = getnameinfo(address, length, address_buf, 100, server_buf, 100, NI_NUMERICHOST | NI_NUMERICSERV);
    if (err != SUCCESS)
    {
        printf("address: [failed to get]\n");
        return;
    }
    printf("address: %s/%s\n", address_buf, server_buf);
}
static void netPrintAddressInfoAddressIP(struct addrinfo *address)
{
    int err;
    char address_buf[100], server_buf[100];
    err = getnameinfo(address->ai_addr, address->ai_addrlen, address_buf, 100, server_buf, 100, NI_NUMERICHOST | NI_NUMERICSERV);
    if (err != SUCCESS)
    {
        printf("address: [failed to get]\n");
        return;
    }
    printf("address: %s/%s\n", address_buf, server_buf);
}

/** NOTE: Socket family is IPv4. Socket_type can be SOCK_STREAM for TCP or SOCK_DGRAM for UDP - chief */
static SOCKET netConnect(const char *hostname, const char *port, int socket_type)
{
    int error_code;

    struct addrinfo hints = {
        .ai_family = AF_INET,
        .ai_socktype = socket_type,
    };

    struct addrinfo *peer_address;
    error_code = getaddrinfo(hostname, port, &hints, &peer_address);
    verify(error_code == SUCCESS, "failed to get address info");

    SOCKET host_socket = socket(peer_address->ai_family, peer_address->ai_socktype, peer_address->ai_protocol);
    verify(ISVALIDSOCKET(host_socket), "socket call failed for host_socket");
    /* error_code = bind(host_socket, peer_address->ai_addr, peer_address->ai_addrlen);
    verify(error_code == SUCCESS, "bind call failed for host_socket"); */

    error_code = connect(host_socket, peer_address->ai_addr, peer_address->ai_addrlen);
    verify(error_code == SUCCESS, "connect call failed for host_socket");

    printf("successfully connected to ");
    netPrintAddressInfoAddressIP(peer_address);

    freeaddrinfo(peer_address);

    return host_socket;
}
static SOCKET netListen(const char *port, uint32 connection_limit)
{
    int error_code;

    struct addrinfo hints = {
        .ai_socktype = SOCK_STREAM,
        .ai_flags = AI_PASSIVE,
        .ai_family = AF_INET,
    };
    struct addrinfo *peer_address;
    error_code = getaddrinfo(0, port, &hints, &peer_address);
    verify(error_code == SUCCESS, "failed to get address info");

    SOCKET listening_socket = socket(peer_address->ai_family, peer_address->ai_socktype, peer_address->ai_protocol);
    verify(ISVALIDSOCKET(listening_socket), "failed to create socket");

    error_code = bind(listening_socket, peer_address->ai_addr, peer_address->ai_addrlen);
    verify(error_code == SUCCESS, "bind() failure");

    freeaddrinfo(peer_address);

    error_code = listen(listening_socket, connection_limit);
    verify(error_code >= 0, "listen() failure");

    printf("listening on port %s...\n", port);

    return listening_socket;
}

static SOCKET netAccept(int socket_listen)
{
    struct sockaddr_storage client_address;
    socklen_t client_length = sizeof(client_address);
    SOCKET client_socket = accept(socket_listen, (struct sockaddr *)&client_address, &client_length);
    verify(ISVALIDSOCKET(client_socket), "failed to accept() client socket");

    printf("connected with client: ");
    netPrintAddressIP((struct sockaddr *)&client_address, client_length);

    return client_socket;
}

/* NET UDP FUNCTIONS */
// netServer()
//      netServerBroadcastIP()
//      netServerListenAndAccept()
//      netServerSend()
//      netServerReceive()

// netClient()
//      netClientConnect()
//      netClientSend()
//      netClientReceive()

typedef struct addrinfo AddressInfo;

#endif