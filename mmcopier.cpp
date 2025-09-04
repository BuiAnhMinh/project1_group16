#include <string>
#include <iostream>
#include <pthread.h>
#include <vector>
#include <cstdlib>
#include <filesystem>
#include "file_utils.h"

constexpr int TOTAL_ARG_COUNT = 4;
constexpr const char* SOURCE_FILE_TYPE  = ".txt";
constexpr const char* SOURCE_FILE_PREFIX = "source";

struct directory_pair_t {
    std::string source_filename;
    std::string destination_filename;
    copy_result_t result;
};

int is_valid_thread_count(const std::string& thread_count_raw){
    try {
        int thread_count = std::stoi(thread_count_raw);
        return (thread_count >= 2 && thread_count <= 10) ? thread_count : -1;
    } catch (const std::exception& e){
        return -1;
    }
}

void* copy_file(void* directory_pair){
    if (directory_pair == NULL){
        return nullptr;
    }
    directory_pair_t* dirs = static_cast<directory_pair_t*>(directory_pair);
    std::string source_filename = dirs->source_filename;
    std::string destination_filename = dirs->destination_filename;
    
    // check file exists before copying
    if (!file_exists(source_filename)) {
        dirs->result = COPY_FAILED;
        return nullptr;
    }
    
    // skip copying if destination file already exists
    if (file_exists(destination_filename)) {
        dirs->result = COPY_SKIPPED;
        return nullptr;
    }
    
    try {
        std::filesystem::copy(source_filename, destination_filename);
        dirs->result = COPY_SUCCESS;
    } catch (const std::exception& e){
        dirs->result = COPY_FAILED;
    }
    return nullptr;
}

int main(int argc, char* argv[]){
    // validate correct argument count including function call
     if (argc != TOTAL_ARG_COUNT){
        std::cerr << "Usage: " << argv[0] 
                  << " <thread_count> <source_dir> <destination_dir>" << std::endl;
        exit(1);
    }

    // validate thread count within allowed range
    std::string thread_count_raw = argv[1];
    int thread_count = is_valid_thread_count(thread_count_raw);
    if (thread_count == -1){
        std::cerr << "Invalid thread count" << std::endl;
        exit(1);
    }

    // validate source directory exists otherwise no files to copy
    std::string source_dir = argv[2];
    if (!directory_exists(source_dir)){
        exit(1);
    }

    // validate or create destination directory
    std::string destination_dir = argv[3];
    if (!check_directory_or_create(destination_dir)){
        exit(1);
    }

    // initialise threads with id's mapping to source filename
    std::vector<pthread_t> threads(thread_count);
    std::vector<directory_pair_t> thread_args(thread_count);

    // create all threads with required args
    for (int i = 0; i < thread_count; i++){
        std::string current_id = std::to_string(i + 1);
        std::string current_filename = std::string("/") + SOURCE_FILE_PREFIX + current_id + SOURCE_FILE_TYPE;
        std::string source_filename = source_dir + current_filename;
        std::string destination_filename = destination_dir + current_filename;
        
        // create args for directory pair, keep accessible, create thread
        directory_pair_t current_arg = {source_filename, destination_filename, COPY_FAILED};
        thread_args[i] = current_arg;
        pthread_create(&threads[i], NULL, copy_file, &thread_args[i]);
    }

    // wait for all threads to complete 
    for (int i = 0; i < thread_count; i++){
        int ret = pthread_join(threads[i], NULL);
        if (ret == 0) {
            std::string status;
            switch (thread_args[i].result) {
                case COPY_SUCCESS:
                    status = "SUCCESS";
                    break;
                case COPY_SKIPPED:
                    status = "SKIPPED";
                    break;
                case COPY_FAILED:
                    status = "FAILED";
                    break;
            }
            std::cout << "Thread " << i << " " << status << ": " 
            << SOURCE_FILE_PREFIX << i + 1 << SOURCE_FILE_TYPE 
            << " from " << thread_args[i].source_filename << " to " 
            << thread_args[i].destination_filename << std::endl;
        }
    }
    return 0;
}