#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <cstring>
#include <unistd.h>
#include <iostream>

//No hace falta hacer bind porque es el cliente

//Ej.: ./time_client 127.0.0.1 3000 t
//argv 1 = IP
//argv 2 = Puerto
//argv 3 = Comando

int main(int argc, char** argv) {

    if(argc < 3) {
        std::cerr << "Usage: " << argv[0] << " host port command(t/d/q)\n";
        return 0;
    }

    struct addrinfo hints;
    struct addrinfo* res;

    memset((void*) &hints, 0, sizeof(struct addrinfo));

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

    int len = 80;
    char buffer[len];
    struct sockaddr server = *res->ai_addr;
    socklen_t serverLen = sizeof(struct sockaddr);

    freeaddrinfo(res); //Liberar

    //Enviar comando al servidor
    sendto(sd, argv[3], sizeof(char), 0, &server, serverLen);

    if (argv[3][0] != 'q') { 
        //Recibir respuesta del servidor si no quiere salir
        int bytes = recvfrom(sd, (void*)buffer, len-1, 0, &server, &serverLen);
        if (bytes == -1) {
            std::cerr << "Error [recvfrom]\n";
            return -1;
        }
        buffer[bytes] = '\0';

        std::cout << buffer << '\n';
    }
    close(sd); //Cerrar socket

    return 0;
}