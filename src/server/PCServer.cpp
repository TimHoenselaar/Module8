#include "../protocol.h"

#include <arpa/inet.h>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <unistd.h>

int main(void)
{
    while(true)
    {
        int socketFd = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
        std::cout << socketFd;
        if (socketFd < 0)
        {
            perror("cannot create socket");
            exit(EXIT_FAILURE);
        }

        struct sockaddr_in sa;
        memset(&sa, 0, sizeof sa);
        sa.sin_family = AF_INET;
        sa.sin_port = htons(PortNumber);
        sa.sin_addr.s_addr = htonl(INADDR_ANY);

        if (bind(socketFd, (struct sockaddr*)&sa, sizeof sa) < 0)
        {
            perror("bind failed");
            close(socketFd);
            exit(EXIT_FAILURE);
        }

        if (listen(socketFd, 10) < 0)
        {
            perror("listen failed");
            close(socketFd);
            exit(EXIT_FAILURE);
        }

        int connectFd = accept(socketFd, NULL, NULL);

        if (connectFd < 0)
            {
                perror("accept failed");
                close(socketFd);
                exit(EXIT_FAILURE);
            }
        std::string message;

        bool connecting = false;
        while (!connecting)
        {
            const int BufferSize = 100;
            char buffer[BufferSize];
            std::string ack = "ACK";
            int nrBytes = read(connectFd, buffer, BufferSize - 1);
            if (nrBytes > 0)
            {
                buffer[nrBytes] = '\0';
                std::cout << "received " << nrBytes << " bytes: " << buffer << std::endl;
                size_t nrBytes = send(connectFd, ack.c_str(), ack.length(), 0);            
            }
            else if(nrBytes == 0)
            {
                connecting = true;
                close(socketFd);
            }
        }

        if (shutdown(connectFd, SHUT_RDWR) < 0)
        {
            perror("shutdown failed");
            close(connectFd);
            close(socketFd);
            exit(EXIT_FAILURE);
        }
    //close(connectFd);
    }
    return EXIT_SUCCESS;
}