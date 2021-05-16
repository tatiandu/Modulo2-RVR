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
        int len = 80;
        char buffer[len];
        char host[NI_MAXHOST];
        char serv[NI_MAXSERV];
        struct sockaddr cliente;
        socklen_t clienteLen = sizeof(struct sockaddr);

        while (true) {
            int bytes = recvfrom(sd_, (void*)buffer, len-1, 0, &cliente, &clienteLen);
            if (bytes == -1) {
                std::cerr << "Error [recvfrom]\n";
                return;
            }
            buffer[bytes] = '\0';

            getnameinfo(&cliente, clienteLen, host, NI_MAXHOST, serv, NI_MAXSERV,
                        NI_NUMERICHOST | NI_NUMERICSERV);

            std::cout << "Thread [" << std::this_thread::get_id() << "] "
                      << bytes << " bytes de " << host << ":" << serv << '\n';;

            sleep(3); //Para poder comprobar la concurrencia

            char reply[len];
            time_t rawtime;
            struct tm* timeInfo;
            size_t bytesSend;

            time(&rawtime);
            timeInfo = localtime(&rawtime);

            switch(buffer[0]) {
                case 't':

                    bytesSend = strftime(reply, sizeof(reply), "%I:%M:%S %p", timeInfo);
                    sendto(sd_, reply, bytesSend, 0, &cliente, clienteLen);
                    break;

                case 'd':
                    bytesSend = strftime(reply, sizeof(reply), "%F", timeInfo);
                    sendto(sd_, reply, bytesSend, 0, &cliente, clienteLen);
                    break;

                case 'q': //Lo gestiona el thread principal
                    break;

                default:
                    std::cout << "Comando no soportado " << buffer[0] << '\n';
                    break;
            }
        }
    }
};
    
int main(int argc, char** argv) {
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

    // bind: "assigning a name to a socket"
    if (bind(sd, res->ai_addr, res->ai_addrlen) == -1) {
        std::cerr << "Error [bind]\n";
        return -1;
    }

    freeaddrinfo(res); //Liberar

    //+++++++++THREADS+++++++++

    std::thread pool[MAX_THREAD];
    for (int i = 0; 0 < MAX_THREAD; ++i) {
        MessageThread *mt = new MessageThread(sd);

        std::thread([&mt](){
            mt->do_message();
            delete mt;
        }).detach();
    }

    char command;
    while (true) {
        std::cin >> command;
        if (command == 'q') break;
    }
    close(sd); //Cerrar socket

    return 0;
}