Tuyauteur est une IA codée en moins de 36 heures lors de l'édition Prologin de 2016.

Pour la tester il faut suivre les instructions de https://bitbucket.org/prologin/stechec2 pour lancer des parties sur le jeu prologin2016.

Cette IA tente de construire de la meilleure façon possible un réseau de tuyaux pour récolter un maximum d'énergie depuis les pulsars.
Elle s'appuie principalement sur un Dijkstra afin de déterminer une méthode pour relier la base aux pulsars tout en évitant d'employer des chemins 'faibles' pour lesquels une seule action de l'adversaire suffirait à les détruire.
Ainsi les noeuds essentiels dans le réseau sont repérés et en partie évités.
Coté attaque, l'algorithme essaye de repérer les tuyaux les plus utiles à l'adversaire par une méthode de backtracking afin de les détruire.
De plus on choisit de systématiquement détruire un accès unique à sa base pour l'empêcher de récolter du plasma.

Au classement de Prologin, cette IA est arrivée 8ème.

L'ensemble de son code à l'exception des fichiers prologin.hh, prologin.cc et Makefile (sous GPLv2) est sous license Apache 2.0.
