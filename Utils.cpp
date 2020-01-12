/*
 * Utils.cpp
 *
 *  Created on: Nov 16, 2019
 *      Author: jeremy
 */

#include "Utils.h"
#include <memory>

std::string Utils::exec(const std::string & cmd) {
    std::array<char, 128> buffer;
    std::string result;
    std::unique_ptr<FILE, decltype(&pclose)> pipe(popen(cmd.c_str(), "r"), pclose);
    if (!pipe) {
        throw std::runtime_error("popen() failed!");
    }
    while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr) {
        result += buffer.data();
    }
    return result;
}
