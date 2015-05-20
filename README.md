# Laser_Game-STM32
Description générale :

Le projet consiste à réaliser un jeu de tir laser sur cible. Plusieurs pistolets laser seront à disposition (jusqu'à 6), le jeu consistera à viser une ou plusieurs cibles, et d'obtenir en fin de partie les scores de chaque joueur.

Chaque pistolet a sa propre "signature" (une fréquence allouée, connue). Votre objectif sera :

- de concevoir et mettre au point l'électronique de traitement du capteur en réception
- d'analyser théoriquement et de mettre en oeuvre un procédé de traitement de signal pour distinguer efficacement les pistolets
- de concevoir et mettre au point le logiciel, en assembleur, qui permettra de trier les joueurs, de compter les points, et d'émettre un son rigolo dès qu'un joueur fait mouche.

!! ATTENTION !!

La pile USB utilise une interruption régulièrement déclenchée de priorité niveau 8. La fonction Send_Byte_On_Received_Byte doit être exploitée dans le main (tâche de fond while(1)) et non dans
une interruption, sinon l'USB plante.

La fonction void Mise_A_Jour_Afficheurs_LED(void) utilise une interruption de priorité 8. Il n'est donc pas possible de l'utiliser dans une fonction d'interruption de niveau plus prioritaire. En effet, l'IT de la fonction de mise à jour ne pourra alors jamais être traitée (plantage de la fonction).

Solution :

- Placer cette fonction en tâche de fond

- Placer cette fonction dans une IT mais de priorité plus faible (>8).
