#pragma once
#ifndef BUCKET_H
#define BUCKET_H

#include <QScrollBar>
#include <QScrollArea>
#include <QWidget>
#include <QMainWindow>
#include <QpaintEvent>
#include <QMouseEvent>
#include <QMimeData>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QPropertyAnimation>
#include <QUrl>
#include <QMovie>
#include <QDebug>
#include <QDir>
#include <QFileInfo>
#include <QLabel>
#include <QDirIterator>
#include <QThread>
#include <QDrag>
#include <QListWidget>
#include <QStatusBar>
#include <QProcess>
#include <QScreen>

#include "ui_bucket.h"
#include "bucket.hpp"
#include "settings.h"
#include "mimedata.h"

class QSvgWidget;

namespace Ui {
	class Bucket;
}

class Bucket : public QMainWindow
{
	Q_OBJECT

public:
    Bucket(NodeSettings settings, QWidget *parent = 0);
	~Bucket();
	void addText(QString);
	void InitializeWindow();
    void MoveTo(QPoint target);
    void Open();
    void Close();
    bool IsImage(QString suffix);
    void SetNewPath(QString path);
    void InitializePin(QString path);
    void InitializeClip();



    void startDrag();

private slots:
    void setTargetDirectory() ;
    void listWidgetItemDoubleClicked(QListWidgetItem *item);

    void on_openExplorerButton_clicked();

signals:
    void IWantAFile(QUrl, QString);
    void ReceivedSmth(QString);
    void ItemSelectedByUser(QListWidgetItem* item);

public slots:
    void GetADropEvent(QDropEvent* event);
    void UpdateVisibleFiles();
        void createData(const QString &mimeType);

protected:
    virtual void enterEvent(QEvent* e);
	void dropEvent(QDropEvent *event) Q_DECL_OVERRIDE;
    void dragLeaveEvent(QDragLeaveEvent *event) Q_DECL_OVERRIDE;
    void mousePressEvent(QMouseEvent *event) Q_DECL_OVERRIDE;

public:
    QDir workingDirectory;
    bool isClosed;

private:
    int width;
    int height;
    int imageNumber;

	Ui::Bucket *ui;
    QListWidget *listWidget;

    QSvgWidget *imageLabel;
    MimeData *mimeData;
};
#endif
