#ifndef FILE_UTILS_H
#define FILE_UTILS_H

#include <string>
#include <filesystem>
#include <iostream>
#include <stdexcept>
#include <filesystem>

enum dir_type_t {
    SOURCE,
    DESTINATION
};

bool directory_exists(const std::string& dir);
bool file_exists(const std::string& file_path);
bool check_directory_or_create(const std::string& dir);

#endif // FILE_UTILS_H