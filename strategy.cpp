#include "strategy.h"
#include "map.h"
#include "pathfinding.h"
#include <iomanip>
#include <algorithm>

std::clock_t startTurn;

// Cette fonction implémente la stratégie de l'IA
void strategy()
{
    startTurn = std::clock();

    // On commence par générer nos cartes qui serviront pour les calculs
    generateWeakMap();
    generateFluxMap();

    // On attaque (ou pas) notre adversaire
    attackOpponentPipe();
    attackWeakOpponentPipe();

    std::clock_t currentTime = std::clock();
    double usedSeconds = double(currentTime-startTurn)/CLOCKS_PER_SEC;
    double secondsLeft = 1.0-usedSeconds;
    std::cout << "Time left : " << std::setprecision(10) << secondsLeft << std::endl;

    // On construit nos tuyaux
    while(points_action() > 0)
    {
        if(!makePipes()) break;
        std::clock_t currentTime = std::clock();
        if(double(currentTime-startTurn)/CLOCKS_PER_SEC > secondsLeft/2) break;
    }

    // On optimise (gratuitement) les points d'aspiration
    optimizeAspiration();

    std::cout << "PA left : " << points_action() << std::endl;

    // On upgrade nos tuyaux s'il nous reste des PA's
    while(points_action() > 0)
    {
        std::clock_t currentTime = std::clock();
        if(double(currentTime-startTurn)/CLOCKS_PER_SEC > secondsLeft/2) break;
        if(!upgradePipes()) break;
    }

    std::clock_t endTurn = std::clock();
    std::cout << "Total turn time : " << std::setprecision(10) << double(endTurn-startTurn)/CLOCKS_PER_SEC << std::endl;
    std::cout << "== Fin de tour ==" << std::endl;
}

// Cette fonction est chargée de construire les tuyaux
// Pour celà on fait un Dijkstra depuis chaque coté de pulsar et on prend le plus utile.
bool makePipes()
{
    auto alreadyConstructed = [](std::vector<position> path)
    {
        for(position pos : path)
        {
            if(!est_tuyau(pos)) return false;
        }
        return true;
    };

    std::vector<position> toBuild = choosePath(isBuildable, bestPath, alreadyConstructed);
    if(toBuild.empty()) return false;

    for(position p : toBuild) printPos(p);
    std::cout << std::endl;

    // On essaye de construire le tuyau jusqu'à ce qu'on n'ait plus de PA's
    for(int i = toBuild.size()-1 ; i >= 0 ; i--)
    {
        position p = toBuild[i];

        if(est_tuyau(p)) continue;
        else if(points_action() == 0) return false;
        else if(est_debris(p))
        {
            if(points_action() < COUT_DEBLAYAGE) return false;
            deblayer(p);
        }

        construire(p);
    }

    return true; // Si on a fini de construire tout le tuyau, on demande une deuxième phase de construction (si on a le temps)
}

// Optimisation (basique) de l'aspiration
void optimizeAspiration()
{
    // On récupère un point d'une base non reliée pour le mettre sur une base reliée

    position usefullBase = position{0,0};
    position uselessBase = position{0,0};
    for(position p : ma_base())
    {
        bool linked = false;
        for(position dir : DIRECTIONS)
        {
            if(est_tuyau(p+dir)) linked = true;
        }

        if(linked && puissance_aspiration(p) != LIMITE_ASPIRATION) usefullBase = p;
        else if(!linked && puissance_aspiration(p) > 0) uselessBase = p;
    }

    if(usefullBase != position{0,0} && uselessBase != position{0,0})
    {
        deplacer_aspiration(uselessBase, usefullBase);
    }
}

