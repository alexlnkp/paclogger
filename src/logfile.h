#ifndef LOGFILE_H
#define LOGFILE_H

constexpr const char* dg_meta = "downgraded";
constexpr const char* ug_meta = "upgraded";

struct LogFile {
    std::string path;
    std::string content;

    LogFile(std::string path);

    // function to read the contents of a file at LogFile::path and write it to LogFile::content
    std::string read();

    // grep LogFile::content, return vector containing whole lines that match just a portion of the string
    std::vector<std::string> grep(std::string &match);
};

// Function that gets current date in "%Y-%m-%d" format
std::string get_date();

std::vector<std::string> _match_current_date(std::vector<std::string> &matches, std::string &date);
std::vector<std::string> match_for_current_date(LogFile &PacLog, std::string match, std::string &date);

std::vector<std::string> prettify(std::vector<std::string> _grades, const char* meta);

std::tuple<std::string, std::string, std::string> parse_package_string(const std::string& package);

std::pair<std::vector<std::string>, std::vector<std::string>> filter_common_entries(
    const std::vector<std::string>& upgrades, const std::vector<std::string>& downgrades);

void log_to_file(std::string path, LogFile &PacLog);

#endif