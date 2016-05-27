#include "pathfinding.h"
#include "map.h"
#include <queue>

// BFS pour déterminer la distance à la base la plus proche
int distToBase(position from, int owner)
{
    std::vector<position> toExplore;
    std::vector<position> toExploreNext;
    std::vector<std::vector<bool>> visited(TAILLE_TERRAIN, std::vector<bool>(TAILLE_TERRAIN, false));
    toExplore.push_back(from);

    int depth = 1;
    while(!toExplore.empty())
    {
        for(position pos : toExplore)
        {
            if(visited[pos.x][pos.y]) continue;
            visited[pos.x][pos.y] = true;

            for(position dir : DIRECTIONS)
            {
                position newPos = pos+dir;
                if(type_case(newPos) == BASE && proprietaire_base(newPos) == owner) return depth;
                if(!isBuildable(newPos)) continue;
                if(est_debris(newPos)) continue;
                toExploreNext.push_back(newPos);
            }
        }

        toExplore = toExploreNext;
        toExploreNext.clear();
        depth++;
    }

    return INFINITY;
}


struct DijkNode
{
    std::vector<position> path;
    int cost;
    int realDist;
    bool operator<(const DijkNode& o) const { return cost > o.cost; }
};

// Dijkstra générique pour faire des recherches de chemins.
// On part de from et on va vers n'importe quelle casr qui évalue canBeEndPos en true.
// Le cout de passage par un noeud est donné par computeCost.
// Toute case évaluant validatePos en false est systématiquement ignorée.
// De plus si la distance réelle dépasse maxPathLength, la case est ignorée également.
std::pair<std::vector<position>, int> dijkPath(position from, CheckPosFunc canBeEndPos, EvalPosFunc computeCost, CheckPosFunc validatePos, int maxPathLength)
{
    std::vector<std::vector<bool>> visited(TAILLE_TERRAIN, std::vector<bool>(TAILLE_TERRAIN, false));
    std::priority_queue<DijkNode> queue;
    queue.push(DijkNode{std::vector<position>(1,from),0});

    while(!queue.empty())
    {
        DijkNode node = queue.top();
        queue.pop();
        position currentPos = node.path.back();

        if(visited[currentPos.x][currentPos.y]) continue;
        visited[currentPos.x][currentPos.y] = true;
        if(node.realDist > maxPathLength) continue;
        if(canBeEndPos(currentPos)) return std::make_pair(node.path, node.cost);

        for(position dir : DIRECTIONS)
        {
            position newPos = currentPos+dir;

            if(!validatePos(newPos)) continue;

            std::vector<position> path = node.path;
            path.push_back(newPos);
            int cost = node.cost + computeCost(newPos);
            queue.push(DijkNode{path, cost, node.realDist+1});
        }
    }

    return std::make_pair(std::vector<position>(), INFINITY);
}

// Calcule le meilleur chemin à construire pour se relier au réseau
std::pair<std::vector<position>, int> bestPath(position from, int maxPathLength)
{
    auto computeCost = [](position p)
    {
        if(est_tuyau(p))
        {
            if(weaknessMap[p.x][p.y] == Weak) return 8; // Utiliser un tuyau "faible" est dangereux, on l'évite donc jusqu'à une certaine mesure
            else return 0; // Un tuyau bien sécurisé est par contre gratuit
        }
        if(est_debris(p)) return 27; // Les débris sont 3* plus difficiles à traverser qu'une case vide mais les contourner peut être pire
        else return 10; // Cout de base d'une case vide
    };
    return dijkPath(from, connectedToMyBase, computeCost, isBuildable, maxPathLength);
}

// Calcule le meilleur chemin pour upgrader le réseau
std::pair<std::vector<position>, int> upgradePath(position from, int maxPathLength)
{
    auto computeCost = [](position p)
    {
        // Mêmes coûts que pour la construction
        if(weaknessMap[p.x][p.y] == Weak) return 8;
        else return 0;
    };
    return dijkPath(from, connectedToMyBase, computeCost, est_tuyau, maxPathLength);
}

// Calcule le chemin le plus court d'un endroit à la base d'un ennemi en ne prenant que des tuyaux.
std::pair<std::vector<position>, int> opponentPipePath(position from)
{
    auto constantCost = [](position p) { return 1; }; // Pas de priorités ici
    auto canBeEndPos = [](position p)
    {
        for(position dir : DIRECTIONS)
        {
            if(type_case(p+dir) == BASE && proprietaire_base(p+dir) == adversaire()) return true;
        }
        return false;
    };
    return dijkPath(from, canBeEndPos, constantCost, est_tuyau, INFINITY);
}

// Choisi le chemin à construire / upgrader et le renvoie
std::vector<position> choosePath(CheckPosFunc startCheck, PathFunc pathFunc, CheckPathFunc alreadyConstructed)
{
    std::vector<position> chosenPath;
    double bestScore = 0;
    for(position pulsar : liste_pulsars())
    {
        pulsar_info info = info_pulsar(pulsar);
        if(info.pulsations_restantes == 0) continue; // Un pulsar vide ne sert à rien
        double pulsar_value = info.puissance/info.periode; // L'intérêt d'une case dépend du flux du pulsar qui l'émmet

        for(position dir : DIRECTIONS)
        {
            position pos = pulsar+dir;
            if(!startCheck(pos)) continue; // On s'assure que le départ est valide

            // Il ne sert à rien de faire un chemin plus long que celui de l'adversaire vers le même tuyau. On exclue cette possibilité.
            int maxLength = opponentPipePath(pos).second;

            std::pair<std::vector<position>, int> optiPathStruct = pathFunc(pos, maxLength);
            std::vector<position> optiPath = optiPathStruct.first;
            int optiLength = optiPathStruct.second;

            if(optiLength == INFINITY) continue;
            if(alreadyConstructed(optiPath)) continue;

            int leftPulses = info.pulsations_restantes - nbPulsations(info, optiLength/4);
            int distReal = distToBase(pos, moi());
            int pipeVal = optiLength+distReal;
            // L'intérêt d'un pulsar dépend de sa valeur des pulsations qu'il peut encore émettre et est inversement proportionnel à la distance.
            double score = pulsar_value*leftPulses/pipeVal/pipeVal;
            if(score >= bestScore)
            {
                bestScore = score;
                chosenPath = optiPath;
            }
        }
    }

    return chosenPath;
}
