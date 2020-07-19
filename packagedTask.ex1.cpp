#include <iostream>
#include <thread>
#include <future>
#include <deque>

int factorial(int N){
    int result = 1;
    for(int k = 2; k <= N; ++k){
        result *= k;
    }
    return result;
}

std::deque<std::packaged_task<int(int)>> taskQueue;
std::mutex mu;
std::condition_variable cv;

void func(){
    std::packaged_task<int(int)> task;
    {
        std::unique_lock<std::mutex> lock(mu);
        cv.wait(lock, []() {
            return !taskQueue.empty();
        });
        task = std::move(taskQueue.front());
        taskQueue.pop_front();
    }
    task(6);
}

int main() {
    std::thread th(func);
    std::packaged_task<int(int)> task(factorial);
    std::future<int> f = task.get_future();
    {
        std::lock_guard<std::mutex> lock(mu);
        taskQueue.push_back(move(task));
    }
    cv.notify_one();
    std::cout<<"Value of the factorial: "<< f.get()<<"\n";
    th.join();
}