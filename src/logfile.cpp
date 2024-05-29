#include <cstring>
#include <ctll.hpp>
#include <ctre.hpp>
#include <filesystem>
#include <fstream>
#include <map>
#include <set>
#include <sstream>
#include <string>
#include <vector>

#include "logfile.h"

static constexpr auto package_pattern = ctll::fixed_string{ R"((\S+) \(([\d.]+-\d+) -> ([\d.]+-\d+)\))" };

// Function that gets current date in "%Y-%m-%d" format
std::string get_date() {
    time_t now = time(0);
    tm *ltm = localtime(&now);
    char buffer[80];
    strftime(buffer, sizeof(buffer), "%Y-%m-%d", ltm);
    return buffer;
}

std::vector<std::string> _match_current_date(std::vector<std::string> &matches, std::string &date) {
    std::vector<std::string> new_matches;
    for (std::string& match : matches) {
        if (match.find(date) != std::string::npos) {
            new_matches.push_back(match);
        }
    }
    return new_matches;
}

std::vector<std::string> match_for_current_date(LogFile &PacLog, std::string match, std::string &date) {
    std::vector<std::string> matches = PacLog.grep(match);
    return _match_current_date(matches, date);
}

std::vector<std::string> prettify(std::vector<std::string> _grades, const char* meta) {
    std::vector<std::string> result;

    for (std::string& _grade : _grades) {
        size_t pos = _grade.find(meta);
        if (pos != std::string::npos) {
            _grade.erase(0, pos + strlen(meta) + 1);
        }
        result.push_back(_grade);
    }
    return result;
}

LogFile::LogFile(std::string path) {
    this->path = path;
    this->content = "";
    read();
}

std::string LogFile::read() {
    std::ifstream file(path);
    std::string line;
    while (std::getline(file, line)) {
        content += line + "\n";
    }
    return content;
}

std::vector<std::string> LogFile::grep(std::string &match) {
    std::vector<std::string> matches;
    std::istringstream contentStream(content);
    std::string cur_line;

    while (std::getline(contentStream, cur_line)) {
        if (cur_line.find(match) != std::string::npos) {
            matches.push_back(cur_line);
        }
    }
    return matches;
}

// Function to parse a package string and return a tuple (package name, old version, new version)
std::tuple<std::string, std::string, std::string> parse_package_string(const std::string& package) {
    // Using CTRE regular expression
    if (auto [whole, pkg_name, old_ver, new_ver] = ctre::match<package_pattern>(package); whole) {
        return std::make_tuple(pkg_name.to_string(), old_ver.to_string(), new_ver.to_string());
    }
    return std::make_tuple(std::string(), std::string(), std::string());
}

std::pair<std::vector<std::string>, std::vector<std::string>> filter_common_entries(
    const std::vector<std::string>& upgrades, const std::vector<std::string>& downgrades) {

    // Create maps for quick lookup by package name and version.
    std::map<std::string, std::pair<std::string, std::string>> upgrades_map;
    std::map<std::string, std::pair<std::string, std::string>> downgrades_map;

    for (const auto& u : upgrades) {
        auto [name, old_version, new_version] = parse_package_string(u);
        upgrades_map[name] = {old_version, new_version};
    }

    for (const auto& d : downgrades) {
        auto [name, old_version, new_version] = parse_package_string(d);
        downgrades_map[name] = {old_version, new_version};
    }

    // Containers for filtered results.
    std::vector<std::string> filtered_upgrades;
    std::vector<std::string> filtered_downgrades;

    // Filter upgrades
    for (const auto& u : upgrades) {
        auto [name, old_version, new_version] = parse_package_string(u);
        if (downgrades_map.find(name) == downgrades_map.end() ||
            downgrades_map[name].first != new_version ||
            downgrades_map[name].second != old_version) {
            filtered_upgrades.push_back(u);
        }
    }

    // Filter downgrades
    for (const auto& d : downgrades) {
        auto [name, old_version, new_version] = parse_package_string(d);
        if (upgrades_map.find(name) == upgrades_map.end() ||
            upgrades_map[name].first != new_version ||
            upgrades_map[name].second != old_version) {
            filtered_downgrades.push_back(d);
        }
    }

    return {filtered_upgrades, filtered_downgrades};
}

void write_updates(std::ofstream &file, const std::string &title, const std::vector<std::string> &updates) {
    if (updates.empty()) return;

    std::set<std::string> unique_updates(updates.begin(), updates.end());
    std::vector<std::string> unique_updates_vector(unique_updates.begin(), unique_updates.end());

    file << "┌" << title << ":\n";
    for (size_t i = 0; i < unique_updates_vector.size(); ++i) {
        file << (i == unique_updates_vector.size() - 1 ? "└────" : "├────") << unique_updates_vector[i] << "\n";
    }
}

// Checks if file at `path` exists
void ensure_file_exists(const std::filesystem::path &path) {
    if (!std::filesystem::exists(path)) {
        std::ofstream create_file(path);
        create_file.close();
    }
}

// Logs all updates and downgrades to file in the order they were installed
void log_to_file(std::string path, LogFile &PacLog) {
    std::ofstream file;

    std::string current_date = get_date();
    std::string date_line = "\n╌╌╌╌╌╌" + current_date + "╌╌╌╌╌╌\n";

    ensure_file_exists(path);

    std::ifstream read_file(path);
    if (!read_file.is_open()) {
        throw std::runtime_error("Failed to open file for reading " + path);
    }

    std::string content((std::istreambuf_iterator<char>(read_file)),
                         std::istreambuf_iterator<char>());
    read_file.close();

    size_t pos = content.find(date_line);
    if (pos != std::string::npos) {
        // Erase from the current date line to the end of the file
        content.erase(pos);

        // Open the file for writing and truncate it
        std::ofstream write_file(path, std::ios::trunc);
        if (!write_file.is_open()) {
            throw std::runtime_error("Failed to open file for writing " + path);
        }

        write_file << content;
        write_file.close();
    }

    file.open(path, std::ios_base::app);
    
    file << date_line << '\n';
    
    std::vector<std::string> upgrades = prettify(
        match_for_current_date(PacLog, ug_meta, current_date), ug_meta
    );

    std::vector<std::string> downgrades = prettify(
        match_for_current_date(PacLog, dg_meta, current_date), dg_meta
    );

    auto [filtered_upgrades, filtered_downgrades] = filter_common_entries(upgrades, downgrades);

    write_updates(file, "Upgraded", filtered_upgrades);
    file << "\n";
    write_updates(file, "Downgraded", filtered_downgrades);

    file.close();
}