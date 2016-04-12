#ifndef PROGRAM_H
#define PROGRAM_H

#define program_version "1.1.4.1"

#ifdef _WIN32
// windows
string app = "AeroGen.exe";
string updaterFile = "updater.exe";
string website = "https://github.com/rue-ryuzaki/AeroGen/blob/master/";
string updaterMD5File = "updater.md5";
#elif defined __linux__
//linux
string app = "AeroGen-linux";
string updaterFile = "updater-linux";
string website = "https://github.com/rue-ryuzaki/AeroGen/blob/master/";
string updaterMD5File = "updater-linux.md5";
#elif defined __APPLE__
// apple
string app = "AeroGen-apple";
string updaterFile = "updater-apple";
string website = "https://github.com/rue-ryuzaki/AeroGen/blob/master/";
string updaterMD5File = "updater-apple.md5";
#else
// unknown OS
string app = "AeroGen-unknown";
string updaterFile = "updater-unknown";
string website = "https://github.com/rue-ryuzaki/AeroGen/blob/master/";
string updaterMD5File = "updater-unknown.md5";
#endif

string currVersionFile = "aerogen-current-version";

#endif /* PROGRAM_H */
