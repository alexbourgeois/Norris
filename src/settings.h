#ifndef SETTINGS_H
#define SETTINGS_H

#include <QString>

enum NodeType {Pin, Clip, Trash};

struct NodeSettings {NodeType nodeType; QString path; int radius;};


enum DockType {Left, Right, Top};
#endif // SETTINGS_H
