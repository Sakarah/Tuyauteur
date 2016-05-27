#include "map.h"
#include <iomanip>

std::vector<std::vector<WeaknessState> > weaknessMap;
std::vector<std::vector<int> > depthMap;

// Génère la carte des segments faibles. Un segment est dit "faible" lorsque il est une arrête essentielle dans notre réseau.
void generateWeakMap()
{
    weaknessMap = std::vector<std::vector<WeaknessState>>(TAILLE_TERRAIN, std::vector<WeaknessState>(TAILLE_TERRAIN, NotComputed));
    depthMap = std::vector<std::vector<int>>(TAILLE_TERRAIN, std::vector<int>(TAILLE_TERRAIN, 0));
    for(position p : ma_base()) computeWeakness(p,0,p);
}

// On génère cette carte par DFS : lorsqu'on trouve un cycle on déclare fort tout le cycle
int computeWeakness(position pos, int depth, position parent)
{
    if(weaknessMap[pos.x][pos.y] == Weak || weaknessMap[pos.x][pos.y] == Strong) return depthMap[pos.x][pos.y];
    weaknessMap[pos.x][pos.y] = Weak;
    depthMap[pos.x][pos.y] = depth;

    int minDepthSafe = depth+1;
    for(position dir : DIRECTIONS)
    {
        if(pos+dir == parent) continue;
        if(est_tuyau(pos+dir) || inMyBase(pos+dir))
        {
            minDepthSafe = std::min(minDepthSafe, computeWeakness(pos+dir, depth+1, pos));
        }
    }

    if(minDepthSafe <= depth) weaknessMap[pos.x][pos.y] = Strong;
    return minDepthSafe;
}



std::vector<std::vector<double> > absoluteFluxMap;
// Génère la carte des flux absolus : un flux absolu correspond au nombre d'unité de plasma qui passent en moyenne sur le tuyau.
void generateFluxMap()
{
    absoluteFluxMap = std::vector<std::vector<double>>(TAILLE_TERRAIN, std::vector<double>(TAILLE_TERRAIN, -1.));

    for(position pulsar : liste_pulsars())
    {
        pulsar_info info = info_pulsar(pulsar);
        if(info.pulsations_restantes == 0) absoluteFluxMap[pulsar.x][pulsar.y] = 0;
        else absoluteFluxMap[pulsar.x][pulsar.y] = info.puissance/info.periode;
    }

    for(position p : ma_base()) computePlasmaFlux(p,p);
    for(position p : base_ennemie()) computePlasmaFlux(p,p);
}

// On génère la carte par DFS
double computePlasmaFlux(position pos, position from)
{
    if(est_pulsar(pos)) return absoluteFluxMap[pos.x][pos.y]; // Si on est un pulsar on donne directement son flux aux cases adjacentes
    if(est_libre(pos) || est_debris(pos) || type_case(pos) == INTERDIT) return 0;

    std::vector<position> givePlasmaTo = directions_plasma(pos);
    bool willGivePlasmaToParent = false;
    for(position p : givePlasmaTo)
    {
        if(p == from)
        {
            willGivePlasmaToParent = true;
            break;
        }
    }

    if(absoluteFluxMap[pos.x][pos.y] == -1)
    {
        absoluteFluxMap[pos.x][pos.y] = 0;

        // Chaque tuyau peut soit recevoir soit donner du plasma aux cases adjacentes.
        std::vector<position> getPlasmaFrom;
        for(position dir : DIRECTIONS)
        {
            if(!est_tuyau(pos+dir) && !est_pulsar(pos+dir)) continue;
            bool give = false;
            for(position p : givePlasmaTo)
            {
                if(p == pos+dir)
                {
                    give = true;
                    break;
                }
            }
            if(!give) getPlasmaFrom.push_back(pos+dir);
        }
        if(givePlasmaTo.empty() && type_case(pos) != BASE) return 0;

        // On calcule le plasma reçu par la case
        for(position p : getPlasmaFrom)
        {
            double enteringFlux = computePlasmaFlux(p, pos);
            absoluteFluxMap[pos.x][pos.y] += enteringFlux;
        }
        // On le divise pour tous les "fils"
        if(type_case(pos) != BASE) absoluteFluxMap[pos.x][pos.y] /= givePlasmaTo.size();
        // On parcourt les tuyaux receveurs
        for(position p : givePlasmaTo) computePlasmaFlux(p, pos);
    }

    if(willGivePlasmaToParent) return absoluteFluxMap[pos.x][pos.y];
    else return 0;
}

// A partir de la carte des flux, on peut obtenir directement le gain moyen d'un joueur et donc en déduire le score relatif ²moyen gagné par tour
double getAveragePointEarning()
{
    double avgPtsEarning = 0;
    for(position p : ma_base()) avgPtsEarning += absoluteFluxMap[p.x][p.y];
    for(position p : base_ennemie()) avgPtsEarning -= absoluteFluxMap[p.x][p.y];
    return avgPtsEarning;
}
