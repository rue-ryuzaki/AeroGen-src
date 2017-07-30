#ifndef PROGRAM_H
#define PROGRAM_H

#include <string>

#define program_version "1.1.4.2"

#ifdef _WIN32
// windows
std::string app = "AeroGen.exe";
std::string updaterFile = "updater.exe";
std::string website = "https://github.com/rue-ryuzaki/AeroGen/blob/master/";
std::string updaterMD5File = "updater.md5";
#elif defined __linux__
//linux
std::string app = "AeroGen-linux";
std::string updaterFile = "updater-linux";
std::string website = "https://github.com/rue-ryuzaki/AeroGen/blob/master/";
std::string updaterMD5File = "updater-linux.md5";
#elif defined __APPLE__
// apple
std::string app = "AeroGen-apple";
std::string updaterFile = "updater-apple";
std::string website = "https://github.com/rue-ryuzaki/AeroGen/blob/master/";
std::string updaterMD5File = "updater-apple.md5";
#else
// unknown OS
std::string app = "AeroGen-unknown";
std::string updaterFile = "updater-unknown";
std::string website = "https://github.com/rue-ryuzaki/AeroGen/blob/master/";
std::string updaterMD5File = "updater-unknown.md5";
#endif

std::string currVersionFile = "aerogen-current-version";

#endif // PROGRAM_H
