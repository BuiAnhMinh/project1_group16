#include "file_utils.h"

bool directory_exists(const std::string& dir){
    if (std::filesystem::exists(dir) && std::filesystem::is_directory(dir)){
        return true;
    } else {
        std::cerr << "Directory does not exist: " << dir << std::endl;
        return false;
    }
}

bool file_exists(const std::string& file_path){
    if (std::filesystem::exists(file_path) && std::filesystem::is_regular_file(file_path)){
        return true;
    } else {
        std::cerr << "File does not exist: " << file_path << std::endl;
        return false;
    }
}

bool check_directory_or_create(const std::string& dir){
    if (!directory_exists(dir)) {
        try {
            std::filesystem::create_directories(dir);
            return true;
        } catch (const std::exception& e){
            std::cerr << "Error creating directory: " << e.what() << std::endl;
            return false;
        }
    }
    return true;
}