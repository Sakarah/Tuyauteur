#ifndef UTILS_H
#define UTILS_H

#include "prologin.hh"
#include <limits>

const int INFINITY = std::numeric_limits<int>::max();
const position DIRECTIONS[4] = {position{1,0}, position{-1,0}, position{0,1}, position{0,-1}};

inline position operator+(const position p1, const position p2) { return position{p1.x+p2.x,p1.y+p2.y}; }
inline position operator-(const position p1, const position p2) { return position{p1.x-p2.x,p1.y-p2.y}; }
inline bool operator!=(const position p1, const position p2) { return !(p1==p2); }
inline int manhattanDist(position p1, position p2) { return abs(p1.x-p2.x)+abs(p1.y+p2.y); }

bool isBuildable(position p);
bool inMyBase(position p);
bool connectedToMyBase(position p);
int nbPulsations(pulsar_info p, int turn);

#include <iostream>
inline void printPos(position p) { std::cout << "(" << p.x << "," << p.y << ")"; }

#endif // UTILS_H
