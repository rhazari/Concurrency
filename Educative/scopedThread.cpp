#include <iostream>
#include <thread>
#include <utility>

/*
ScopedThread gets a thread t in its constructor and checks if t is still joinable. If the thread t passed into the constructor is not joinable anymore, there is no need for the scoped_thread. If t is joinable, the destructor calls t.join(). Because the copy constructor and copy assignment operator are declared as delete, instances of scoped_thread can not be copied to or assigned from.
*/

class ScopedThread{
    std::thread t;
public:
    explicit ScopedThread(std::thread t_): t(std::move(t_)){
        if (!t.joinable()) 
            throw std::logic_error("No thread");
    }

    ~ScopedThread(){
        t.join();
    }

    ScopedThread (ScopedThread&)= delete;
    ScopedThread& operator=(ScopedThread const &)= delete;
};