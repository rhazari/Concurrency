#include <iostream>
#include <thread>
#include <atomic>

constexpr int LIMIT = 100;
int val = 1;

std::atomic<int> atomic_val{1};

void printEvenWithAtomic(){
    while(atomic_val <= LIMIT){
        if(atomic_val % 2 == 0 && atomic_val <= LIMIT){
            std::cout<<atomic_val<<" ";
            atomic_val.fetch_add(1, std::memory_order_relaxed);
        }
    }
}

void printOddWithAtomic(){
    while(atomic_val <= LIMIT){
        if(atomic_val % 2 == 1 && atomic_val <= LIMIT){
            std::cout<<atomic_val<<" ";
            atomic_val.fetch_add(1, std::memory_order_relaxed);
        }
    }
}

int main(){
    std::cout<<"Sync with atomics\n";
    const auto start2 = std::chrono::steady_clock::now();
    std::thread th3(printEvenWithAtomic);
    std::thread th4(printOddWithAtomic);

    th3.join();
    th4.join();
    const std::chrono::duration<double> duration2 = std::chrono::steady_clock::now() - start2;
    std::cout<<"\nTime elapsed " << duration2.count() << " seconds" << std::endl;
    std::cout<<"\n";

    return 0;
}