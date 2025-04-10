#include "config.hpp"

#include "util/files.hpp"

Config Config::loadFromJson(const std::string& path) {
    Config config;

    files::Json config_file = files::read_to_json(path.c_str());

    config.rows = config_file["grid_rows"];
    config.cols = config_file["grid_cols"];
    config.cellSize = 1.0 / config.rows;
    config.timestep = config_file["timestep"];
    config.saveFrames = config_file["save_frames"];

    return config;
}
