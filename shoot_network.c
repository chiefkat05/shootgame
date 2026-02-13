/** A very descriptively-named header file **/
#ifndef PORTABLE_NET_H
#define PORTABLE_NET_H

#include "shoot.h"

static SOCKET shoot_net_open_listening_socket(const char *hostname, const char *port, struct addrinfo **listening_address)
{
    int error_code;

    struct addrinfo hints = {
        .ai_family = AF_INET,
        .ai_socktype = SOCK_DGRAM,
    };
    error_code = getaddrinfo(hostname, port, &hints, listening_address);
    verify(error_code == SUCCESS, "failed to get address info");

    SOCKET host_socket = socket((*listening_address)->ai_family, (*listening_address)->ai_socktype, (*listening_address)->ai_protocol);
    verify(ISVALIDSOCKET(host_socket), "socket call failed for host_socket");

    error_code = bind(host_socket, (*listening_address)->ai_addr, (*listening_address)->ai_addrlen);
    if (error_code != SUCCESS)
    {
        printf("Network Error: Failed to bind address\n");
        freeaddrinfo(*listening_address);
        return -1;
    }

    return host_socket;
}
static void shoot_net_broadcast(const char *hostname, const char *port, void *data, uint64 data_length)
{
    int error_code;

    struct addrinfo hints = {
        .ai_family = AF_INET,
        .ai_socktype = SOCK_DGRAM,
    };
    struct addrinfo *broadcast_address;
    error_code = getaddrinfo(hostname, port, &hints, &broadcast_address);
    verify(error_code == SUCCESS, "failed to get address info");

    SOCKET broadcast_socket = socket(broadcast_address->ai_family, broadcast_address->ai_socktype, broadcast_address->ai_protocol);
    // int option = 1;
    // setsockopt(broadcast_socket, SOL_SOCKET, SO_BROADCAST, &option, sizeof(option));
    verify(ISVALIDSOCKET(broadcast_socket), "socket call failed for broadcast_socket");

    int64 bytes_sent = sendto(broadcast_socket, data, data_length, 0, broadcast_address->ai_addr, broadcast_address->ai_addrlen);
    if (bytes_sent < 0)
    {
        printf("Network Error: Failed to broadcast message\n");
    }

    printf("broadcasting existance to local area network using hostname and port %s %s\n", hostname, port);
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

    return peer_socket;
}
static void shoot_net_receive(SOCKET host_socket, struct sockaddr *return_address, socklen_t *return_length, void *data_out, uint64 data_length)
{
    struct sockaddr_storage socket_address;
    uint32 socket_address_length = sizeof(socket_address);
    int bytes_received = recvfrom(host_socket, data_out, data_length, 0, (struct sockaddr *)&socket_address, &socket_address_length);
    if (bytes_received < 1)
    {
        printf("failed to receive data to peer\n");
    }
    printf("received %i bytes of data from peer\n", bytes_received);
}
static void shoot_net_send(SOCKET peer_socket, struct addrinfo *peer_address, void *data, uint64 data_length)
{
    uint64 bytes_sent = sendto(peer_socket, data, data_length, 0, peer_address->ai_addr, peer_address->ai_addrlen);
    if (bytes_sent < 1)
    {
        printf("failed to sent data to peer\n");
    }

    char hostname[100], port[100];
    getnameinfo(peer_address->ai_addr, peer_address->ai_addrlen, hostname, sizeof(hostname), port, sizeof(port), NI_NUMERICHOST | NI_NUMERICSERV);
    printf("sent %lu bytes of data to peer at %s %s\n", bytes_sent, hostname, port);
}

/** Keep updating this untill you're happy with it, e.g. maybe the header can include some data information or something. - Chief **/
static bool32 shoot_net_poll(SOCKET listening_socket, SOCKET max_socket,
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
        struct sockaddr_storage peer_socket_address;
        uint32 peer_socket_address_length;
        shoot_net_receive(listening_socket, (struct sockaddr *)&peer_socket_address, &peer_socket_address_length, out_data, out_data_length);

        return TRUE;
    }
    return FALSE;
}
static void shoot_net_get_socket_ip(SOCKET host_socket, char *out_hostname, uint32 out_hostname_length,
            char *out_port, uint32 out_port_length)
{
    struct sockaddr_storage network_socket_address;
    uint32 network_socket_address_length = sizeof(network_socket_address);
    getsockname(host_socket, (struct sockaddr *)&network_socket_address, &network_socket_address_length);

    getnameinfo((struct sockaddr *)&network_socket_address, network_socket_address_length,
            out_hostname, out_hostname_length, out_port, out_port_length, NI_NUMERICHOST | NI_NUMERICSERV);
}
static void shoot_net_get_address_ip(struct addrinfo *address, char *out_hostname, uint32 out_hostname_length,
            char *out_port, uint32 out_port_length)
{
    getnameinfo(address->ai_addr, address->ai_addrlen, out_hostname, out_hostname_length, out_port, out_port_length, NI_NUMERICHOST | NI_NUMERICSERV);
}
static void shoot_net_get_socket_address_ip(struct sockaddr *socket_address, uint32 socket_address_length,
            char *out_hostname, uint32 hostname_length, char *out_port, uint32 port_length)
{
    char hostname_buffer[100], port_buffer[100];
    getnameinfo(socket_address, socket_address_length,
        hostname_buffer, sizeof(hostname_buffer), port_buffer, sizeof(port_buffer), NI_NUMERICHOST);

    strcpy(out_hostname, hostname_buffer);
    strcpy(out_port, port_buffer);
}

typedef struct addrinfo AddressInfo;

#endif