#include <string>
#include <stdexcept>
#include <iostream>
#include <fstream> 
#include <filesystem>
#include <pthread.h>
#include <vector>
#include <cstdlib>

const int TOTAL_ARG_COUNT = 4;

struct directory_pair_t {
    std::string source_filename;
    std::string destination_filename;
};

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

bool is_valid_directory(const std::string& dir, const std::string& dir_type){
    try {
        if (!std::filesystem::is_directory(dir)) {
            std::cerr << dir_type << " does not exist or is not a directory" << std::endl;
            return 0;
        }
    } catch (const std::exception& e) {
        std::cerr << dir_type << " directory error: " << e.what() << std::endl;
        return 0;
    }
    return 1;
}

void* copy_file(void* directory_pair){
    if (directory_pair == NULL){
        std::cerr << "Invalid NULL directory pair passed to thread" << std::endl;  
    }
    directory_pair_t* dirs = static_cast<directory_pair_t*>(directory_pair);
    std::string source_filename = dirs->source_filename;
    std::string destination_filename = dirs->destination_filename;
    try {
        std::filesystem::copy(source_filename, destination_filename);
    } catch (const std::exception& e){
        std::cerr << "Error copying file: " << e.what() << std::endl;
    }
    return nullptr;
}

int main(int argc, char* argv[]){
    // validate correct argument count including function call
     if (argc != TOTAL_ARG_COUNT){
        std::cerr << "Usage: " << argv[0] 
                  << " <thread_count> <source_dir> <destination_dir>" << std::endl;
        return EXIT_FAILURE;
    }

    // validate thread count within allowed range
    std::string thread_count_raw = argv[1];
    int thread_count = is_valid_thread_count(thread_count_raw);
    if (thread_count == -1){
        std::cerr << "Invalid thread count" << std::endl;
        return EXIT_FAILURE;
    }

    // validate source and destination directories exist and are directories
    std::string source_dir = argv[2];
    std::string destination_dir = argv[3];
    if (!is_valid_directory(source_dir, "Source") || 
        !is_valid_directory(destination_dir, "Destination")){
        return EXIT_FAILURE;
    }

    // initialise threads with id's mapping to source filename
    std::vector<pthread_t> pthreads(thread_count);
    for (int i = 0; i < thread_count; i++){
        std::string source_filename = source_dir + std::to_string(i) + ".txt";
        std::string destination_filename;
        //pthread_create(&pthreads[i], NULL, your_thread_function, thread_args);
        pthread_create()
                
        // pthread_create(&thread_id, NULL, thread_function, thread_args);
    }
    return EXIT_SUCCESS;
}



    



