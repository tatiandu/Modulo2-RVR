#include <string.h>

#include "Serializable.h"
#include "Socket.h"

Socket::Socket(const char * address, const char * port):sd(-1)
{
    struct addrinfo hints;
    struct addrinfo* res;

    memset((void*) &hints, 0, sizeof(struct addrinfo));

    hints.ai_family = AF_INET; //Para IPv4
    hints.ai_socktype = SOCK_DGRAM; //Para UDP

    int returnCode = getaddrinfo(address, port, &hints, &res);
    if (returnCode != 0) {
        std::cerr << "Error [getaddrinfo]: " << gai_strerror(returnCode) << '\n';
        return;
    }

    int sd = socket(res->ai_family, res->ai_socktype, 0);
    if (sd == -1) {
        std::cerr << "Error [socket]: Creación socket\n";
        return;
    }

    sa = *res->ai_addr;
    sa_len = res->ai_addrlen;

    freeaddrinfo(res); //Liberar
}

int Socket::recv(Serializable &obj, Socket * &sock)
{
    struct sockaddr sa;
    socklen_t sa_len = sizeof(struct sockaddr);

    char buffer[MAX_MESSAGE_SIZE];

    ssize_t bytes = ::recvfrom(sd, buffer, MAX_MESSAGE_SIZE, 0, &sa, &sa_len);

    if ( bytes <= 0 )
    {
        return -1;
    }

    if ( sock != 0 )
    {
        sock = new Socket(&sa, sa_len);
    }

    obj.from_bin(buffer);

    return 0;
}

int Socket::send(Serializable& obj, const Socket& sock)
{
    obj.to_bin();

    int bytes = sendto(sock.sd, obj.data(), obj.size(), 0, &sock.sa, sock.sa_len);
    if (bytes == -1) {
        std::cerr << "Error [Socket::send]\n";
        return -1;
    }
    else
        return 0;
}

bool operator== (const Socket &s1, const Socket &s2)
{
    struct sockaddr_in* sock1 = (struct sockaddr_in*) &s1.sa;
    struct sockaddr_in* sock2 = (struct sockaddr_in*) &s2.sa;

    return (sock1->sin_family == sock2->sin_family &&
            sock1->sin_addr.s_addr == sock2->sin_addr.s_addr &&
            sock1->sin_port == sock2->sin_port);
};

std::ostream& operator<<(std::ostream& os, const Socket& s)
{
    char host[NI_MAXHOST];
    char serv[NI_MAXSERV];

    getnameinfo((struct sockaddr *) &(s.sa), s.sa_len, host, NI_MAXHOST, serv,
                NI_MAXSERV, NI_NUMERICHOST);

    os << host << ":" << serv;

    return os;
};

