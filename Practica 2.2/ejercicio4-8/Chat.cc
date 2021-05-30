#include "Chat.h"

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

void ChatMessage::to_bin()
{
    alloc_data(MESSAGE_SIZE);
    memset(_data, 0, MESSAGE_SIZE);

    char *tmp = _data;

    memcpy(tmp, &type, sizeof(uint8_t));
    tmp += sizeof(uint8_t);
    memcpy(tmp, nick.c_str(), 8 * sizeof(char));
    tmp += 8 * sizeof(char);
    memcpy(tmp, message.c_str(), 80 * sizeof(char));
}

int ChatMessage::from_bin(char * bobj)
{
    alloc_data(MESSAGE_SIZE);
    memcpy(static_cast<void *>(_data), bobj, MESSAGE_SIZE);

    char *tmp = _data;

    memcpy(&type, tmp, sizeof(uint8_t));
    tmp += sizeof(uint8_t);
    nick = tmp;
    tmp += 8 * sizeof(char);
    message = tmp;

    return 0;
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

void ChatServer::do_messages()
{
    while (true)
    {
        ChatMessage msg;
        Socket *client = new Socket(socket);
        socket.recv(msg, client);

        std::unique_ptr<Socket> ptrClient(client);

        if (msg.type == 0) { //LOGIN
            clients.push_back(std::move(ptrClient));
        }
        else if (msg.type == 1) { //MESSAGE
            for (auto it = clients.begin(); it != clients.end(); ++it) {
                if (!(**it == *ptrClient))
                    socket.send(msg, **it);
            }
        }
        else if (msg.type == 2) { //LOGOUT
            auto it = clients.begin();
            while (it != clients.end()) {
                if (**it == *ptrClient) {
                    it = clients.erase(it);
                    std::cout << "[LOGOUT]: " << msg.nick << '\n';
                    break;
                }
                else ++it;
            }
        }
    }
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

void ChatClient::login()
{
    std::string msg;

    ChatMessage em(nick, msg);
    em.type = ChatMessage::LOGIN;

    socket.send(em, socket);
}

void ChatClient::logout()
{
    std::string msg;

    ChatMessage em(nick, msg);
    em.type = ChatMessage::LOGOUT;

    socket.send(em, socket);
}

void ChatClient::input_thread()
{
    while (true)
    {
        std::string msg;
        std::getline(std::cin, msg);

        ChatMessage em(nick, msg);
        em.type = ChatMessage::MESSAGE;

        socket.send(em, socket);
    }
}

void ChatClient::net_thread()
{
    while(true)
    {
        ChatMessage em;
        socket.recv(em);

        if (em.nick != nick && em.nick != "") { //Para no mostrar mensajes propios
            std::cout << em.nick << ": " << em.message << '\n';
        }
    }
}

