#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <cstring>
#include <unistd.h>
#include <iostream>

int main(int argc, char** argv) {
    struct addrinfo hints;
    struct addrinfo* res;

    memset((void*) &hints, 0, sizeof(struct addrinfo));

    hints.ai_flags    = AI_PASSIVE; //Devolver 0.0.0.0
    hints.ai_family = AF_INET; //Para IPv4
    hints.ai_socktype = SOCK_DGRAM; //Para UDP

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

    bool exit = false;
    while(!exit)
    {
        int len = 80;
        char buffer[len];
        char host[NI_MAXHOST];
        char serv[NI_MAXSERV];
        struct sockaddr cliente;
        socklen_t clienteLen = sizeof(struct sockaddr);

        int bytes = recvfrom(sd, (void*)buffer, len-1, 0, &cliente, &clienteLen);
        if (bytes == -1) {
            std::cerr << "Error [recvfrom]\n";
            return -1;
        }
        buffer[bytes] = '\0';

        getnameinfo(&cliente, clienteLen, host, NI_MAXHOST, serv, NI_MAXSERV,
                    NI_NUMERICHOST | NI_NUMERICSERV);

        std::cout << bytes << " bytes de " << host << ":" << serv << '\n';

        char reply[80];
        time_t rawtime;
        struct tm* timeInfo;
        size_t bytesSend;

        switch(buffer[0]) {
            case 't':
                time(&rawtime);
                timeInfo = localtime(&rawtime);
                bytesSend = strftime(reply, sizeof(reply), "%I:%M:%S %p", timeInfo);
                sendto(sd, reply, bytesSend, 0, &cliente, clienteLen);
                break;

            case 'd':
                time(&rawtime);
                timeInfo = localtime(&rawtime);
                bytesSend = strftime(reply, sizeof(reply), "%F", timeInfo);
                sendto(sd, reply, bytesSend, 0, &cliente, clienteLen);
                break;

            case 'q': //Envia un mensaje vacio
                exit = true;
                std::cout << "Saliendo...\n";
                break;

            default: //Envia un mensaje vacio
                std::cout << "Comando no soportado " << buffer[0] << '\n';
                break;
        }
    }
    close(sd); //Cerrar socket

    return 0;
}