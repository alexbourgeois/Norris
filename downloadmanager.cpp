#include "downloadmanager.h"

#include <QFileInfo>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QString>
#include <QStringList>
#include <QTimer>
#include <stdio.h>

DownloadManager::DownloadManager(QObject *parent)
    : downloadedCount(0), totalCount(0)
{
    manager = new QNetworkAccessManager(this);
}

void DownloadManager::append(const QUrl &url, QString path)
{
    if (downloadQueue.isEmpty())
        QTimer::singleShot(0, this, SLOT(startNextDownload()));

    struct RequestedFileStruct *requestedFile = new RequestedFileStruct(url, path);
    downloadQueue.enqueue(requestedFile);
    ++totalCount;
}

QString DownloadManager::saveFileName(const QUrl &fileURL)
{
    QString path = fileURL.path();
    QString basename = QFileInfo(path).fileName();

    if (basename.isEmpty())
        basename = "download";

    if (QFile::exists(basename)) {
        // already exists, don't overwrite
//        int i = 0;
//        basename += '.';
//        while (QFile::exists(basename + QString::number(i)))
//            ++i;

//        basename += QString::number(i);

        basename = "NULL";
    }

    return basename;
}

void DownloadManager::startNextDownload()
{
    if (downloadQueue.isEmpty()) {
        qDebug() << downloadedCount << "/" << totalCount << "files downloaded successfully\n";
        emit finished();
        return;
    }

    RequestedFileStruct *requestedFile = downloadQueue.dequeue();
    QUrl url = requestedFile->url;
    QString filename = saveFileName(url);
    if(filename == "NULL") {
        qDebug() << "File already exists !";
        return;
    }

    output.setFileName(requestedFile->path + "/" + filename);

     qDebug() << "Downloading " << url.toEncoded().constData() <<  " to " << requestedFile->path << "/" << filename;

    if (!output.open(QIODevice::WriteOnly)) {
        fprintf(stderr, "Problem opening save file '%s' for download '%s': %s\n",
                qPrintable(filename), url.toEncoded().constData(),
                qPrintable(output.errorString()));
        startNextDownload();
        return;                 // skip this download
    }

    QNetworkRequest *request = new QNetworkRequest(url);
    currentDownload = manager->get(*request);
    connect(currentDownload, SIGNAL(finished()), this,
            SLOT(downloadFinished()));
    connect(currentDownload, SIGNAL(readyRead()), this,
            SLOT(downloadReadyRead()));

    // prepare the output

    downloadTime.start();
}

void DownloadManager::downloadFinished()
{
    output.close();

    if (currentDownload->error()) {
        // download failed
        fprintf(stderr, "Failed: %s\n", qPrintable(currentDownload->errorString()));
    } else {
        printf("Succeeded.\n");
        ++downloadedCount;
    }

    currentDownload->deleteLater();
    startNextDownload();
}

void DownloadManager::downloadReadyRead()
{
    output.write(currentDownload->readAll());
}
