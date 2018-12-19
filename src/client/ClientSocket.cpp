#include "ClientSocket.h"

bool ClientSocket::TryLogin(int &socketFd, std::string userName)
{    
    Connecter(socketFd);
    size_t nrBytes = send(socketFd, userName.c_str(), userName.length(), 0);

    if (nrBytes != userName.length())
    {
        std::cout << "not everything is sent (" << nrBytes << "/" << userName.length() << " bytes sent)\n";
    }
    const int BufferSize = 100;
    char buffer[BufferSize];
    int incBytes = read(socketFd, buffer, BufferSize - 1);
    if (incBytes >= 0)
    {
        buffer[incBytes] = '\0';
        std::cout << "received " << incBytes << " bytes: " << buffer << std::endl;
    }

    if (!strcmp(buffer, "ACK"))
    {
        Disconnect(socketFd);
        return false;
    }
    return true;

}

void ClientSocket::Connecter (int &socketFd)
{
    socketFd = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);

    if (socketFd == -1)
    {
        perror("cannot create socket");
        exit(EXIT_FAILURE);
    }

    struct sockaddr_in sa;
    memset(&sa, 0, sizeof sa);
    sa.sin_family = AF_INET;
    sa.sin_port = htons(PortNumber);

    int result = inet_pton(AF_INET, "127.0.0.1", &sa.sin_addr);
    if (result != 1)
    {
        perror("could not convert ip address to network address structure");
        exit(EXIT_FAILURE);
    }
    else
    {
        result = connect(socketFd, (struct sockaddr*)&sa, sizeof sa);
        std::cout << result;
        if (result < 0)
        {
            perror("connect failed");
            close(socketFd);
            exit(EXIT_FAILURE);
        }
    }
}

void ClientSocket::Disconnect (int &socketFd)
{
    if (shutdown(socketFd, SHUT_RDWR) < 0)
    {
        perror("shutdown failed");
        close(socketFd);
        exit(EXIT_FAILURE);
    }
    close(socketFd);
}

void ClientSocket::Messenger (int &socketFd, Command cmd, std::string userName)
{
    
    std::string message;
    bool exit = false;
    std::cout << "Welcome to the messenger, type exit to leave \n";
    while(!exit)
    {
        std::getline(std::cin, message);
        std::cout << "your message was: " << message << std::endl;
        Connecter(socketFd); 
        if(message == "exit")
        {
            exit = true;
        }
        else
        {
            std::string temp = Command(cmd);
            message = userName + ", " + temp + ", " + message;
            size_t nrBytes = send(socketFd, message.c_str(), message.length(), 0);

            if (nrBytes != message.length())
            {
                std::cout << "not everything is sent (" << nrBytes << "/" << message.length() << " bytes sent)\n";
            }

            const int BufferSize = 100;
            char buffer[BufferSize];

            int incBytes = read(socketFd, buffer, BufferSize - 1);

            if (incBytes >= 0)
            {
                buffer[incBytes] = '\0';
                std::cout << "received " << incBytes << " bytes: " << buffer << std::endl;
            }
        }
        Disconnect(socketFd);
    }  
}



