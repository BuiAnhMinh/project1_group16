#include <string>
#include <iostream>
#include <pthread.h>
#include <vector>
#include <cstdlib>

constexpr int TOTAL_ARG_COUNT = 4;
constexpr const char* SOURCE_FILE_TYPE  = ".txt";
constexpr const char* SOURCE_FILE_PREFIX = "source";
constexpr int MIN_THREAD_COUNT = 2;
constexpr int MAX_THREAD_COUNT = 10;

enum dir_type_t {
    SOURCE,
    DESTINATION
};

enum copy_result_t {
    COPY_FAILED = 0,
    COPY_SKIPPED = 1,
    COPY_SUCCESS = 2
};

struct directory_pair_t {
    std::string source_filename;
    std::string destination_filename;
    copy_result_t result;
};

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
        std::cerr << "Invalid number of arguments, must be " << TOTAL_ARG_COUNT << std::endl;
        std::cerr << "Usage: " << argv[0] << "<thread_count> <source_dir> <destination_dir>" << std::endl;
        exit(1);
    }

    // validate thread count within allowed range
    int thread_count;
    try {
        thread_count = std::stoi(argv[1]);
        if (thread_count < MIN_THREAD_COUNT || thread_count > MAX_THREAD_COUNT){
            std::cerr << "Invalid thread count, must be between 2 and 10" << std::endl;
            exit(1);
        }
    } catch (const std::exception& e){
        std::cerr << "Invalid thread count, must be an integer" << std::endl;
        exit(1);
    }

    // validate source directory exists otherwise no files to copy
    std::string source_dir = argv[2];
    if (!directory_exists(source_dir)){
        std::cerr << "Source directory does not exist" << std::endl;
        exit(1);
    }

    // create or validate destination directory exists
    std::string destination_dir = argv[3];
    try {
        std::filesystem::create_directories(destination_dir);
        if (!directory_exists(destination_dir)) {
            std::cerr << "Error creating destination directory" << std::endl;
            exit(1);
        }
    } catch (const std::exception& e) {
        std::cerr << "Error creating destination directory: " << e.what() << std::endl;
        exit(1);
    }

    // initialise threads with id's mapping to source filename
    std::vector<pthread_t> threads(thread_count);
    std::vector<directory_pair_t> thread_args(thread_count);

    // create all threads with required args
    for (int i = 0; i < thread_count; i++){
        std::string current_id = std::to_string(i + 1);
        std::string current_filename = SOURCE_FILE_PREFIX + current_id + SOURCE_FILE_TYPE;
        
        // protect against directories with "/" or "//"
        std::filesystem::path source_filename = std::filesystem::path(source_dir) / current_filename;
        std::filesystem::path destination_filename = std::filesystem::path(destination_dir) / current_filename;
        
        // create args for directory pair, keep accessible, create thread
        thread_args[i] = {source_filename.string(), destination_filename.string(), COPY_FAILED};
        int ret = pthread_create(&threads[i], NULL, copy_file, &thread_args[i]);
        if (ret != 0) {
            std::cerr << "Error creating thread " << i << ": " << ret << std::endl;
            exit(1);
        }
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

    // @TODO: remove this for final submission
    // delete destination directory after output printed
    try {
        std::filesystem::remove_all(destination_dir);
        std::cout << "Deleted destination directory: " << destination_dir << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "Error deleting destination directory: " << e.what() << std::endl;
    }

    return 0;
}