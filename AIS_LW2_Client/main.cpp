#include <fstream>
#include <iostream>
#include <utility>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 

#define msgType 0
#define endType 1
#define filenameType 2


void error(const char *msg) {
    perror(msg);
    exit(1);
}

struct packet {
public:
    packet(const unsigned char type, const unsigned char length, std::string msg): length(length),text(std::move(msg)), type(type){}
    unsigned char length;
    std::string text;
    unsigned char type;
};

void sendPacket(packet &packet, int socketfd) {
    char lengthBuffer[1];
    lengthBuffer[0] = packet.length;
    int n = write(socketfd, lengthBuffer, 1);
    lengthBuffer[0] = packet.type;
    n = write(socketfd, lengthBuffer, 1);
    char messageBufer[packet.length];
    strcpy(messageBufer, packet.text.c_str());
    n = write(socketfd, messageBufer, packet.length);
}

int main(int argc, char *argv[])
{
    int sockfd, portno, n;
    struct sockaddr_in serv_addr;
    struct hostent *server;
    std::string filePath;
    char buffer[256];
    if (argc < 4) {
        fprintf(stderr,"usage %s hostname port\n", argv[0]);
        exit(0);
    }
    portno = atoi(argv[2]);
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) 
        error("ERROR opening socket");
    server = gethostbyname(argv[1]);
    if (server == NULL) {
        fprintf(stderr,"ERROR, no such host\n");
        exit(0);
    }
    memset((char *) &serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr, 
         (char *)&serv_addr.sin_addr.s_addr,
         server->h_length);
    serv_addr.sin_port = htons(portno);
    if (connect(sockfd,(struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0) 
        error("ERROR connecting");
    filePath = argv[3];

    if(filePath.length() > 256) {
        error("Too long file name");
    }
    packet fileName(2,filePath.length() +1, filePath);
    std::cout << (int)fileName.type << std::endl;
    sendPacket(fileName, sockfd);
    std::ifstream rStream(filePath);
    std::string line;
    while (std::getline(rStream, line)) {
        packet linePacket(0,line.length(), line);
        sendPacket(linePacket, sockfd);
    }
    packet endPacket(1, 1,"\0");
    sendPacket(endPacket, sockfd);
    
    
    /*strcpy(buffer, filePath.c_str());
    std::cout << buffer << std::endl;
    n = write(sockfd,buffer,strlen(buffer));
    if (n < 0) 
        error("ERROR writing to socket");
    memset(buffer, 0,256);*/

    /*
    std::string line;
    std::ifstream rStream(filePath);
    while (std::getline(rStream, line)) {
        int i = 0;
        do {
            std::string substr= line.substr(i*256,(i+1)*256);
            strcpy(buffer, substr.c_str());
            i++;
            std::cout << buffer << std::endl;
            n = write(sockfd,buffer,strlen(buffer));
            if (n < 0) 
                error("ERROR writing to socket");
        }while (i*255 < line.length());
    }
    */
    
    
    std::cout << "file was uploaded" << std::endl;
    close(sockfd);
    return 0;
}
