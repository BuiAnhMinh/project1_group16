#include "file_utils.h"

bool directory_exists(const std::string& dir){
    if (std::filesystem::exists(dir) && std::filesystem::is_directory(dir)){
        return true;
    } else {
        return false;
    }
}

bool file_exists(const std::string& file_path){
    if (std::filesystem::exists(file_path) && std::filesystem::is_regular_file(file_path)){
        return true;
    } else {
        return false;
    }
}

bool check_directory_or_create(const std::string& dir){
    if (std::filesystem::exists(dir) && std::filesystem::is_directory(dir)) {
        return true;
    }
    try {
        std::filesystem::create_directories(dir);
        return true;
    } catch (const std::exception& e){
        return false;
    }
}