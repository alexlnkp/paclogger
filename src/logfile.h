#ifndef LOGFILE_H
#define LOGFILE_H

struct LogFile {
    std::string path;
    std::string content;

    // Constructor that takes in one parameter: path
    LogFile(std::string path) {
        this->path = path;
        this->content = "";

        // read the file and write its content to content
        read();
    }

    // function to read the contents of a file at the path and write it to content
    std::string read() {
        std::ifstream file(path);
        std::string line;
        while (std::getline(file, line)) {
            content += line + "\n";
        }

        return content;
    }

    // grep content, return vector containing whole lines that match just a portion of the string
    std::vector<std::string> grep(std::string& match) {
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
};

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

#endif