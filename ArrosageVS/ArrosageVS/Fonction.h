// Fonction.h

#ifndef _FONCTION_h
#define _FONCTION_h

#include <LiquidCrystal.h>

//Definition des connections hardware
#define RELAIS  7         //pin de connection pour le relais
#define SONDEVCC 13 //Courant vers la sonde
#define SONDEHUMIDITE A0  //pin de connection pour la sonde d'humidité
#define BOUTON1 8 //Bouton +
#define BOUTON2 9 //bouton -
#define BOUTON3 10 //bouton selection
#define DELAI_MESURES 30000 //Temps entre les prises de mesures

/**************Prototypes de fonction********************/

int arrosage(int* taux_humidite,int temps_arrosage,int relais,int* arrosages_consec,int sonde_vcc, int sonde_analog,int* deja_arrose,int*selection,unsigned long horloge_principale,unsigned long* horloge_delai_mesures );
void effacer_ligne(int nCol, int nLigne);
int affichage_selection1(int taux_humidite, int arrosage_set);
int affichage_selection2(int temps_arrosage);
int affichage_selection3(unsigned long timer1,unsigned long timer4,unsigned long delai_arrosage);
int affichage_defaut();
int modifier_arrosage_set(int* arrosage_set);
void modifier_temps_arrosage(int *temps_arrosage,int bouton);
void affichage_serial(int bouton1, int bouton2, int bouton3, int selection, int taux_humidite,int arrosages_consec,int deja_arrose,unsigned long timer1,unsigned long timer4);

#if defined(ARDUINO) && ARDUINO >= 100
	#include "Arduino.h"
#else
	#include "WProgram.h"
#endif


#endif
