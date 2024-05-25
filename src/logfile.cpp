#include <vector>
#include <string>
#include <sstream>
#include <fstream>

#include "logfile.h"

// Function that gets current date in "%Y-%m-%d" format
std::string get_date() {
    time_t now = time(0);
    tm *ltm = localtime(&now);
    char buffer[80];
    strftime(buffer, sizeof(buffer), "%Y-%m-%d", ltm);
    return buffer;
}

std::vector<std::string> _match_current_date(std::vector<std::string>& matches) {
    std::vector<std::string> new_matches;
    for (std::string& match : matches) {
        if (match.find(get_date()) != std::string::npos) {
            new_matches.push_back(match);
        }
    }
    return new_matches;
}

std::vector<std::string> match_for_current_date(LogFile &PacLog, std::string match) {
    std::vector<std::string> matches = PacLog.grep(match);
    return _match_current_date(matches);
}

std::vector<std::string> prettify(std::vector<std::string> _grades, const std::string_view &meta) {
    std::vector<std::string> result;

    for (std::string& _grade : _grades) {
        size_t pos = _grade.find(meta);
        if (pos != std::string::npos) {
            _grade.erase(0, pos + meta.length() + 1);
        }
        result.push_back(_grade);
    }
    return result;
}

LogFile::LogFile(std::string path) {
    this->path = path;
    this->content = "";

    // read the file and write its content to content
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

std::vector<std::string> LogFile::grep(std::string& match) {
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

std::string getEnvVar(std::string const &key) {
    char * val = getenv( key.c_str() );
    return val == NULL ? std::string("") : std::string(val);
}

void log_to_file(std::string path, LogFile &PacLog) {
    std::ofstream file(path, std::ios::app);

    file << '\n' << "╌╌╌╌╌╌" << get_date() << "╌╌╌╌╌╌" << '\n' << '\n';

    std::vector<std::string> upgrades = prettify(
        match_for_current_date(
            PacLog,
            std::string{ug_meta}
        ),
        ug_meta
    );

    std::vector<std::string> downgrades = prettify(
        match_for_current_date(
            PacLog,
            std::string{dg_meta}
        ),
        dg_meta
    );

    file << "┌Upgraded:\n";
    for (size_t i = 0; i < upgrades.size(); i++) {
        if (i == upgrades.size() - 1) {
            file << "└────" << upgrades[i] << "\n";
            break;
        }
        file << "├────" << upgrades[i] << "\n";
    }

    file << "\n┌Downgraded:\n";
    for (size_t i = 0; i < downgrades.size(); i++) {
        if (i == downgrades.size() - 1) {
            file << "└────" << downgrades[i] << "\n";
            break;
        }
        file << "├────" << downgrades[i] << "\n";
    }

    file.close();
}