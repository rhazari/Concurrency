#include <iostream>
#include <mutex>
#include <condition_variable>
#include <thread>
#include <chrono>

std::condition_variable cv;
std::mutex mx;

int data = 0;

void wait(){
    std::unique_lock<std::mutex> lk(mx);
    std::cout<<"Waiting for signal from main()"<<"\n";
    cv.wait(lk, []{
        return data == 1;
    });
    std::cout<<"Done waiting ..."<<"\n";
    lk.unlock();
}

void signal(){
    std::this_thread::sleep_for(std::chrono::seconds(1));
    {
        std::lock_guard<std::mutex> lk(mx);
        std::cout<<"Notifying..."<<"\n";
    }
    cv.notify_all();
    std::this_thread::sleep_for(std::chrono::seconds(1));
    {
        std::lock_guard<std::mutex> lk(mx);
        data = 1;
        std::cout<<"Notifying again ..."<<"\n";
    }
    cv.notify_all();
}

int main(){

    std::thread t1(wait), t2(wait), t3(wait), t4(signal);

    t1.join();
    t2.join();
    t3.join();
    t4.join();
}
