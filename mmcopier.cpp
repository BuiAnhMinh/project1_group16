#include <string>
#include <stdexcept>
#include <iostream>
#include <filesystem>
#include <pthread.h>
#include <vector>
#include <cstdlib>

const int TOTAL_ARG_COUNT = 4;
const std::string SOURCE_FILE_TYPE = ".txt";
const std::string SOURCE_FILE_PREFIX = "source";

enum dir_type_t {
    SOURCE,
    DESTINATION
};

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

bool directory_exists(std::string dir){
    if (std::filesystem::exists(dir) && std::filesystem::is_directory(dir)){
        return true;
    } else {
        std::cerr << "Directory does not exist: " << dir << std::endl;
        return false;
    }
}

bool file_exists(std::string file_path){
    if (std::filesystem::exists(file_path) && std::filesystem::is_regular_file(file_path)){
        return true;
    } else {
        std::cerr << "File does not exist: " << file_path << std::endl;
        return false;
    }
}

bool is_valid_directory(const std::string& dir, dir_type_t dir_type){
    try {
        if (!directory_exists(dir) && dir_type == SOURCE) {
            std::cerr << dir_type << " does not exist or is not a directory" << std::endl;
            return false;
        } else if (!directory_exists(dir) && dir_type == DESTINATION){
            std::filesystem::create_directories(dir);
        }
    } catch (const std::exception& e) {
        std::cerr << dir_type << " directory error: " << e.what() << std::endl;
        return false;
    }
    return true;
}

void* copy_file(void* directory_pair){
    if (directory_pair == NULL){
        std::cerr << "Invalid NULL directory pair passed to thread" << std::endl;
        return nullptr;
    }
    directory_pair_t* dirs = static_cast<directory_pair_t*>(directory_pair);
    std::string source_filename = dirs->source_filename;
    std::string destination_filename = dirs->destination_filename;
    
    // check file exists before copying
    if (!file_exists(source_filename)) {
        return nullptr;
    }
    
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
    if (!is_valid_directory(source_dir, SOURCE) || 
        !is_valid_directory(destination_dir, DESTINATION)){
        return EXIT_FAILURE;
    }

    // initialise threads with id's mapping to source filename
    std::vector<pthread_t> threads(thread_count);
    std::vector<directory_pair_t> thread_args(thread_count);

    // create all threads with required args
    for (int i = 0; i < thread_count; i++){
        std::string current_id = std::to_string(i);
        std::string current_filename = "/" + SOURCE_FILE_PREFIX + current_id + SOURCE_FILE_TYPE;
        std::string source_filename = source_dir + current_filename;
        std::string destination_filename = destination_dir + current_filename;
        
        // create args for directory pair, keep accessible, create thread
        directory_pair_t current_arg = {source_filename, destination_filename};
        thread_args[i] = current_arg;
        pthread_create(&threads[i], NULL, copy_file, &thread_args[i]);
    }

    // wait for all threads to complete 
    for (int i = 0; i < thread_count; i++){
        int ret = pthread_join(threads[i], NULL);
        if (ret == 0) {
            std::cout << "Thread " << i << " completed successfully" << std::endl;
        } else {
            std::cerr << "Error on thread " << i << ": " << ret << std::endl;
        }
    }
    return EXIT_SUCCESS;
}



