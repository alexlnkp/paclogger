#include <vector>
#include <string>
#include <sstream>
#include <fstream>

#include "logfile.h"

#define PACMAN_LOG_FILE "/var/log/pacman.log"

int main() {
    std::ios::sync_with_stdio(false);
    
    LogFile PacLog(PACMAN_LOG_FILE);

    std::vector<std::string> matches = match_for_current_date(PacLog, "upgraded");

    for (std::string& match : matches) {
        printf("%s\n", match.c_str());
    }

    return 0;
}