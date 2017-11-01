#ifndef CONTAINER_H
#define CONTAINER_H

#include <QMainWindow>
#include <QObject>
#include <QDebug>
#include <QDir>

#include "settings.h";

class Container : public QMainWindow
{
    Q_OBJECT
public:
    explicit Container(QWidget *parent = nullptr);

    virtual void Initialize(NodeSettings settings);
signals:

public slots:
};

#endif // CONTAINER_H
