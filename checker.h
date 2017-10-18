#ifndef CHECKER_H
#define	CHECKER_H

#include <iostream>
#include <string>

#include <QFile>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QUrl>

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

inline std::string md5UpdaterHash()
{
    QByteArray content;

    QUrl url = QString::fromStdString(website)
            + QString::fromStdString(updaterMD5File) + "?raw=true";
    QNetworkAccessManager manager;
    QNetworkReply* reply = manager.get(QNetworkRequest(url));

    if (!reply->error()) {
        content = reply->readAll();
    } else {
        return "error read server updater md5 file";
    }
    std::string value = QString(content).toStdString();
    if (value.find("404 Not Found") != std::string::npos
            || value.find("Not Found") != std::string::npos) {
        return "error read server updater md5 file";
    }
    return value;
}

inline bool fileExists(const char* fname)
{
    QFile file(fname);
    return file.exists();
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
    QString     value;
    uint32_t    sub[4];

    bool update() {
        for (size_t i = 0; i < 4; ++i) {
            sub[i] = 0;
        }
        size_t dot = 0;
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
                        for (size_t i = 0; i < 3; ++i) {
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
                        for (size_t i = 0; i < 4; ++i) {
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
    QByteArray content;
    QUrl url = QString::fromStdString(website)
            + QString::fromStdString(currVersionFile) + "?raw=true";
    QNetworkAccessManager manager;
    QNetworkReply* reply = manager.get(QNetworkRequest(url));

    if (!reply->error()) {
        content = reply->readAll();
    }

    return QString(content).toStdString();
}

inline bool checkUpdate()
{
    std::string thisVersion = program_version;
    std::string newVersion  = serverVersion();
    if (newVersion.find("404 Not Found") != std::string::npos
            || newVersion.find("Not Found") != std::string::npos
            || newVersion.empty()) {
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
    bool result = false;

    QByteArray content;
    QUrl url = QString::fromStdString(website)
            + QString::fromStdString(updaterMD5File) + "?raw=true";
    QNetworkAccessManager manager;
    QNetworkReply* reply = manager.get(QNetworkRequest(url));

    if (!reply->error()) {
        content = reply->readAll();
        result = true;
    }

    std::string value = QString(content).toStdString();
    if (value.find("404 Not Found") != std::string::npos
            || value.find("Not Found") != std::string::npos) {
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
    bool result = false;

    QUrl url = QString::fromStdString(website)
            + QString::fromStdString(updaterFile) + "?raw=true";
    QNetworkAccessManager manager;
    QNetworkReply* reply = manager.get(QNetworkRequest(url));

    if (!reply->error()) {
        QFile file(updaterFile.c_str());
        if (file.open(QFile::WriteOnly)) {
            file.write(reply->readAll());
            file.close();
            result = true;
        }
    }
    return result;
}

#endif	// CHECKER_H
