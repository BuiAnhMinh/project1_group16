#include "file_utils.h"
#include <filesystem>

bool directory_exists(const std::string& dir){
    try {
        return std::filesystem::exists(dir) && std::filesystem::is_directory(dir);
    } catch (const std::exception&) {
        return false;
    }
}

bool file_exists(const std::string& file_path){
    try {
        return std::filesystem::exists(file_path) && std::filesystem::is_regular_file(file_path);
    } catch (const std::exception&) {
        return false;
    }
}