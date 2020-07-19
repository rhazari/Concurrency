#include <iostream>
#include <thread>
#include <functional>
#include <mutex>
#include <queue>
#include <condition_variable>

template <typename T>
class ProducerConsumer{
    std::condition_variable cv_;
    std::mutex mx_;
    std::queue<T> queue_;
    size_t size_;
public:
    ProducerConsumer(int size): size_(size){}

    void add(T var){
        std::unique_lock<std::mutex> lock(mx_);
        cv_.wait(lock, [this](){
            return !isFull();
        });
        queue_.push(var);
        lock.unlock();
        cv_.notify_all();
    }

    int consume(){
        std::unique_lock<std::mutex> lock(mx_);
        cv_.wait(lock, [this](){
            return !isEmpty();
        });
        auto var = queue_.front();
        queue_.pop();
        lock.unlock();
        cv_.notify_all();
        return var;
    }

    bool isFull(){
        return queue_.size() >= size_;
    }

    bool isEmpty(){
        return queue_.size() == 0;
    }

    size_t length(){
        return queue_.size();
    }

    void clear(){
        std::unique_lock<std::mutex> lock(mx_);
        while(!isEmpty()){
            queue_.pop();
        }
        lock.unlock();
        cv_.notify_all();
    }
};

template <typename T>
void func1(ProducerConsumer<T> &PC){
    std::cout<<"Producing ..."<<"\n";
    for(int k = 0; k < 5; ++k){
        PC.add(k*k+1);
    }
}

template <typename T>
void func2(ProducerConsumer<T> &PC){
    std::this_thread::sleep_for(std::chrono::milliseconds(2000));
    std::cout<<"Consuming ..."<<"\n";
    while(!PC.isEmpty()){
        std::cout<<PC.consume()<<"\n";
    }
}

int main(){
    ProducerConsumer<int> PC(5);

    std::thread th1(func1<int>, std::ref(PC));
    std::thread th2(func2<int>, std::ref(PC));

    th1.join();
    th2.join();
}