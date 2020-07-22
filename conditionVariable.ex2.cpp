#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>

class Application{
    std::mutex mx;
    std::condition_variable cv;
    bool m_dataLoaded;
public:
    Application(): m_dataLoaded(false){}

    void loadData(){
        std::cout<<"Loading Data ..."<<"\n";
        std::this_thread::sleep_for(std::chrono::milliseconds(2000));
        std::lock_guard<std::mutex> lk(mx);
        m_dataLoaded = true;
        cv.notify_one();
    }

    bool isDataLoaded(){
        return m_dataLoaded;
    }

    void mainTask(){
        std::cout<<"Setting up Application Server..."<<"\n";
        std::unique_lock<std::mutex> lk(mx);

        cv.wait(lk, [this](){
            return isDataLoaded();
        });

        std::cout<<"Can start processing the loaded data ..."<<"\n";
        lk.unlock();
    }
};

int main(){
    Application app;
    std::thread th1(&Application::mainTask, &app);
    std::thread th2(&Application::loadData, &app);

    th1.join();
    th2.join();
}