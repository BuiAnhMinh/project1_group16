#include <iostream>
#include <string>
#include <stdexcept>
#include <string>

int isValidThreadCount(const std::string& threadCountRaw){
    if (threadCountRaw.size() > 2){
        return -1;
    }
    try {
        int threadCount = std::stoi(threadCountRaw);
        return (threadCount >= 2 && threadCount <= 10) ? threadCount : -1;
    } catch (const std::exception& e){
        return -1;
    }
}

int main(int argc, char* argv[]){
     if (argc != 4){
        std::cerr << "Usage: " << argv[0] 
                  << " <thread_count> <source_dir> <destination_dir>" << std::endl;
        return 1;
    }

    std::string threadCountRaw = argv[1];
    std::string sourceDir = argv[2];
    std::string destinationDir = argv[3];
    return 0;
}




    



