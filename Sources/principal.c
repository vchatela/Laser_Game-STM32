#include "gassp72.h"
#include "../etat.h" //pour le son
#include "Affichage_Valise.h"
#include "stdlib.h" 

int m2(unsigned short[], int);
unsigned short int dma_buf[64];
int occurence[6] = {0,0,0,0,0,0};
int compteur_tir[6] = {0,0,0,0,0,0};
int k[6] = {17, 18, 19, 20, 23, 24}; //cf calcul des rangs pour les fq 85,90,95,100,115 et 120 kHz
int bool_tir[6]={0,0,0,0,0,0};
int premier_tir = 0;
int tab_M2[6];
int M2TIR =10000000; //1000000; 	// on calcule une valeur théorique correspondant a 100mv crète a crète, il se pourra qu'on la change empiriquement
			// voir en bas de partie 2 pour deroulement calcul (plus note

/* ON A MULTIPLIE PAR 10 LA VALEUR DES RESISTANCES
*/
int timerLED = 0;
int finTimerLED = 200; //alumer 1sec (periode 200ms)
int SYSTICK_PER = 72000000/200; // periode de traitement : 5ms ->  200Hz -> pour 72MHz d'horloge donc il faut 72000/0.2 = 360000 

//Son
type_etat etat;
int offset = 0;
void son_callback(void);

//son1
extern int PeriodeSonMicroSec;
extern short Son; //short car 16 bits
extern int LongueurSon;
//son2
extern int PeriodeSonMicroSec2;
extern short Son2; //short car 16 bits
extern int LongueurSon2;
/* 
 * Permet de determiner aléatoirement la cible effective
 */
void ChoixCible(){
	int emplacement=(rand() % 4) +1 ;  // un nb entre 1 et 4
	int i;	
	// on change de cible et on allume la led correspondant a emplacement
	Choix_Capteur((char) emplacement);
	for (i =0; i<4; i++)
		Prepare_Clear_LED((char) i);
	Prepare_Set_LED((char) emplacement-1);
	Mise_A_Jour_Afficheurs_LED();
}

/* 
 * A chaque coup du timer : 
 * - vérifier si on a tiré sur une cible
 * - alumer la LED de la carte pendant 1sec
 * - gérer l'affichage de l'oscillo
 */
void extern sys_callback(){
	int i=0;
	// on gérera l'affichage a l'oscilo
	GPIO_Set(GPIOB, 1);
	timerLED++;

	// acquisition des données
	Start_DMA1(64); // démarrage du DMA pour les 64 points
	Wait_On_End_Of_DMA1();
	Stop_DMA1;
	// calcul des DFT M2(k) 
	for (i=0; i< 6; i++){
		tab_M2[i]=m2(dma_buf, k[i]);
		// maj des compteurs
		if (tab_M2[i] > M2TIR)
			occurence[i]+=1;
		else{
			occurence[i]=0;
			bool_tir[i]=0;
			if(timerLED == finTimerLED) {
				GPIO_Clear(GPIOA, 3);
				timerLED=0;
			}
		}
	}	
	GPIO_Clear(GPIOB, 1);
}


