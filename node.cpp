#include "node.h"

Node::Node(QString name) {
	this->name = name;
}

Node::~Node() {

}

void Node::AddChild(Node* node)
{
	_childs.insert(node->GetName(), node);
}

int Node::RemoveChild(QString name) {
	return _childs.remove(name);
}

Node* Node::GetChild(QString name) {
	return _childs[name];
}

QMap<QString, Node*> Node::GetAllChilds()
{
	return _childs;
}


QString Node::GetName()
{
	return name;
}
