#include <vector>
#include <string>
#include <sstream>
#include <fstream>

#include "logfile.h"
#include "env.h"

#define PACMAN_LOG_FILE "/var/log/pacman.log"

int main() {
    std::ios::sync_with_stdio(false);
    
    LogFile PacLog(PACMAN_LOG_FILE);

    log_to_file(PL_ENV, PacLog);

    return 0;
}
