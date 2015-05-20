
/* Fichier devant contenir l'ensemble de fichiers utiles pour le projet LaserQuest
et qui concenent l'affichage sur la valise */
/* mais non disponible en version source pour les étudiants.						*/

#ifndef _AFFICHAGE_VALISE_H__
#define _AFFICHAGE_VALISE_H__
#include "stm32f10x.h"


void Init_Affichage(void);
void Prepare_Afficheur(char Aff, char Valeur);
void Prepare_Set_Point_Unite(char Aff);
void Prepare_Clear_Point_Unite(char Aff);
void Prepare_Set_LED(char LED);
void Prepare_Clear_LED(char LED);
void Choix_Capteur(char Capteur);
// define utile pour la fonction Prepare_Set_LED et Prepare_Clear_LED
#define LED_LCD_R 5
#define LED_LCD_V  4
#define LED_Cible_4 3
#define LED_Cible_3 2
#define LED_Cible_2 1
#define LED_Cible_1 0

void Mise_A_Jour_Afficheurs_LED(void);

#endif

