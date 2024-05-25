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
    std::vector<std::string> grep(std::string& match);
};

// Function that gets current date in "%Y-%m-%d" format
std::string get_date();

std::vector<std::string> _match_current_date(std::vector<std::string>& matches);
std::vector<std::string> match_for_current_date(LogFile &PacLog, std::string match);

std::vector<std::string> prettify(std::vector<std::string> _grades, const char* meta);

void log_to_file(std::string path, LogFile &PacLog);

#endif