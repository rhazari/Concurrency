#include <iostream>
#include <string>
#include <thread>
#include <future>
#include <vector>
#include <algorithm>
#include <filesystem>

namespace fs = std::filesystem;

struct Result{
    std::vector<std::string> files_;
    std::vector<fs::path> dirs_;
    Result(Result&& r): files_(std::move(r.files_)), dirs_(std::move(r.dirs_)){}
    Result() = default;
};

Result listDir(fs::path&& dir){
    Result result;
    for(auto it = fs::directory_iterator(dir); it != fs::directory_iterator(); ++it){
        if(it->is_directory()) {
            result.dirs_.push_back(it->path());
        }
        else{
            result.files_.push_back(it->path().string());
        }
    }
    return result;
}

int main() {
    fs::path root("/mnt/c/Users/Raihan");
    std::vector<fs::path> dirsToDo;
    dirsToDo.push_back(root);
    std::cout << "Current path is: " << fs::current_path() << '\n';

    std::vector<std::string> files;
    while(!dirsToDo.empty()){
       std::vector<std::future<Result>> futures;

       for(int k = 0; k < 16 && !dirsToDo.empty(); ++k){
           auto fut = std::async(&listDir, std::move(dirsToDo.back()));
           dirsToDo.pop_back();
           futures.push_back(std::move(fut));
       }
       try{
           while(!futures.empty()){
               auto ftr = std::move(futures.back());
               futures.pop_back();
               // This should invoke the move constructor
               Result result = ftr.get();
               std::copy(result.files_.begin(), result.files_.end(), std::back_inserter(files));
               std::copy(result.dirs_.begin(), result.dirs_.end(), std::back_inserter(dirsToDo));
           }
       }
       catch(...){
           std::cout<<"Unknown exception\n";
       }
    }
    std::for_each(files.begin(), files.end(), [](std::string& file){
       std::cout<<file<<"\n";
    });
}
