#include "files.hpp"

#include <fstream>
#include <sstream>

#include "files.hpp"
#include "format.hpp"

namespace files {

std::string read_to_string(const char* path) {
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

Json read_to_json(const char* path) {
    std::ifstream file_stream;
    file_stream.exceptions(std::ifstream::failbit | std::ifstream::badbit);

    Json parsed_json;
    try {
        file_stream.open(path);
        file_stream >> parsed_json;
    } catch (std::ifstream::failure& e) {
        eprintln("File could not be read: {}", e.what());
    } catch (nlohmann::json_abi_v3_11_3::detail::parse_error& e) {
        eprintln("JSON parse error: {}", e.what());
    }

    return parsed_json;
}

}
