
#ifndef SERVER_H
#define SERVER_H
#include <iostream>
#include <utility>
#include <thread>
#include <mutex>
#include <sys/socket.h>
#include <thread>
#include <utility>
#include "ConcurentQueue.h"



class file_saver_consumer {
public:
    file_saver_consumer(std::string savePath, int maxFileSize, conc_queue<int>& p_queue): save_path(std::move(savePath)), max_file_size(maxFileSize) {
        terminate = false;
        thread = std::thread(&file_saver_consumer::processing, this, std::ref(p_queue));
    }
    void terminate_task();
private:
    std::thread thread;
    bool terminate;
    std::string save_path;
    int max_file_size;
    void processing(conc_queue<int>&);
};


class server {
public:
    server(const int port, const int maxThreads,
        const int fileMaxSize, std::string savePath):port(port), max_threads(maxThreads),
    file_max_size(fileMaxSize), save_path(std::move(savePath))
    {
        is_active = false;
    }
    void startServer();
    void stopServer();
    int file_max_size;
    std::string save_path;
    ~server() {
        delete[] consumers_;
    }
private:
    int port;
    int max_threads;
    file_saver_consumer* consumers_;
    std::thread main_thread;
    conc_queue<int> queue_;
    bool is_active;
    void mainActivity(int);
    /*void socketProcessing(int);
    std::mutex local_mutex;
    int active_threads;*/

    static void error(const char *);
   
};



#endif //SERVER_H
