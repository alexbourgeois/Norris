#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QDesktopWidget>
#include <QPainter>
#include <QListWidgetItem>
#include <QPainter>
#include <QPaintEvent>
#include <QMap>
#include <QDebug>

#include "node.h"
#include "nodewidget.h"
#include "settings.h"
#include "qxtglobalshortcut.h"
#include "downloadmanager.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
    Q_PROPERTY(int backgroundAlpha READ getBackgroundAlpha WRITE setBackgroundAlpha)

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    void AddNode(Node*, NodeSettings);
    void RemoveNode(QString name);
    void ShowTrash();
    void HideTrash();
    void SetNight();
    void SetDay();
    virtual void paintEvent(QPaintEvent*);
    virtual void enterEvent(QEvent* e);
    void mouseMoveEvent(QMouseEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void dragEnterEvent(QDragEnterEvent *event);
    void dropEvent(QDropEvent *event);
    void leaveEvent(QEvent* e);

    int getBackgroundAlpha() {
        return backgroundAlpha;
    }

    void setBackgroundAlpha(int value) {
        backgroundAlpha = value;
        QPalette palette;
        palette.setBrush(QPalette::Background, QColor(0,0,0,backgroundAlpha));
        this->setAutoFillBackground(true);
        setPalette(palette);

        repaint();
        this->show();
    }

signals:
    void launchShortcutPressed();
    void clipboardShortcutPressed();

public slots:
    void ClipboardShortcutActivated();
    void ToggleNodeShortcut();
    void ToggleUI();
    void CreatePin(NodeWidget* node,QListWidgetItem* item);
    void DownloadFile(QUrl, QString);
    void NodeMoving(NodeWidget*);
    void NodeMoved(NodeWidget*);
    void WarnNodesFileDownloaded();

public:
    int width;
    int height;
    int backgroundAlpha = 255;
    bool trashVisible;
    bool UIVisible;

    NodeWidget* trashWidget;

private:
	void InitializeWindow();

    QThread workerThread;
    QPoint dragPosition;
    DownloadManager *manager;
    QList<NodeWidget*> _nodes;
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
