#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <cstring>
#include <unistd.h>
#include <iostream>

//No hace falta hacer bind porque es el cliente

//Ej.: ./echo_client 127.0.0.1 2222
//argv 1 = IP
//argv 2 = Puerto

int main(int argc, char** argv) {

    if(argc < 2) {
        std::cerr << "Usage: " << argv[0] << " host port\n";
        return 0;
    }

    struct addrinfo hints;
    struct addrinfo* res;

    memset((void*) &hints, 0, sizeof(struct addrinfo));

    hints.ai_family = AF_INET; //Para IPv4
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

    if (connect(sd, res->ai_addr, res->ai_addrlen) == -1) {
        std::cerr << "Error [connect]\n";
        return -1;
    }

    freeaddrinfo(res); //Liberar

    while (true)
    {
        int len = 80;
		char buffer[len];

        std::cin >> buffer;
        int bufferLen = strlen(buffer);
        buffer[bufferLen] = '\0';

        if (buffer[0] == 'Q' && buffer[1] == '\0') {
            break;
        }

        send(sd, buffer, bufferLen, 0);
        int bytes = recv(sd, (void*)buffer, len-1, 0);

        std::cout << buffer << '\n';
    }
    close(sd); //Cerrar socket

    return 0;
}