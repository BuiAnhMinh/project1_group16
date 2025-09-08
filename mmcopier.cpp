#include <string>
#include <iostream>
#include <pthread.h>
#include <vector>
#include <cstdlib>
#include <fstream>
#include <sys/stat.h>

constexpr int TOTAL_ARG_COUNT = 4;
constexpr const char* SOURCE_FILE_TYPE  = ".txt";
constexpr const char* SOURCE_FILE_PREFIX = "source";
constexpr int MIN_THREAD_COUNT = 2;
constexpr int MAX_THREAD_COUNT = 10;
constexpr char FORWARD_SLASH = '/';

enum copy_result_t {
    NOT_COPIED,
    SOURCE_NOT_FOUND,
    SKIPPED_FILE_ALREADY_EXISTS,
    COPY_SUCCESS,
    COPY_ERROR
};

struct directory_pair_t {
    std::string source_filename;
    std::string destination_filename;
    copy_result_t result;
};

bool file_exists(const std::string& file_path){
    return std::ifstream(file_path).good();
}

bool directory_exists(const std::string& path){
    struct stat sb;
    return (stat(path.c_str(), &sb) == 0 && S_ISDIR(sb.st_mode));
}

// normalise dir to have SLASH at end
std::string normalise_dir(std::string path){
    if (path.empty()) return {};
    if (path == "/") return path;
    char last_char = path.back();
    return (last_char == FORWARD_SLASH) ? path : path += FORWARD_SLASH;
}

void* copy_file(void* directory_pair){
    if (directory_pair == NULL) return nullptr;

    directory_pair_t* dirs = static_cast<directory_pair_t*>(directory_pair);
    std::string source_filename = dirs->source_filename;
    std::string destination_filename = dirs->destination_filename;
    
    // check file exists before copying
    if (!file_exists(source_filename)) {
        dirs->result = SOURCE_NOT_FOUND;
        return nullptr;
    }

    // skip copying if destination file already exists at dest
    if (file_exists(destination_filename)){
        dirs->result = SKIPPED_FILE_ALREADY_EXISTS;
        return nullptr;
    }
    
    try {
        // output file stream from source to dest, check for errors
        std::ifstream source(source_filename, std::ios::binary);
        std::ofstream dest(destination_filename, std::ios::binary);
        if (!source.is_open() || !dest.is_open()) return nullptr;
        dest << source.rdbuf();
        if (!source || !dest) return nullptr;
        dirs->result = COPY_SUCCESS;
    } catch (const std::exception& e){
        dirs->result = COPY_ERROR;
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
    if (!directory_exists(argv[2])){
        std::cerr << "Source directory does not exist" << std::endl;
        exit(1);
    }
    std::string source_dir = normalise_dir(argv[2]);

    // validate destination directory exists otherwise no files to copy
    if (!directory_exists(argv[3])){
        std::cerr << "Destination directory does not exist" << std::endl;
        exit(1);
    }
    std::string destination_dir = normalise_dir(argv[3]);

    if (destination_dir == source_dir){
        std::cerr << "Source cannot be the same as destination" << std::endl;
        exit(1);
    }

    // initialise threads with id's mapping to source filename
    std::vector<pthread_t> threads(thread_count);
    std::vector<directory_pair_t> thread_args(thread_count);

    // create all threads with required args
    for (int i = 0; i < thread_count; i++){
        std::string current_filename = SOURCE_FILE_PREFIX + std::to_string(i + 1) + SOURCE_FILE_TYPE;
        std::string source_filename = source_dir + current_filename;
        std::string destination_filename = destination_dir + current_filename;
        
        // create args for directory pair, keep accessible, create thread
        thread_args[i] = {source_filename, destination_filename, NOT_COPIED};
        int ret = pthread_create(&threads[i], NULL, copy_file, &thread_args[i]);
        if (ret != 0) {
            std::cerr << "Error creating thread " << i << ": " << ret << std::endl;
            exit(1);
        }
    }

    // wait for all threads to complete 
    for (int i = 0; i < thread_count; i++){
        int ret = pthread_join(threads[i], NULL);
        if (ret != 0) {
            std::cerr << "Failed to terminate thread " << i << ": " << ret << std::endl;
            exit(1);
        }
        
        std::string status;
        switch (thread_args[i].result) {
            case SOURCE_NOT_FOUND:
                status = "SOURCE_NOT_FOUND";
                break;
            case SKIPPED_FILE_ALREADY_EXISTS:
                status = "SKIPPED_FILE_ALREADY_EXISTS";
                break;
            case COPY_SUCCESS:
                status = "COPY_SUCCESS";
                break;
            case COPY_ERROR:
                status = "COPY_ERROR";
                break;
            default:
                status = "NOT_COPIED";
        }
        std::cout << "Thread " << i << " " << status << ": " 
        << SOURCE_FILE_PREFIX << i + 1 << SOURCE_FILE_TYPE 
        << " from " << thread_args[i].source_filename << " to " 
        << thread_args[i].destination_filename << std::endl;
    }
    return 0;
}