int main(void) {
	/*******************************
	*	INITIALISATION		*
	********************************/
	int i;	           //indice de boucle
	int fin_jeu = 0;   // booleen signifiant si le jeu est terminé
	int finduJeu = 10; //nombre de tirs avant la fin du jeu

	int Periode_en_Tck = 72*PeriodeSonMicroSec; // 72000000/1000000*Periode      -> période d'échantillonage (Tech)
	// 72/3 car on veut prendre 3 echantillions PWD pour 1 échantillonage donc 3 fois la fréquence
	int Periode_PWM_en_Tck = 24*PeriodeSonMicroSec;
	// activation de la PLL qui multiplie la fréquence du quartz par 9
	CLOCK_Configure();
	// PA2 (ADC voie 2) = entrée analog
	GPIO_Configure(GPIOA, 2, INPUT, ANALOG);
	// PB1 = sortie pour profilage à l'oscillo
	GPIO_Configure(GPIOB, 1, OUTPUT, OUTPUT_PPULL);
	// PA3 = sortie pour LED
	GPIO_Configure(GPIOA, 3, OUTPUT, OUTPUT_PPULL);

	// activation ADC, sampling time 1us
	Init_TimingADC_ActiveADC_ff( ADC1, 72 );
	Single_Channel_ADC( ADC1, 2 );
	// Déclenchement ADC par timer2, periode (72MHz/320kHz)ticks
	Init_Conversion_On_Trig_Timer_ff( ADC1, TIM2_CC2, 225 );
	// Config DMA pour utilisation du buffer dma_buf (a créér)
	Init_ADC1_DMA1( 0, dma_buf );

	// Config Timer, période exprimée en périodes horloge CPU (72 MHz)
	Systick_Period_ff( SYSTICK_PER );
	// enregistrement de la fonction de traitement de l'interruption timer
	// ici le 3 est la priorité, sys_callback est l'adresse de cette fonction, a créér en C
	Systick_Prio_IT( 3, sys_callback );
	SysTick_On;
	SysTick_Enable_IT;

	//Son
	// config port PB0 pour être utilisé par TIM3-CH3
	GPIO_Configure(GPIOB, 0, OUTPUT, ALT_PPULL);
	/*Initialisation de etat*/
	etat.son = &Son; // initialisation de l'@ de début du son (de l'asm)
	etat.position = 4512;
	etat.taille = LongueurSon;
	// config TIM3-CH3 en mode PWM
	etat.resolution = PWM_Init_ff( TIM3, 3, Periode_PWM_en_Tck );   //valeur max de la largeur de limpulsion
	etat.periode_ticks= Periode_en_Tck;

	// initialisation du timer 4
	// Periode_en_Tck doit fournir la durée entre interruptions,
	// exprimée en périodes Tck de l'horloge principale du STM32 (72 MHz)
	Timer_1234_Init_ff( TIM4, Periode_en_Tck );
	// enregistrement de la fonction de traitement de l'interruption timer
	// ici le 2 est la priorité, timer_callback est l'adresse de cette fonction, a créér en asm,
	// cette fonction doit être conforme à l'AAPCS
	Active_IT_Debordement_Timer( TIM4, 2, son_callback );


	/*********************/
	/*     MAIN PROG     */
	/*********************/
	Init_Affichage();
	for (i=0; i<4; i++){
		Prepare_Afficheur((char) i+1, (char) 0);
		Prepare_Clear_LED((char) i+1);
	}
	Choix_Capteur((char) 4);
	Prepare_Set_LED((char) 3);
	Mise_A_Jour_Afficheurs_LED();
	while(1){
		int i; //indice de boucle
		if (premier_tir)
			srand(timerLED);
		
		
		// verifie les compteurs
		for (i=0; i< 6; i++){
			if (occurence[i] == 3 && bool_tir[i]==0){
				if (!premier_tir)
					premier_tir = 1;
				//affichage de la LED
				GPIO_Set(GPIOA, 3);
				bool_tir[i]=1;
				// On joue le son1 (lancement son)
				etat.position = 0;
				Run_Timer( TIM4 );
				//On incremente le compteur de tir
				compteur_tir[i]++;
				if (i<4){
					Prepare_Afficheur((char) i+1, (char) compteur_tir[i]);
				}
				ChoixCible();
			}
			// on regarde si qqu a atteind le score max
			if (compteur_tir[i] == finduJeu){
				fin_jeu = 1;
				Prepare_Afficheur((char) i+1, (char) 99);
				break;
			}
		}
		if(fin_jeu){
			// on arrete le jeu
			// on affiche les 4 leds
			for (i=0; i<4; i++){
				Prepare_Set_LED((char) i);
			}
			Mise_A_Jour_Afficheurs_LED();
			
			break;
		}
	}

	// On joue le second son à la fin du jeu
	etat.son = &Son2; // initialisation de l'@ de début du son2 (de l'asm)
	etat.taille = LongueurSon2;
	etat.resolution = PWM_Init_ff( TIM3, 3, Periode_PWM_en_Tck );   //valeur max de la largeur de limpulsion
	etat.periode_ticks= Periode_en_Tck;
	etat.position = 0;
	while(1){};

}	
