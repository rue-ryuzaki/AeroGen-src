#ifndef AEROGEN_CHECKER_H
#define AEROGEN_CHECKER_H

#include <QFile>

#ifdef QT_NETWORK_LIB
#include <QEventLoop>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QObject>
#include <QUrl>

#include <iostream>
#include <string>
#endif // QT_NETWORK_LIB

#include "program.h"

inline bool fileExists(const char* fname)
{
    QFile file(fname);
    return file.exists();
}

#ifdef QT_NETWORK_LIB
inline std::string updaterFileHash()
{
    QFile file(QString::fromStdString(updaterFile));
    if (file.open(QIODevice::ReadOnly)) {
        QByteArray fileData = file.readAll();
        file.close();
        QByteArray hashData = QCryptographicHash::hash(fileData, QCryptographicHash::Md5);
        // or QCryptographicHash::Sha1
        return hashData.toHex().data();
    } else {
        return "error read local updater md5 file";
    }
}

inline std::string md5UpdaterHash()
{
    QByteArray content;
    QUrl url = QString::fromStdString(website) + QString::fromStdString(updaterMD5File);
    QNetworkAccessManager manager;
    QNetworkReply* reply = manager.get(QNetworkRequest(url));
    QEventLoop loop;
    QObject::connect(reply, SIGNAL(finished()), &loop, SLOT(quit()));
    loop.exec();
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

inline void startUpdater()
{
    std::string command =
#ifndef _WIN32
        "./" +
#endif
        updaterFile;

    system(command.c_str());
}

struct ver
{
    ver()
        : value(),
          sub()
    { }

    QString     value;
    uint32_t    sub[4];

    bool update() {
        for (size_t i = 0; i < 4; ++i) {
            sub[i] = 0;
        }
        int32_t dot = 0;
        for (int32_t i = 0; i < value.size(); ++i) {
            if (value[i] == '.') {
                ++dot;
            }
        }
        switch (dot) {
            case 2 : // fallthrough
            case 3 :
                {
                    QString pattern("^([0-9]*)");
                    for (int32_t i = 1; i < dot; ++i) {
                        pattern += ".([0-9]*)";
                    }
                    pattern += ".([0-9]*)$";
                    QRegExp regExp(pattern);
                    if (regExp.exactMatch(value)) {
                        for (int32_t i = 0; i <= dot; ++i) {
                            sub[i] = regExp.cap((i + 1)).toInt();
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
    QUrl url = QString::fromStdString(website) + QString::fromStdString(currVersionFile);
    QNetworkAccessManager manager;
    QNetworkReply* reply = manager.get(QNetworkRequest(url));
    QEventLoop loop;
    QObject::connect(reply, SIGNAL(finished()), &loop, SLOT(quit()));
    loop.exec();
    if (!reply->error()) {
        content = reply->readAll();
    } else {
        std::cout << QString(reply->readAll()).toStdString() << std::endl;
        std::cout << reply->error() << std::endl;
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
    for (size_t i = 0; i < 4; ++i) {
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
    QUrl url = QString::fromStdString(website) + QString::fromStdString(updaterMD5File);
    QNetworkAccessManager manager;
    QNetworkReply* reply = manager.get(QNetworkRequest(url));
    QEventLoop loop;
    QObject::connect(reply, SIGNAL(finished()), &loop, SLOT(quit()));
    loop.exec();
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
    QUrl url = QString::fromStdString(website) + QString::fromStdString(updaterFile);
    QNetworkAccessManager manager;
    QNetworkReply* reply = manager.get(QNetworkRequest(url));
    QEventLoop loop;
    QObject::connect(reply, SIGNAL(finished()), &loop, SLOT(quit()));
    loop.exec();
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
#endif // QT_NETWORK_LIB

#endif // AEROGEN_CHECKER_H
