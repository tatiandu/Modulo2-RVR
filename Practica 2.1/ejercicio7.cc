#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <cstring>
#include <unistd.h>
#include <iostream>
#include <thread>

#define MAX_THREAD 5

class MessageThread
{
private:
    int sd_;

public:
    MessageThread(int sd) : sd_(sd) {};

    void do_message() {
        while (true) {
            int len = 80;
            char buffer[len];

            int bytes = recv(sd_, (void*)buffer, len-1, 0);
            if (bytes <= 0) {
                std::cerr << "Conexión terminada\n";
                break;
            }
            buffer[bytes] = '\0';

            send(sd_, buffer, bytes, 0);
        }
        close(sd_);
    }
};

int main(int argc, char** argv) {
    struct addrinfo hints;
    struct addrinfo* res;

    memset((void*) &hints, 0, sizeof(struct addrinfo));

    hints.ai_family   = AF_INET; //Para IPv4
    hints.ai_socktype = SOCK_STREAM; //Para TCP

    int returnCode = getaddrinfo(argv[1], argv[2], &hints, &res);
    if (returnCode != 0) {
        std::cerr << "Error [getaddrinfo]: " << gai_strerror(returnCode) << '\n';
        return -1;
    }

    int sd = socket(res->ai_family, res->ai_socktype, 0);
    if (sd == -1) {
        std::cerr << "Error [socket]: Creación socket\n";
        return -1;
    }

    // bind: "assigning a name to a socket"
    if (bind(sd, res->ai_addr, res->ai_addrlen) == -1) {
        std::cerr << "Error [bind]\n";
        return -1;
    }

    freeaddrinfo(res); //Liberar

    if (listen(sd, 16) == -1) {
        std::cerr << "Error [listen]\n";
        return -1;
    }

    char host[NI_MAXHOST];
    char serv[NI_MAXSERV];
    struct sockaddr cliente;
    socklen_t clienteLen = sizeof(struct sockaddr);

    while (true) {
        int client_sd = accept(sd, &cliente, &clienteLen);
        if (client_sd == -1) {
            std::cerr << "Error [accept]\n";
            return -1;
        }

        getnameinfo(&cliente, clienteLen, host, NI_MAXHOST, serv, NI_MAXSERV,
            NI_NUMERICHOST | NI_NUMERICSERV);

        std::cout << "Conexión desde " << host << " " << serv << std::endl;

        MessageThread *mt = new MessageThread(client_sd);

        std::thread([&mt](){
            mt->do_message();
            delete mt;
        }).detach();
    }
    close(sd); //Cerrar socket

    return 0;
}