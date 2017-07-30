#ifndef CHECKER_H
#define	CHECKER_H

#include <curl/curl.h>
#include <iostream>
#include <string>
#include <unistd.h>

#include <QFile>

#include "program.h"

inline std::string updaterFileHash()
{
    std::string result;
    QFile file(QString::fromStdString(updaterFile));

    if (file.open(QIODevice::ReadOnly)) {
        QByteArray fileData = file.readAll();
        QByteArray hashData = QCryptographicHash::hash(fileData, QCryptographicHash::Md5);
        // or QCryptographicHash::Sha1
        result = hashData.toHex().data();
    } else {
        result = "error read local updater md5 file";
    }
    return result;
}

inline size_t write_data(char* ptr, size_t size, size_t nmemb, std::string* data)
{
    if (data) {
        data->append(ptr, size * nmemb);
        return size * nmemb;
    }
    return 0;  // будет ошибка
}

inline size_t read_data(void* ptr, size_t size, size_t nmemb, FILE* stream)
{
    return fwrite(ptr, size, nmemb, stream);
}

inline std::string md5UpdaterHash()
{
    CURL* curl_handle = curl_easy_init();

    std::string content;
    std::string url = website + updaterMD5File + "?raw=true";

    if (curl_handle) {
        // задаем  url адрес
        curl_easy_setopt(curl_handle, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl_handle, CURLOPT_SSL_VERIFYPEER, 0);
        curl_easy_setopt(curl_handle, CURLOPT_FOLLOWLOCATION, true);
        //curl_easy_setopt(curl_handle, CURLOPT_PORT, 8085);
        curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, &content);
        curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, write_data);
        // выполняем запрос
        CURLcode res = curl_easy_perform(curl_handle);
        if (res != CURLE_OK) {
            std::cout << "Some false!" << std::endl;
            std::cerr << curl_easy_strerror(res) << std::endl;
            content = "error read server updater md5 file";
        }
        // закрываем дескриптор curl
        curl_easy_cleanup(curl_handle);
    }
    if (content.find("404 Not Found") != std::string::npos || content.find("Not Found") != std::string::npos) {
        content = "error read server updater md5 file";
    }
    return content;
}

inline bool fileExists(const char* fname)
{
    return (access(fname, F_OK) != -1);
}

inline void startUpdater()
{
#ifdef _WIN32
// windows
    std::string command = updaterFile;
#else
    std::string command = "./" + updaterFile;
#endif
    system(command.c_str());
}

struct ver
{
    QString value;
    int sub[4];
    
    bool update() {
        for (int i = 0; i < 4; ++i) {
            sub[i] = 0;
        }
        int dot = 0;
        for (int i = 0; i < value.size(); ++i) {
            if (value[i] == '.') {
                ++dot;
            }
        }
        switch (dot) {
            case 2 :
            {
                QRegExp regExp3("([0-9]+)*.*([0-9]+)*.*([0-9]+)");
                if (regExp3.exactMatch(value)) {
                    for (int i = 0; i < 3; ++i) {
                        sub[i] = regExp3.cap((i + 1)).toInt();
                    }
                } else {
                    std::cout << "Error read application version!" << std::endl;
                    return false;
                }
            }
                break;
            case 3 :
            {
                QRegExp regExp4("([0-9]+)*.*([0-9]+)*.*([0-9]+)*.*([0-9]+)");
                if (regExp4.exactMatch(value)) {
                    for (int i = 0; i < 4; ++i) {
                        sub[i] = regExp4.cap((i + 1)).toInt();
                    }
                } else {
                    std::cout << "Error read application version!" << std::endl;
                    return false;
                }
            }
                break;
            default:
                std::cout << "Error read application version!" << std::endl;
                return false;
        }
        return true;
    }
};

