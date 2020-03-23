/*
 Name:		Sketch1.ino
 Created:	28/01/2018 15:37:50
 Author:	Carl-Philippe Cyr
*/

#include "Fonction.h"
#include "format check.h"


LiquidCrystal affichage(12, 11, 2, 3, 4, 5); //LiquidCrystal nom des entrees hardware: RS, E, d4, d5, d6, d7

int modif=0;
int deja_arrose=0; 
int affichage_courant=0;
int arrosages_simul = 0;
int arrosage_set = 700;
int temps_arrosage=8000;
int arrosages_consec=0;
int taux_humidite=4321;
int rafraichissement=10000;
unsigned long reset=14400000;	//4h
unsigned long horloge_principale;		//timer de base
unsigned long horloge_menu;		//timer menu deroulant
unsigned long horloge_arrosages_consec;		//timer reset arrosages consec
unsigned long horloge_delai_arrosages;		//timer delai apres arrosages
unsigned long horloge_delai_mesures; //timer delai de la prise de mesures

boolean bouton1=1;
boolean bouton2=1;
boolean bouton3=1;
boolean bouton3a=1;
int selection=0;
unsigned long delai_arrosage=600000;	//10min minimum entre les arrosages 



/***Déclaration entrées sorties, initialisation arduino et LCD***/
void setup()
{
  Serial.begin(9600);
  

  pinMode(RELAIS, OUTPUT);        //Signale à l’Arduino que la connexion 7 doit pouvoir envoyer du courant
  pinMode(SONDEVCC,OUTPUT);			//La connexion 13 doit pouvoir envoyer du courant
  pinMode(SONDEHUMIDITE, INPUT);  //Signale que l'analogue A0 peut recevoir de l'information
  pinMode(BOUTON1, INPUT_PULLUP); //digital 8 reçoit un signal et utilise une resistance de pullup - bouton +
  pinMode(BOUTON2, INPUT_PULLUP); //digital 9 reçoit un signal et utilise une resistance de pullup - bouton -
  pinMode(BOUTON3, INPUT_PULLUP); //digital 10 reçoit un signal et utilise une resistance de pullup - bouton menu

  affichage.begin(16, 2); //Grandeur de la matrice de communication (16 colonnes 2 lignes)
  affichage.setCursor(0, 0);
  affichage.print(" Ici Rantanplan ");
  affichage_serial(bouton1,bouton2,bouton3,selection,taux_humidite,arrosages_consec,deja_arrose, horloge_principale, horloge_delai_arrosages);
  horloge_principale=millis();	//initialisation des timers			//timer 1 tiens le decompte du temps en continu
  horloge_menu=millis();										//timer 2 tiens un decompte si on ne modifie rien
  horloge_arrosages_consec=millis();										//timer 3 tiens un decompte du temps depuis le reset de arrosages_consec
  horloge_delai_arrosages=millis();
  horloge_delai_mesures=0;
 }

void loop()
{
	
	affichage_serial(bouton1,bouton2,bouton3,selection,taux_humidite,arrosages_consec,deja_arrose, horloge_principale, horloge_delai_arrosages);

	bouton3a = digitalRead(BOUTON3);	//Debounce du bouton selection
	if(bouton3a != bouton3)
		bouton3 = digitalRead(BOUTON3);
	else
		bouton3 = 1;

	horloge_principale=millis();	//timer faisant le compte du temps en continu
	delay(125);	
	if(!bouton3){	//si le bouton de selection est appuye
		selection++;	//incremente le menu pour le switchcase
		horloge_menu=horloge_principale;	//Commence le decompte pour les menus sans modif
	}

	switch (selection) {
		case 0 :
					if(affichage_courant!=0)				//On reaffiche seulement si les donnees a afficher ont changees
						affichage_courant=affichage_defaut();
					if(arrosages_consec<3)
						arrosage(&taux_humidite,temps_arrosage,RELAIS,&arrosages_consec,SONDEVCC,SONDEHUMIDITE,&deja_arrose,&selection,horloge_principale,&horloge_delai_mesures);
			break;

		case 1 :
					arrosage(&taux_humidite,temps_arrosage,RELAIS,&arrosages_consec,SONDEVCC,SONDEHUMIDITE,&deja_arrose,&selection,horloge_principale,&horloge_delai_mesures);
					modif=modifier_arrosage_set(&arrosage_set);//Verifie si la valeur d'arrosage_set a changee
					if(affichage_courant!=1 || modif ==1){		//si les donnees a afficher ont changees
						affichage_courant =	affichage_selection1(taux_humidite,arrosage_set);	//On reaffiche
						horloge_menu = horloge_principale;		//timer 2 tiens un decompte si on ne modifie rien
						modif=0;
					}
			break;

		case 2 :
					arrosage(&taux_humidite,temps_arrosage,RELAIS,&arrosages_consec,SONDEVCC,SONDEHUMIDITE,&deja_arrose,&selection,horloge_principale,&horloge_delai_mesures);
					modif=modifier_temps_arrosage(&temps_arrosage); //Verifie si temps_arrosage a change
					if(affichage_courant!=2 || modif==1){		//si les donnees a afficher ont changees
						affichage_courant =	affichage_selection2(temps_arrosage);	//On reaffiche
						horloge_menu = horloge_principale;		//timer 2 tiens un decompte si on ne modifie rien
						modif=0;
					}
					
			break;

		case 3 :
					if(horloge_delai_arrosages<=0)
					{
						horloge_delai_arrosages=horloge_principale;
					}
					while(horloge_principale-horloge_delai_arrosages < delai_arrosage){
						delay(50);
					if(deja_arrose==0)
						break;
					
					//affichage_serial(bouton1,bouton2,bouton3,selection,taux_humidite,arrosages_consec,deja_arrose, horloge_principale, horloge_delai_arrosages);
					horloge_principale=millis();
					affichage_courant=affichage_selection3(horloge_principale,horloge_delai_arrosages,delai_arrosage);
					Serial.print(horloge_principale-horloge_delai_arrosages);
					}
					horloge_menu=horloge_principale;
					deja_arrose=0;
					horloge_delai_arrosages =0;
					selection=0;
			break;
	}




	if((horloge_principale-horloge_menu > rafraichissement && deja_arrose==0) || (selection>3 && deja_arrose==0))
		selection=0;
	if(deja_arrose==1 && selection>3)
		selection=0;

	if(horloge_principale-horloge_arrosages_consec > reset){
		arrosages_consec=0;
		horloge_menu=horloge_principale;
		horloge_arrosages_consec=horloge_principale;
		horloge_delai_arrosages=horloge_principale;
	}
}