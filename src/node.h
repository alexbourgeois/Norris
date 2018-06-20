#pragma once
#include <QObject>
#include <memory>
#include "bucket.hpp"
#include <QMap>

class Node {

public:
	Node(QString name);
	~Node();
	void AddChild(Node*);
	int RemoveChild(QString name);
	Node* GetChild(QString name);
	QMap<QString, Node*> GetAllChilds();
	QString GetName();

	Bucket* bucket;
protected:
	//QList<Token*> _childs;
	QString name;
	QMap<QString, Node*> _childs;
};