inline std::string serverVersion()
{
    CURL* curl_handle = curl_easy_init();
 
    std::string content;
    std::string url = website + currVersionFile + "?raw=true";

    if (curl_handle) {
        // задаем  url адрес
        curl_easy_setopt(curl_handle, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl_handle, CURLOPT_SSL_VERIFYPEER, 0);
        curl_easy_setopt(curl_handle, CURLOPT_FOLLOWLOCATION, true);
        //curl_easy_setopt(curl_handle, CURLOPT_PORT, 8085);
        curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, &content);
        curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, write_data);
        // выполняем запрос
        CURLcode res = curl_easy_perform(curl_handle);
        if (res != CURLE_OK) {
            std::cout << "Some false!" << std::endl;
            std::cerr << curl_easy_strerror(res) << std::endl;
            content = "";
        }
        // закрываем дескриптор curl
        curl_easy_cleanup(curl_handle);
    }

    return content;
}

inline bool checkUpdate()
{
    std::string thisVersion = program_version;
    std::string newVersion  = serverVersion();
    if (newVersion.find("404 Not Found") != std::string::npos || newVersion.find("Not Found") != std::string::npos || newVersion == "") {
        std::cout << "Error 404: Server not found" << std::endl;
        return false;
    }
    std::cout << "server version:" << newVersion << std::endl;
    ver thisVer;
    thisVer.value = QString::fromStdString(thisVersion);
    thisVer.update();
    ver newVer;
    newVer.value = QString::fromStdString(newVersion);
    if (!newVer.update()) {
        return false;
    }
    
    for (int i = 0; i < 4; ++i) {
        if (thisVer.sub[i] < newVer.sub[i]) {
            return true;
        }
        if (thisVer.sub[i] > newVer.sub[i]) {
            return false;
        }
    }
    return false;
}

inline bool checkUpdater()
{
    bool result = true;
    CURL* curl_handle = curl_easy_init();
 
    std::string content;
    std::string url = website + updaterFile + "?raw=true";

    if (curl_handle) {
        // задаем  url адрес
        curl_easy_setopt(curl_handle, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl_handle, CURLOPT_SSL_VERIFYPEER, 0);
        curl_easy_setopt(curl_handle, CURLOPT_FOLLOWLOCATION, true);
        //curl_easy_setopt(curl_handle, CURLOPT_PORT, 8085);
        curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, &content);
        curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, write_data);
        // выполняем запрос
        CURLcode res = curl_easy_perform(curl_handle);
        if (res != CURLE_OK) {
            result = false;
            std::cout << "Some false!" << std::endl;
            std::cerr << curl_easy_strerror(res) << std::endl;
            content = "";
        }
        // закрываем дескриптор curl
        curl_easy_cleanup(curl_handle);
    }
    
    if (content.find("404 Not Found") != std::string::npos || content.find("Not Found") != std::string::npos) {
        result = false;
    }
    
    return result;
}

inline bool DownloadUpdater()
{
    if (!checkUpdater()) {
        std::cerr << "Updater checking failed!" << std::endl;
        return false;
    }
    bool result = true;
    CURL* curl_handle = curl_easy_init();
    
    std::string url = website + updaterFile + "?raw=true";

    if (curl_handle) {
        FILE* fp = fopen(updaterFile.c_str(), "wb");
        curl_easy_setopt(curl_handle, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl_handle, CURLOPT_SSL_VERIFYPEER, 0);
        curl_easy_setopt(curl_handle, CURLOPT_FOLLOWLOCATION, true);
        //curl_easy_setopt(curl_handle, CURLOPT_PORT, 8085);
        curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, read_data);
        curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, fp);
        CURLcode res = curl_easy_perform(curl_handle);
        if (res != CURLE_OK) {
            result = false;
            std::cout << "Some wrong!" << std::endl;
            std::cerr << curl_easy_strerror(res) << std::endl;
        }
        // always cleanup
        curl_easy_cleanup(curl_handle);
        fclose(fp);
    }
    return result;
}

#endif	// CHECKER_H
