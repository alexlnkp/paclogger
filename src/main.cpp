#include <vector>
#include <string>
#include <sstream>
#include <fstream>

#include "logfile.h"

#define PACMAN_LOG_FILE "/var/log/pacman.log"

int main() {
    std::ios::sync_with_stdio(false);
    
    LogFile PacLog(PACMAN_LOG_FILE);

    std::string updates_log = getEnvVar("PACLOGGER_UPDATES_LOG");
    updates_log = (updates_log == "") ? "pacman.updates.log" : updates_log;

    log_to_file(updates_log, PacLog);

    return 0;
}
