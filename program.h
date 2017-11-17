#ifndef AEROGEN_PROGRAM_H
#define AEROGEN_PROGRAM_H

#include <string>

const std::string program_version = "1.1.5";

const std::string website = "https://raw.githubusercontent.com/rue-ryuzaki/AeroGen/master/";
#ifdef _WIN32
// windows
const std::string app = "AeroGen.exe";
const std::string updaterFile = "updater.exe";
const std::string updaterMD5File = "updater.md5";
#elif defined __linux__
//linux
const std::string app = "AeroGen-linux";
const std::string updaterFile = "updater-linux";
const std::string updaterMD5File = "updater-linux.md5";
#elif defined __APPLE__
// apple
const std::string app = "AeroGen-apple";
const std::string updaterFile = "updater-apple";
const std::string updaterMD5File = "updater-apple.md5";
#else
// unknown OS
const std::string app = "AeroGen-unknown";
const std::string updaterFile = "updater-unknown";
const std::string updaterMD5File = "updater-unknown.md5";
#endif

const std::string currVersionFile = "aerogen-current-version";

#endif // AEROGEN_PROGRAM_H
