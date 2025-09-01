#include <iostream>
#include <string>
#include <stdexcept>
#include <string>

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
     if (argc != 4){
        std::cerr << "Usage: " << argv[0] 
                  << " <thread_count> <source_dir> <destination_dir>" << std::endl;
        return EXIT_FAILURE;
    }

    std::string thread_count_raw = argv[1];
    std::string source_dir = argv[2];
    std::string destination_dir = argv[3];
    return EXIT_SUCCESS;
}



    



