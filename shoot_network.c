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

static SOCKET shoot_net_open_listening_socket(const char *hostname, const char *port)
{
    int error_code;

    struct addrinfo *listening_address;

    struct addrinfo hints = {
        .ai_family = AF_INET,
        .ai_socktype = SOCK_DGRAM,
    };
    error_code = getaddrinfo(hostname, port, &hints, &listening_address);
    verify(error_code == SUCCESS, "failed to get address info");

    SOCKET host_socket = socket(listening_address->ai_family, listening_address->ai_socktype, listening_address->ai_protocol);
    verify(ISVALIDSOCKET(host_socket), "socket call failed for host_socket");

    error_code = bind(host_socket, listening_address->ai_addr, listening_address->ai_addrlen);
    if (error_code != SUCCESS)
    {
        printf("Network Error: Failed to bind address\n");
        freeaddrinfo(listening_address);
        return -1;
    }

    printf("opened listening socket for receiving at ip %s\n", hostname);
    freeaddrinfo(listening_address);

    return host_socket;
}
static SOCKET shoot_net_open_peer_socket(const char *hostname, const char *port, struct addrinfo **peer_address)
{
    int error_code;

    if (peer_address == NULL)
    {
        printf("please include a pointer to a (struct addrinfo *) object\n");
        return -1;
    }

    struct addrinfo hints = {
        .ai_family = AF_INET,
        .ai_socktype = SOCK_DGRAM,
    };
    error_code = getaddrinfo(hostname, port, &hints, peer_address);
    verify(error_code == SUCCESS, "failed to get address info");

    SOCKET peer_socket = socket((*peer_address)->ai_family, (*peer_address)->ai_socktype, (*peer_address)->ai_protocol);
    verify(ISVALIDSOCKET(peer_socket), "socket call failed for peer_socket");

    printf("opened peer socket for sending to ip %s\n", hostname);

    return peer_socket;
}
static void shoot_net_receive(SOCKET host_socket, struct sockaddr *return_address, socklen_t *return_length, void *data_out, uint64 data_length)
{
    int bytes_received = recvfrom(host_socket, data_out, data_length, 0, return_address, return_length);
    if (bytes_received < 1)
    {
        printf("failed to receive data to peer\n");
    }
}
static void shoot_net_send(SOCKET peer_socket, struct addrinfo *peer_address, void *data, uint64 data_length)
{
    uint64 bytes_sent = sendto(peer_socket, data, data_length, 0, peer_address->ai_addr, peer_address->ai_addrlen);
    if (bytes_sent < 1)
    {
        printf("failed to sent data to peer\n");
    }
}

/** Keep updating this untill you're happy with it, e.g. maybe the header can include some data information or something. - Chief **/
static void shoot_net_poll(SOCKET listening_socket, SOCKET max_socket,
    void *out_data, uint64 out_data_length)
{
    fd_set read_set;

    FD_ZERO(&read_set);
    FD_SET(listening_socket, &read_set);

    /** Unless doing multithreading, please never set this to anything greater than 0. - chief **/
    struct timeval wait_timer = {.tv_sec = 0, .tv_usec = 0};
    verify(select(max_socket + 1, &read_set, 0, 0, &wait_timer) >= 0, "select() failure");

    if (FD_ISSET(listening_socket, &read_set))
    {
        shoot_net_receive(listening_socket, 0, 0, out_data, out_data_length);
    }
}

typedef struct addrinfo AddressInfo;

#endif