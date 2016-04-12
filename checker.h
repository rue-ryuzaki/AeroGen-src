#ifndef CHECKER_H
#define	CHECKER_H

#include <curl/curl.h>
#include <unistd.h>
#include <iostream>
#include "program.h"

using namespace std;

string updaterFileHash() {
    string result = "";
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

static size_t write_data(char *ptr, size_t size, size_t nmemb, string* data) {
    if (data) {
        data->append(ptr, size * nmemb);
        return size * nmemb;
    } else return 0;  // будет ошибка
}

size_t read_data(void *ptr, size_t size, size_t nmemb, FILE *stream) {
    size_t written = fwrite(ptr, size, nmemb, stream);
    return written;
}

string md5UpdaterHash() {
    CURL *curl_handle;
    curl_handle = curl_easy_init();

    string content = "";
    string url = website + updaterMD5File + "?raw=true";

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
            cout << "Some false!\n";
            cerr << curl_easy_strerror(res) << endl;
            content = "error read server updater md5 file";
        }
        // закрываем дескриптор curl
        curl_easy_cleanup(curl_handle);
    }
    if (content.find("404 Not Found") != string::npos || content.find("Not Found") != string::npos) {
        content = "error read server updater md5 file";
    }
    return content;
}

bool fileExists(const char * fname) {
    return (access(fname, F_OK) != -1);
}

void startUpdater() {
#ifdef _WIN32
// windows
    string command = updaterFile;
#else
    string command = "./" + updaterFile;
#endif
    system(command.c_str());
}

struct ver {
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
            case 2:
            {
                QRegExp regExp3("([0-9]+)*.*([0-9]+)*.*([0-9]+)");
                if (regExp3.exactMatch(value)) {
                    for (int i = 0; i < 3; ++i) {
                        sub[i] = regExp3.cap((i + 1)).toInt();
                    }
                } else {
                    cout << "Error read application version!\n";
                    return false;
                }
            }
                break;
            case 3:
            {
                QRegExp regExp4("([0-9]+)*.*([0-9]+)*.*([0-9]+)*.*([0-9]+)");
                if (regExp4.exactMatch(value)) {
                    for (int i = 0; i < 4; ++i) {
                        sub[i] = regExp4.cap((i + 1)).toInt();
                    }
                } else {
                    cout << "Error read application version!\n";
                    return false;
                }
            }
                break;
            default:
                cout << "Error read application version!\n";
                return false;
                break;
        }
        return true;
    }
};

string serverVersion() {
    CURL *curl_handle;
    curl_handle = curl_easy_init();
 
    string content = "";
    string url = website + currVersionFile + "?raw=true";

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
            cout << "Some false!\n";
            cerr << curl_easy_strerror(res) << endl;
            content = "";
        }
        // закрываем дескриптор curl
        curl_easy_cleanup(curl_handle);
    }

    return content;
}

bool checkUpdate() {
    string thisVersion = program_version;

    string newVersion  = serverVersion();
    if (newVersion.find("404 Not Found") != string::npos || newVersion.find("Not Found") != string::npos || newVersion == "") {
        cout << "Error 404: Server not found\n";
        return false;
    }
    cout << "server version:" << newVersion  << endl;
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

bool checkUpdater() {
    bool result = true;
    CURL *curl_handle;
    curl_handle = curl_easy_init();
 
    string content = "";
    string url = website + updaterFile + "?raw=true";

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
            cout << "Some false!\n";
            cerr << curl_easy_strerror(res) << endl;
            content = "";
        }
        // закрываем дескриптор curl
        curl_easy_cleanup(curl_handle);
    }
    
    if (content.find("404 Not Found") != string::npos || content.find("Not Found") != string::npos) {
        result = false;
    }
    
    return result;
}

bool DownloadUpdater() {
    if (!checkUpdater()) {
        cerr << "Updater checking failed!\n";
        return false;
    }
    bool result = true;
    CURL *curl_handle;
    curl_handle = curl_easy_init(); 
    
    string url = website + updaterFile + "?raw=true";

    if (curl_handle) {
        FILE * fp = fopen(updaterFile.c_str(), "wb");
        curl_easy_setopt(curl_handle, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl_handle, CURLOPT_SSL_VERIFYPEER, 0);
        curl_easy_setopt(curl_handle, CURLOPT_FOLLOWLOCATION, true);
        //curl_easy_setopt(curl_handle, CURLOPT_PORT, 8085);
        curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, read_data);
        curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, fp);
        CURLcode res = curl_easy_perform(curl_handle);
        if (res != CURLE_OK) {
            result = false;
            cout << "Some wrong!\n";
            cerr << curl_easy_strerror(res) << endl;
        }
        /* always cleanup */
        curl_easy_cleanup(curl_handle);
        fclose(fp);
    }
    return result;
}

#endif	/* CHECKER_H */