// Attaque (ou choix de non attaque) des tuyaux ennemis
void attackOpponentPipe()
{
    // On recherche le meilleur tuyau à détruire
    position bestPipeToDestroy;
    double bestPipeScore = 0;

    // A priori le meilleur tuyau à détruire a un flux élevé
    std::vector<position> pipeList;
    auto fluxComp = [](position t1, position t2){ return absoluteFluxMap[t1.x][t1.y] > absoluteFluxMap[t2.x][t2.y]; };
    std::sort(pipeList.begin(), pipeList.end(), fluxComp);
    int currentGain = getAveragePointEarning();

    for(position pos : pipeList)
    {
        // On évite de timeout pour la recherche d'une cible
        std::clock_t currentTime = std::clock();
        if(double(currentTime-startTurn)/CLOCKS_PER_SEC > 0.3) break;

        // Si le tuyau est "faible" on a d'autant plus intéret à le détruire
        double scoreMultiplicator = 1;
        if(weaknessMap[pos.x][pos.y] == Weak) scoreMultiplicator = 2;

        // Backtrack pour déterminer la différence de flux entrant dans la base adverse suite à la destruction
        detruire(pos);
        generateFluxMap();
        int gainAfterDestruction = getAveragePointEarning();
        annuler();
        int diffGain = gainAfterDestruction - currentGain;

        // On favorise la coupure là ou l'ennemi perd en plus des plasmas
        int plasmaScore = 0;
        for(position dir : DIRECTIONS)
        {
            int plasma = charges_presentes(pos+dir);
            if(plasma == 0) continue;

            bool plasmaTowardsPipe = false;
            std::vector<position> plasmaDirs = directions_plasma(pos+dir);
            for(position p : plasmaDirs)
            {
                if(p == pos)
                {
                    plasmaTowardsPipe = true;
                    break;
                }
            }

            if(plasmaTowardsPipe) plasmaScore += plasma/plasmaDirs.size();
        }

        // On préfère couper proche de l'adversaire et loin de nous
        int distToOpponent = distToBase(pos, adversaire());
        int distToSelf = distToBase(pos, moi());
        if(distToOpponent == 1) distToOpponent += 2; // On évite de casser trop proche de l'adversaire non plus
        double score = scoreMultiplicator * (plasmaScore+diffGain) * distToSelf / distToOpponent;

        if(score > bestPipeScore)
        {
            bestPipeScore = score;
            bestPipeToDestroy = pos;
        }
    }

    // On applique l'action si c'est sufisemment rentable
    if(bestPipeScore >= 3) detruire(bestPipeToDestroy);
    generateFluxMap();
}

// Attaque basique d'un opposant (se fait en plus de l'attaque traditionnelle)
// On détruit inconditionellement un tuyau utile à la racine d'une base enemie.
void attackWeakOpponentPipe()
{
    int nbOpponentPipeTopLeft = 0;
    int nbOpponentPipeBottomRight = 0;
    position pipeTopLeft;
    position pipeBottomRight;
    for(position p : base_ennemie())
    {
        for(position dir : DIRECTIONS)
        {
            position pos = p+dir;
            if(est_tuyau(pos) && absoluteFluxMap[pos.x][pos.y] != 0) // Un tuyau est utile si son flux est non nul
            {
                if(p.x == 0 || p.y == 0)
                {
                    nbOpponentPipeTopLeft++;
                    pipeTopLeft = pos;
                }
                else
                {
                    nbOpponentPipeBottomRight++;
                    pipeBottomRight = pos;
                }
            }
        }
    }
    if(nbOpponentPipeTopLeft == 1) detruire(pipeTopLeft);
    else if(nbOpponentPipeBottomRight == 1) detruire(pipeBottomRight);
}

bool upgradePipes()
{
    // Fonction de vérification que le chemin n'est pas déjà fait
    auto alreadyUpgraded = [](std::vector<position> path)
    {
        for(position pos : path)
        {
            if((pos.x + pos.y) % 2 && !est_super_tuyau(pos)) return false;
        }
        return true;
    };

    std::vector<position> toBuild = choosePath(est_tuyau, upgradePath, alreadyUpgraded);
    if(toBuild.empty()) return false;

    for(position p : toBuild) printPos(p);
    std::cout << std::endl;

    for(int i = toBuild.size()-1 ; i >= 0 ; i--)
    {
        position p = toBuild[i];
        if((p.x + p.y) % 2) ameliorer(p);
    }

    return true;
}
