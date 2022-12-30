#include <iostream>
#include <thread>
#include <condition_variable>
#include <mutex>
#include <atomic>

std::mutex mx;
std::condition_variable cv;

constexpr int LIMIT = 100;
int val = 1;

std::atomic<int> atomic_val(1);

void printEven(){
    while(val <= LIMIT){
        std::unique_lock<std::mutex> lk(mx);
        cv.wait(lk, []{
            return val % 2 == 0;
        });
        if(val <= LIMIT){
            std::cout<<val<<" ";
        }
        val++;
        lk.unlock();
        cv.notify_all();
    }
}

void printOdd(){
     while(val <= LIMIT){
        std::unique_lock<std::mutex> lk(mx);
        cv.wait(lk, []{
            return val % 2 == 1;
        });
        if(val <= LIMIT){
            std::cout<<val<<" ";

        }
        val++;
        lk.unlock();
        cv.notify_all();
    }
}

void printEvenWithAtomic(){
    while(atomic_val <= LIMIT){
        if(atomic_val % 2 == 0 && atomic_val <= LIMIT){
            std::cout<<atomic_val<<" ";
            atomic_val++;
        }
    }
}

void printOddWithAtomic(){
    while(atomic_val <= LIMIT){
        if(atomic_val % 2 == 1 && atomic_val <= LIMIT){
            std::cout<<atomic_val<<" ";
            atomic_val++;
        }
    }
}

int main(){
    std::cout<<"Sync with locks\n";
    const auto start1 = std::chrono::steady_clock::now();
    std::thread th1(printEven);
    std::thread th2 (printOdd);

    th1.join();
    th2.join();
    const std::chrono::duration<double> duration1 = std::chrono::steady_clock::now() - start1;
    std::cout<<"\nTime for mySumition " << duration1.count() << " seconds" << std::endl;
    std::cout<<"\n";

    std::cout<<"Sync with atomics\n";
    const auto start2 = std::chrono::steady_clock::now();
    std::thread th3(printEvenWithAtomic);
    std::thread th4(printOddWithAtomic);

    th3.join();
    th4.join();
    const std::chrono::duration<double> duration2 = std::chrono::steady_clock::now() - start2;
    std::cout<<"\nTime for mySumition " << duration2.count() << " seconds" << std::endl;
    std::cout<<"\n";

    return 0;
}