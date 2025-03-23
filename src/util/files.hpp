#pragma once

#include <json.hpp>

namespace files {

using Json = nlohmann::json;

std::string read_to_string(const char* path);
Json read_to_json(const char* path);

}
