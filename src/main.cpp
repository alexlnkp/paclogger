#include <fstream>
#include <iostream>
#include <regex>
#include <set>
#include <vector>
#include <iterator>

void strip_fluff(std::string &pkg) {
    size_t it = 0;
    size_t pos;
    while ((pos = pkg.rfind("->")) != std::string::npos && it < pkg.size() + 1) {
        pkg.erase(pos, 3);
        it++; // Safeguards, in case something goes HORRIBLY wrong
    }
}

void removeSpaces(std::string &str) {
    // n is length of the original string
    int n = str.length();
 
    // i points to next position to be filled in
    // output string/ j points to next character
    // in the original string
    int i = 0, j = -1;
 
    // flag that sets to true is space is found
    bool spaceFound = false;
 
    // Handles leading spaces
    while (++j < n && str[j] == ' ');
 
    // read all characters of original string
    while (j < n)
    {
        // if current characters is non-space
        if (str[j] != ' ')
        {
            // remove preceding spaces before dot,
            // comma & question mark
            if ((str[j] == '.' || str[j] == ',' ||
                 str[j] == '?') && i - 1 >= 0 &&
                 str[i - 1] == ' ')
                str[i - 1] = str[j++];
 
            else {
                // copy current character at index i
                // and increment both i and j
                str[i++] = str[j++];
            }
 
            // set space flag to false when any
            // non-space character is found
            spaceFound = false;
        }
        // if current character is a space
        else if (str[j++] == ' ')
        {
            // If space is encountered for the first
            // time after a word, put one space in the
            // output and set space flag to true
            if (!spaceFound)
            {
                str[i++] = ' ';
                spaceFound = true;
            }
        }
    }
 
    // Remove trailing spaces
    if (i <= 1) 
        str.erase(str.begin() + i, str.end());
    else
        str.erase(str.begin() + i - 1, str.end());

    return;
}

/*
    \brief Function to parse a string containing a package line
    \brief and return a vector of package info.
    Format: `X repo/pkg old_ver new_ver`
    \example 4 core/linux 6.8.9.arch1-2 6.9.1.arch1-2
    \details So, the vector will be {4, "core/linux", "6.8.9.arch1-2", "6.9.1.arch1-2"}
    \details Which means the package number is in increments of 4.
    \details To get the next package number - you'd need to add 4 to the index.
*/
std::vector<std::string> parse_package_line(const std::string& line) {
    std::istringstream iss(line);
    std::vector<std::string> result((std::istream_iterator<std::string>(iss)),
                                     std::istream_iterator<std::string>());
    return result;
}

// Function to parse a string containing a list of packages to exclude
// and return a set of package numbers to exclude
std::set<int> parse_exclude_line(const std::string& line) {
    std::set<int> result;
    std::istringstream iss(line);
    std::string token;
    while (std::getline(iss, token, ' ')) {
        // Convert the token to an integer and add it to the set
        result.insert(std::stoi(token));
    }
    return result;
}

int main(int argc, char* argv[]) {
    std::ios::sync_with_stdio(false);
    
    std::string usr = ":: 4 packages to upgrade/install."                                       "\n"
                      "4  core/linux          6.8.9.arch1-2          -> 6.9.1.arch1-2"          "\n"
                      "3  core/linux-headers  6.8.9.arch1-2          -> 6.9.1.arch1-2"          "\n"
                      "2  extra/electron28    28.3.1-3               -> 28.3.2-1"               "\n"
                      "1  aur/obs-studio-git  30.1.2.r137.g649c62c-1 -> 30.1.2.r205.g72924ac-1" "\n"
                      "==> Packages to exclude: (eg: \"1 2 3\", \"1-3\", \"^4\" or repo name)"  "\n"
                      "-> Excluding packages may cause partial upgrades and break systems"      "\n"
                      "==> 1 3 4";
    
    std::string pkg = "4  core/linux          6.8.9.arch1-2          -> 6.9.1.arch1-2"          "\n"
                      "3  core/linux-headers  6.8.9.arch1-2          -> 6.9.1.arch1-2"          "\n"
                      "2  extra/electron28    28.3.1-3               -> 28.3.2-1"               "\n"
                      "1  aur/obs-studio-git  30.1.2.r137.g649c62c-1 -> 30.1.2.r205.g72924ac-1" "\n";

    removeSpaces(pkg);
    strip_fluff(pkg);

    std::vector<std::string> packages = parse_package_line(pkg);

    std::cout << packages.at(1);

    return 0;
}
