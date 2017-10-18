#ifndef OSM_LOG_H
#define	OSM_LOG_H

#include <ctime>
#include <iostream>

template <typename T>
void logIt(T v)
{
    char buffer[80];
    time_t seconds = time(NULL);
    tm* timeinfo = localtime(&seconds);
    char* format = "%B %d %H:%M:%S ";
    strftime(buffer, 80, format, timeinfo);
    std::cout << buffer << v << std::endl;
}

#endif	// OSM_LOG_H
