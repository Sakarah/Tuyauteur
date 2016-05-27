/// This file has been generated, if you wish to
/// modify it in a permanent way, please refer
/// to the script file : gen/generator_cxx.rb

#include "strategy.h"

/* Tuyauteur - Projet d'extracteur ProVogon en (seulement) 2 dimentions
 * ====================================================================
 * Cette IA a pour objectif d'optimiser l'extraction du plasma des pulsars.
 * Pour celà elle se base principalement sur un Dijkstra pour relier la base aux pulsars.
 * On cherche à éviter au maximum d'avoir des chemins faibles au sens où comme beaucoup de ressources en dépendent,
 * des méchants pirates pourraient être tentés de les détruire pour gagner le monopole.
 * Ainsi on cherche ses noeuds essentiels et on leur attribue des poids plus élevés.
 * Etant donné que nous avons un concurrent VoLogin il parrait naturel de vouloir repérer le centre névralgique de leur approvisionnement
 * afin de récupérer ces ressources qui nous seront bien plus utiles.
 * On calcule donc un flux qui correspond aux nombre moyen de ressources qui passent par un segment de pipeline.
 * Le reste des détails se situe en commentaire des fonctions ci-jointes.
 */

/// Fonction appelée au début de la partie.
void partie_init()
{
  // fonction a completer
}

/// Fonction appelée à chaque tour.
void jouer_tour()
{
    strategy();
}

/// Fonction appelée à la fin de la partie.
void partie_fin()
{
  // fonction a completer
}

