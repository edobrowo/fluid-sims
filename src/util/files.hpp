#include <fstream>
#include <sstream>

#include "format.hpp"

std::string read_file_to_string(const char* path) {
    std::ifstream file_stream;
    file_stream.exceptions(std::ifstream::failbit | std::ifstream::badbit);

    std::string result;
    std::stringstream string_stream;
    try {
        file_stream.open(path);
        string_stream << file_stream.rdbuf();
        file_stream.close();
        result = string_stream.str();
    } catch (std::ifstream::failure& e) {
        eprintln("File could not be read: {}", e.what());
    }

    return result;
}
