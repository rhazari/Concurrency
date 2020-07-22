#include <condition_variable>
#include <functional>
#include <vector>
#include <future>
#include <thread>
#include <queue>
#include <iostream>

class MemoryPool {
public:
    using Task = std::function<void()>;

    explicit MemoryPool(std::size_t numThreads) {
        start(numThreads);
    }

    ~MemoryPool() {
        stop();
    }

    template<class T>
    auto enqueue(T task) -> std::future<decltype(task())> {
        auto wrapper = std::make_shared<std::packaged_task<decltype(task())()>>(std::move(task));
        {
            std::unique_lock<std::mutex> lock{ m_mutex };
            m_queue.emplace([=] {
                (*wrapper)();
            });
        }
        m_eventVar.notify_one();
        return wrapper->get_future();
    }

private:

    std::vector<std::thread> m_threads;
    std::condition_variable m_eventVar;
    std::mutex m_mutex;
    std::queue<Task> m_queue;
    bool stopFlag = false;

    void start(std::size_t numThreads) {
        for (auto k = 0; k < numThreads; ++k) {
            m_threads.emplace_back([=] {
                while (true) {
                    Task task;
                    {
                        std::unique_lock<std::mutex> lock{ m_mutex };
                        m_eventVar.wait(lock, [=] {
                            return stopFlag || !m_queue.empty();
                        });

                        if (stopFlag && m_queue.empty())
                            break;

                        task = m_queue.front();
                        m_queue.pop();
                    }
                    task();
                }
            });
        }
    }

    void stop() {
        {
            std::unique_lock<std::mutex> lock{ m_mutex };
            stopFlag = true;
        }
        m_eventVar.notify_all();

        for (auto& thread : m_threads) {
            thread.join();
        }
    }

};

int main() {
    MemoryPool pool{ 4 };
    auto f1 = pool.enqueue([] {
        return 1;
    });

    auto f2 = pool.enqueue([] {
        return 2;
    });

    std::cout << f1.get() + f2.get() << "\n";

    return 0;
}