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
#include <node.h>

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
     Q_PROPERTY(QColor color READ color WRITE setColor)

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

    void setColor (QColor color){
        setStyleSheet(QString("background-color: rgba(%1, %2, %3, %4);").arg(color.red()).arg(color.green()).arg(color.blue()).arg(color.alpha()));
    }
    QColor color(){
        return QWidget::palette().color(QWidget::backgroundRole());
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
    int backgroundAlpha;
    bool trashVisible;
    bool UIVisible;

    NodeWidget* trashWidget;

private:
	void InitializeWindow();

    QPropertyAnimation* dayNightAnimation;
    bool animationHasAValue;
    QThread workerThread;
    QPoint dragPosition;
    DownloadManager *manager;
    QList<NodeWidget*> _nodes;
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
