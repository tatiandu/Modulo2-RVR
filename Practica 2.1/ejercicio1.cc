#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <cstring>
#include <iostream>

//  int ai_family;		        /* Protocol family for socket. */
//  int ai_socktype;		    /* Socket type. */
//  socklen_t ai_addrlen;		/* Length of socket address. */
//  struct sockaddr *ai_addr;	/* Socket address for socket. */
//  struct addrinfo *ai_next;	/* Pointer to next in list. */

int main(int argc, char** argv) {
    struct addrinfo hints;
    struct addrinfo* res;

    memset((void*) &hints, 0, sizeof(struct addrinfo));

    hints.ai_family = AF_UNSPEC; //Para IPv4 y IPv6
    hints.ai_socktype = 0; //Para todo tipo de sockets

    int returnCode = getaddrinfo(argv[1], argv[2], &hints, &res);

    if (returnCode != 0) {
        std::cerr << "Error: " << gai_strerror(returnCode) << '\n';
        return -1;
    }

    for (auto i = res; i != nullptr; i = i->ai_next) {
        char host[NI_MAXHOST];
        char serv[NI_MAXSERV];

        getnameinfo(i->ai_addr, i->ai_addrlen, host, NI_MAXHOST, serv, NI_MAXSERV,
                    NI_NUMERICHOST | NI_NUMERICSERV);

        //Para cada dirección mostrar la IP numerica, la familia y tipo de socket
        std::cout << host << "    " << i->ai_family << "    "
                  << i->ai_socktype << '\n';
    }

    freeaddrinfo(res); //Liberar

    return 0;
}