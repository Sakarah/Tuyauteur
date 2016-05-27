#include "utils.h"

// Renvoie true si la case est éventuellement constructible
bool isBuildable(position p)
{
    if(p.x <= 0 || p.x >= TAILLE_TERRAIN-1) return false;
    if(p.y <= 0 || p.y >= TAILLE_TERRAIN-1) return false;
    if(est_pulsar(p)) return false;
    return true;
}

// Renvoie true si la case est dans notre base
bool inMyBase(position p)
{
    return type_case(p) == BASE && proprietaire_base(p) == moi();
}

// Renvoie true si le tuyau est connecté à notre base
bool connectedToMyBase(position p)
{
    for(position dir : DIRECTIONS)
    {
        if(inMyBase(p+dir)) return true;
    }
    return false;
}

// Renvoie le nombre de pulsations qui se dérouleront pendant les turn prochains tours
int nbPulsations(pulsar_info p, int turn)
{
    int turnsToNextPulse = tour_actuel() % p.periode;
    turn -= turnsToNextPulse;
    if(turn < 0) return 0;
    else return turn/p.periode;
}
