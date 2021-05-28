#include "Serializable.h"

#include <iostream>
#include <string>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>

class Jugador: public Serializable
{
public:
    Jugador(const char * _n, int16_t _x, int16_t _y) : pos_x(_x), pos_y(_y)
    {
        strncpy(name, _n, MAX_NAME);
    };

    virtual ~Jugador() {};

    void to_bin()
    {
        int32_t data_size = MAX_NAME * sizeof(char) + 2 * sizeof(int16_t);
        alloc_data(data_size);

        char* tmp = _data;

        memcpy(tmp, name, MAX_NAME * sizeof(char)); //Copiar Name
        tmp += MAX_NAME * sizeof(char);
        memcpy(tmp, &pos_x, sizeof(int16_t));       //Copiar PosX
        tmp += sizeof(int16_t);
        memcpy(tmp, &pos_y, sizeof(int16_t));       //Copiar PosY
    }

    int from_bin(char * data)
    {
        char* tmp = data;

        memcpy(name, tmp, MAX_NAME * sizeof(char)); //Copiar Name
        tmp += MAX_NAME * sizeof(char);
        memcpy(&pos_x, tmp, sizeof(int16_t));       //Copiar PosX
        tmp += sizeof(int16_t);
        memcpy(&pos_y, tmp, sizeof(int16_t));       //Copiar PosY

        return 0;
    }

    void info() {
        std::cout << "Player: " << name << "  X: " << pos_x << "  Y: " << pos_y << '\n';
    }

private:
    static const size_t MAX_NAME = 20;
    char name[MAX_NAME];

    int16_t pos_x;
    int16_t pos_y;
};

int main(int argc, char **argv)
{
    Jugador one_r("", 0, 0);
    Jugador one_w("PlayerEj3", 80, 962);

    int fd = open("./data_jugador", O_CREAT | O_TRUNC | O_RDWR, 0666);
    if(fd == -1) {
        std::cout << "[Error] file open.\n";
        return -1;
    }

    //Serializar one_w y escribirlo en un fichero
    one_w.to_bin();
    int bytes = write(fd, one_w.data(), one_w.size());
    close(fd);
    if (bytes == -1) {
        std::cout << "[Error] write.\n";
        return -1;
    }

    fd = open("./data_jugador", O_RDONLY);
    if(fd == -1) {
        std::cout << "[Error] file open.\n";
        return -1;
    }

    //Leer de un fichero y deserializar en one_r
    char* buffer = (char*)malloc(one_w.size());
    bytes = read(fd, buffer, one_w.size());
    close(fd);
    if (bytes == -1) {
        std::cout << "[Error] read.\n";
        return -1;
    }
    one_r.from_bin(buffer);

    one_r.info(); //Mostar one_r

    return 0;
}