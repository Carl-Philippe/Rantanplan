// Fonction.h

#ifndef _FONCTION_h
#define _FONCTION_h

#include <LiquidCrystal.h>


 
//Definition des connections hardware
#define RELAIS_A  7         //pin de connection pour le RELAIS_A (arrosage)
#define SONDEVCC 13 //Courant vers la sonde
#define SONDEHUMIDITE A0  //pin de connection pour la sonde d'humidit�
#define BOUTON1 8 //Bouton +
#define BOUTON2 9 //bouton -
#define BOUTON3 10 //bouton selection


/**************Prototypes de fonction********************/

int arrosage(float* taux_humidite,long temps_arrosage,int relais,int* arrosages_consec,int sonde_vcc, int sonde_humidite,int* deja_arrose,int*selection);
void effacer_ligne(int nCol, int nLigne);
int affichage_selection1(int taux_humidite, int arrosage_set);
int affichage_selection2(long temps_arrosage);
int affichage_selection3(unsigned long timer1,unsigned long timer4,unsigned long delai_arrosage);
int affichage_defaut(float taux_humidite);
int modifier_arrosage_set(int* arrosage_set);
void modifier_temps_arrosage(int *temps_arrosage,int bouton);
void affichage_serial(int bouton1, int bouton2,
					  int bouton3, int selection, int taux_humidite,int arrosages_consec,int deja_arrose,unsigned long timer1,unsigned long timer2,unsigned long timer3,unsigned long timer4);

#if defined(ARDUINO) && ARDUINO >= 100
	#include "Arduino.h"
#else
	#include "WProgram.h"
#endif


#endif
