#ifndef MAP_H
#define MAP_H

#include "utils.h"

enum WeaknessState
{
    NotComputed,
    Weak,
    Strong
};
extern std::vector<std::vector<WeaknessState> > weaknessMap;

void generateWeakMap();
int computeWeakness(position pos, int depth, position parent);

extern std::vector<std::vector<double> > absoluteFluxMap;
void generateFluxMap();
double computePlasmaFlux(position pos, position from);
double getAveragePointEarning();

#endif // MAP_H
