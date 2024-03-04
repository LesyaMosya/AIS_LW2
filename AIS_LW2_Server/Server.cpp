
#include "Server.h"

#include <fstream>
#include <string.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <iostream>
#include <stdio.h>
#include <unistd.h>

#include "ConcurentQueue.h"
void server::startServer() {
    int sockfd;
    struct sockaddr_in serv_addr;
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) 
        error("ERROR opening socket");
    memset((char *) &serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(port);
    if (bind(sockfd, (struct sockaddr *) &serv_addr,
                  sizeof(serv_addr)) < 0) 
        error("ERROR on binding");
    is_active = true;
   
    main_thread = std::thread(&server::mainActivity, this, sockfd);
    
}

void server::mainActivity(int mainSocketFd) {

    /*file_saver_consumer *cons {};*/
    consumers_ =static_cast<file_saver_consumer*>(operator new[] (max_threads * sizeof(file_saver_consumer)));
    for (int i =0; i < max_threads; i++) {
        new (consumers_ + i) file_saver_consumer(save_path, file_max_size, queue_);
    }
    
    int newsockfd;
    socklen_t addrLen;
    struct sockaddr_in cli_addr;
    listen(mainSocketFd,max_threads);
    addrLen= sizeof(cli_addr);
    while (is_active) {
        newsockfd = accept(mainSocketFd, (struct sockaddr *) &cli_addr, &addrLen);
        if (newsockfd < 0)
            error("ERROR on accept");
        std::cout << "pushed "<< newsockfd << std::endl;
        queue_.push(newsockfd);
    }
    close(mainSocketFd);
}

/*void server::socketProcessing(int socketfd) {
    char buffer[256];
    int n;
    memset(buffer, 0,256);
    
    n = read(socketfd,buffer,255);   //читаем имя файла
    std::ofstream wstream(save_path + buffer);
    int i = 0;
    do {
        memset(buffer, 0,256);
        n = read(socketfd,buffer,255);
        wstream << buffer;
        i++;
        if(i* 256 > file_max_size)
            break;
    } while (n>0);
    wstream.close();
    close(socketfd);
    {
        std::unique_lock<std::mutex> lock(local_mutex);
        active_threads--;
    }
}*/

void server::stopServer() {
    is_active = false;
    main_thread.join();
    for (int i =0; i <max_threads; i++) {
        consumers_[i].terminate_task();
    }
}



void server::error(const char *msg) {
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

packet read_packet(int socketfd) {
    char lengthBuffer[1];
    int n = read(socketfd,lengthBuffer,1);
    unsigned char length = lengthBuffer[0];
    n = read(socketfd,lengthBuffer,1);
    unsigned char type = lengthBuffer[0];
    char msgBuffer[length];
    n = read(socketfd, msgBuffer, length);
    return {type, length, msgBuffer};
}

void  file_saver_consumer::processing(conc_queue<int>& p_queue){
    while (!terminate) {
        if(!p_queue.empty()) {
            int socketfd = p_queue.pop();
            std::cout << "processing " << socketfd << std::endl;

            packet fileName = read_packet(socketfd);
            std::cout << (int)fileName.type << std::endl << (int)fileName.length << " " << fileName.text.length() << std::endl;
            if(fileName.type != 2)
                continue;
            std::cout << fileName.text << std::endl;
            std::ofstream wstream(save_path + fileName.text);
            int fileSize = 0;
            packet msgPacket = read_packet(socketfd);
            while (msgPacket.type != 1 && fileSize < max_file_size) {
                fileSize +=msgPacket.length;
                wstream << msgPacket.text << std::endl;
                //std::cout << msgPacket.text << std::endl;
                msgPacket = read_packet(socketfd);
            }
            
            wstream.close();
        }
        else {
            std::this_thread::sleep_for(std::chrono::milliseconds(1000));
        }
    }
}

void file_saver_consumer::terminate_task() {
    terminate = true;
    thread.join();
}

