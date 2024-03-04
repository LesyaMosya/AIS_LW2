#include <iostream>
#include <iomanip>
#include <cstdlib>
#include <libconfig.h++>
#include "TextException.h"
#include <string.h>
#include <unistd.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <csignal>

#include "Server.h"
using namespace std;
using namespace libconfig;


int max_threads;
int port;
int file_max_size;
string save_path;

void load_config(const string &);
template<typename T>
T GetParam(Config&, const string &);

void sigtermHandler(int signum) {
    cout << "SIGTERM signal (" << signum << ") received.\n";
    exit(signum);
}

void sighupHandler(int signum) {
    cout << "SIGHUP signal (" << signum << ") received.\n";
    exit(signum);
}

int main(int argc, char *argv[])
{
    signal(SIGTERM, sigtermHandler);  
    signal(SIGHUP, sigtermHandler);  
    try {
        load_config("settings.cfg");
    }
    catch (const text_exception& text_ex) {
        std::cout << text_ex.what();
        return (40);
    }
    server myServer(port, max_threads, file_max_size, save_path);
    myServer.startServer();
    while(true) {
        sleep(1);
    }
    return 0;
}

void load_config(const string &configFilePath) {
    Config cfg;
    try
    {
        cfg.readFile(configFilePath.c_str());
    }
    catch(const FileIOException &fioex)
    {
        throw text_exception("I/O error while reading config file");
    }
    
    port = GetParam<int>(cfg, "port");
    max_threads = GetParam<int>(cfg, "threads");
    
    file_max_size = GetParam<int>(cfg, "file_max_size");
    save_path = GetParam<string>(cfg, "save_path");

    if(max_threads <= 0) {
        throw text_exception("value of 'threads' must be greater than zero");
    }
    if(port <= 0) {
        throw text_exception("value of 'port' must be greater than zero");
    }
    if(file_max_size <= 0) {
        throw text_exception("value of 'file_max_size' must be greater than zero");
    }
    
}

template<typename T>
T GetParam(Config& cfg, const string& param_name) {
    try
    {
        T param = cfg.lookup(param_name);
        cout << "Value of "<< param_name << " = " << param << endl;
        return param;
    }
    catch(const SettingNotFoundException &nfex)
    {
        std::stringstream ss;
        ss << "No '" << param_name <<"' setting in configuration file." << endl;
        throw text_exception(ss.str());
    }
    catch(const SettingTypeException &setTypeErr) {
        std::stringstream ss;
        ss << "'" << param_name <<"' is a bad type" << endl;
        throw text_exception(ss.str());
    }
}
