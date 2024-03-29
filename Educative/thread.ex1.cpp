// coutSynchronised.cpp

#include <chrono>
#include <iostream>
#include <mutex>
#include <thread>

std::mutex coutMutex;

class Worker{
public:
    Worker(const std::string n):_name(n){
    };
 
    void operator() (){
      for (int i = 1; i <= 3; ++i){ 
        // begin work
        std::this_thread::sleep_for(std::chrono::milliseconds(200));
        // end work
        coutMutex.lock();
        std::cout << _name << ": " << "Work " << i << " done !!!" << std::endl;
        coutMutex.unlock();
      }
    }
private:
  std::string _name;
};


int main(){

  std::cout << std::endl;
  
  std::cout << "Boss: Let's start working." << "\n\n";
 
  std::thread herb= std::thread(Worker("Herb"));
  std::thread andrei= std::thread(Worker("Andrei"));
  std::thread scott= std::thread(Worker("Scott"));
  std::thread bjarne= std::thread(Worker("Bjarne"));
  std::thread bart= std::thread(Worker("Bart"));
  std::thread jenne= std::thread(Worker("Jenne"));
  
  herb.join();
  andrei.join();
  scott.join();
  bjarne.join();
  bart.join();
  jenne.join();
  
  std::cout << "\n" << "Boss: Let's go home." << std::endl;
  
  std::cout << std::endl;

}