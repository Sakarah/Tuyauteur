#ifndef PATHFINDING_H
#define PATHFINDING_H

#include "utils.h"

using CheckPosFunc = std::function<bool(position)>;
using EvalPosFunc = std::function<int(position)>;
using PathFunc = std::function<std::pair<std::vector<position>, int>(position,int)>;
using CheckPathFunc = std::function<bool(std::vector<position>)>;

int distToBase(position from, int owner);
std::pair<std::vector<position>, int> dijkPath(position from, CheckPosFunc canBeEndPos, EvalPosFunc computeCost, CheckPosFunc validatePos, int maxPathLength);
std::pair<std::vector<position>, int> bestPath(position from, int maxPathLength);
std::pair<std::vector<position>, int> upgradePath(position from, int maxPathLength);

std::vector<position> choosePath(CheckPosFunc startCheck, PathFunc pathFunc, CheckPathFunc alreadyConstructed);

#endif // PATHFINDING_H
