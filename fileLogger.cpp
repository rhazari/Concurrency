#include <iostream>
#include <thread>
#include <mutex>
#include <fstream>

class LogFile {
    std::mutex mu_;
    std::once_flag flag_;
    std::ofstream f_;

public:
    LogFile(){}

    void print_msg(std::string msg, int id){
        // File will be opened only once by one of the threads
        std::call_once(flag_, [&](){
            f_.open("logFile.txt");
        });

        std::unique_lock<std::mutex> locker(mu_, std::defer_lock);
        locker.lock();
        f_ << msg <<"->"<<id<<"\n";
        locker.unlock();
    }
};

void func(LogFile& log){
    for(int k = 0; k < 100; ++k){
        log.print_msg("From thread", k+1);
    }
}

int main(){
    LogFile log;
    std::thread t1(func, std::ref(log));

    for(int k = 0; k < 100; ++k)
        log.print_msg("From main", k+1);

    t1.join();
}