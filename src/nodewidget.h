#pragma once
#ifndef NODEWIDGET_H
#define NODEWIDGET_H

#include <QWidget>
#include <QpaintEvent>
#include <Qpainter>
#include <QLabel>
#include <QPropertyAnimation>
#include <QApplication>
#include <QDebug>
#include <QTime>
#include <QtMath>

#include "node.h"
#include "bucket.hpp"
#include "settings.h"
#include "ui_node.h"

namespace Ui {
    class NodeWidget;
}

class NodeWidget : public QWidget
{
    Q_OBJECT

public:
    NodeWidget(Node* node, QWidget *parent, int size,  NodeSettings settings);
	~NodeWidget();

	virtual void paintEvent(QPaintEvent*);
	virtual void enterEvent(QEvent* e);
	void mouseMoveEvent(QMouseEvent *event);
	void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void mouseDoubleClickEvent(QMouseEvent *event);
	void dragEnterEvent(QDragEnterEvent *event);
	void dropEvent(QDropEvent *event);
	void leaveEvent(QEvent* e);

    void SetNodeImage(QString fileName);
	void DrawContent();
    void MoveTo(QPoint target);
    void MoveTo(int x, int y);
	QPoint Center();
	QString GetName();
    QPoint BottomLeft();
    void OpenBucket(bool open);

	Node *node;
	Bucket *bucket;

public slots:
    void ToggleBucket();
    void DockNode();
    void CopyClipboard();
    void DockingCompleted();
    void ChangeNodeImage(QString);
    void CreatePin(QListWidgetItem *item);
    void FileRequested(QUrl, QString);
    void HandleNewFile();

signals:
    void DropOnNode(QDropEvent* event);
    void BucketItemSelected(NodeWidget* node, QListWidgetItem *item);
    void FileNeeded(QUrl, QString);

    void NodeMoving(NodeWidget*);
    void NodeMoved(NodeWidget*);

public:
    NodeSettings settings;

private:
    void InitializeName();
    void InitializeWindow();

    QString actualImage;
    int _size;
	int _screenWidth, _screenHeight;
	bool isBucketOpened;
    bool bucketWasOpen;
    bool isMoving;

    int displacementStartTime;
    float dockTiggerValue = 0.5;
    int displacementLength;

	QPoint dragPosition, center;
	QString _name;
	QColor _color;
	QLabel *nodeName;
    QWidget *nodeDrawer;
    Ui::NodeWidget *ui;
};

#endif // NODEWIDGET_H
