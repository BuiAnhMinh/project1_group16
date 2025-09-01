#include <string>
#include <stdexcept>
#include <string>
#include <iostream>
#include <fstream> 
#include <filesystem>
#include <pthread>

const int TOTAL_ARG_COUNT = 4
const int FILE_ARGS = 2

int is_valid_thread_count(const std::string& thread_count_raw){
    if (thread_count_raw.size() > 2){
        return -1;
    }
    try {
        int thread_count = std::stoi(thread_count_raw);
        return (thread_count >= 2 && thread_count <= 10) ? thread_count : -1;
    } catch (const std::exception& e){
        return -1;
    }
}

int main(int argc, char* argv[]){
    // validate correct argument count including function call
     if (argc != REQUIRED_ARG_COUNT){
        std::cerr << "Usage: " << argv[0] 
                  << " <thread_count> <source_dir> <destination_dir>" << std::endl;
        return EXIT_FAILURE;
    }

    // validate thread count within allowed range
    std::string thread_count_raw = argv[1];
    int thread_count = is_valid_thread_count(thread_count_raw)
    if (thread_count == -1){
        std::cerr << "Invalid thread count" << std::endl;
        return EXIT_FAILURE;
    }

    std::string source_dir = argv[2];
    std::string destination_dir = argv[3];

    // initialise threads with id's mapping to source filename
    std::vector<pthread_t> pthreads(thread_count);
    for (int i = 0; i < thread_count; i++){
        std::string source_filename = "task1/source_dir/source" + std::to_string(i) + ".txt";
    }
    return EXIT_SUCCESS;
}



    



