#include <iostream>
#include <queue>
#include <thread>
#include <condition_variable>
#include <mutex>
#include <vector>
#include <sstream>

const int nbConsumers = 4;
const int nbToConsume = 3;
const int nbToProduce = nbToConsume * nbConsumers;

template <typename T>
class Queue{
    private:
        std::queue<T> queue_;
        std::mutex mx_;
        std::condition_variable cv_;
        size_t size_ = 0;
    
    public:
        T front(){
            std::unique_lock<std::mutex> lock(mx_);
            cv_.wait(lock, [this]{
                return !queue_.empty();
            });
            auto elem = queue_.front();
            lock.unlock();
            return elem;
        }

        void pop() {
            std::unique_lock<std::mutex> lock(mx_);
            cv_.wait(lock, [this]{
                return !queue_.empty();
            });

            queue_.pop();
            lock.unlock();
        }

        void push(const T& elem){
            std::unique_lock<std::mutex> lock(mx_);
            queue_.push(elem);
            ++size_;
            lock.unlock();
            cv_.notify_one();
        }

        void push(T&& elem){
            std::unique_lock<std::mutex> lock(mx_);
            queue_.push(std::move(elem));
            ++size_;
            lock.unlock();
            cv_.notify_one();
        }

        size_t size() const {
            return size_;
        }
};

void print(std::string x) {
  static std::mutex mutex;
  std::unique_lock<std::mutex> locker(mutex);
  std::cout << x << "\n";
}

void produce(Queue<int>& q) {
  for (int i = 1; i <= nbToProduce; ++i) {
    std::ostringstream tmp;
    tmp << "--> " << i;
    print(tmp.str());
    q.push(i);
  }
}

void consume(Queue<int>& q, unsigned int id) {
  for (int i = 0; i < nbToConsume; ++i) {
    auto item = q.front();
    q.pop();
    std::ostringstream tmp;
    tmp << "        " << item << " --> C" << id;
    print(tmp.str());
  }
}

int main(){
    Queue<int> Q;
    // Start the producer thread.
    std::thread prod1(std::bind(produce, std::ref(Q)));

    // Start the consumer threads.
    std::vector<std::thread> consumers;
    for (int i = 0 ; i < nbConsumers ; ++i) {
        std::thread consumer(std::bind(&consume, std::ref(Q), i + 1));
        consumers.push_back(std::move(consumer));
    }

    prod1.join();

    for (auto& consumer : consumers) {
        consumer.join();
    }

}