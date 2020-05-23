/*
 Name:		Sketch1.ino
 Created:	28/01/2018 15:37:50
 Author:	Carl-Philippe Cyr
*/

#include "Fonction.h"

#define HUM_PIN A0

LiquidCrystal affichage(12, 11, 2, 3, 4, 5); //LiquidCrystal nom des entrees hardware: RS, E, d4, d5, d6, d7

int modif=0;
int deja_arrose=0; 
int affichage_courant=0;
int arrosages_simul = 0;
int arrosage_set = 650;
long temps_arrosage=30000;
int arrosages_consec=0;
float taux_humidite = 0.0;
int ventilation_set = 30;


int rafraichissement=8000;
int rafraich_menu =10000;
//unsigned long reset=14400000;
unsigned long timer1;		//timer de base
unsigned long timer2;		//timer menu deroulant
unsigned long timer3;		//timer reset arrosages consec
unsigned long timer4;		//timer delai apres arrosages
unsigned long timer5;		//timer delai entre les mesures
unsigned long delai_arrosage=300000;	//5min minimum entre les arrosages 

boolean bouton1=1;
boolean bouton2=1;
boolean bouton3=1;
int selection=0;




/***D�claration entr�es sorties, initialisation arduino et LCD***/
void setup()
{
  Serial.begin(9600);
  

  pinMode(RELAIS_A, OUTPUT);        //Signale � l�Arduino que la connexion 7 doit pouvoir envoyer du courant
  pinMode(SONDEVCC,OUTPUT);			//La connexion 13 doit pouvoir envoyer du courant
  pinMode(SONDEHUMIDITE, INPUT);  //Signale que l'analogue A0 peut recevoir de l'information
  pinMode(BOUTON1, INPUT_PULLUP); //digital 8 re�oit un signal et utilise une resistance de pullup - bouton +
  pinMode(BOUTON2, INPUT_PULLUP); //digital 9 re�oit un signal et utilise une resistance de pullup - bouton -
  pinMode(BOUTON3, INPUT_PULLUP); //digital 10 re�oit un signal et utilise une resistance de pullup - bouton menu

  digitalWrite(RELAIS_A,HIGH);
  taux_humidite = analogRead(HUM_PIN);	//Lis le taux d'humidite
  affichage.begin(16, 2); //Grandeur de la matrice de communication (16 colonnes 2 lignes)
  affichage.setCursor(0, 0);
  affichage.print("Ici ");
  affichage.setCursor(0, 1);
  affichage.print("Rantanplan ");
  delay(2000);
  affichage_serial(bouton1,bouton2,bouton3,selection,taux_humidite,arrosages_consec,deja_arrose, timer1, timer2, timer3, timer4);
  timer1=millis();	//initialisation des timers			//timer 1 tiens le decompte du temps en continu
  timer2=millis();										//timer 2 tiens un decompte si on ne modifie rien
  timer3=millis();										//timer 3 tiens un decompte du temps depuis le reset de arrosages_consec
  timer4=millis();
  timer4=0;
  timer5=millis();
 }


void loop()
{
	if((timer1 - timer5) >= 10000)
	{
  digitalWrite(SONDEVCC,HIGH);
  delay(20);
		taux_humidite = analogRead(HUM_PIN);	//Lis le taux d'humidite
   digitalWrite(SONDEVCC,LOW);
		timer5 = timer1;
	}

	affichage_serial(bouton1,bouton2,bouton3,selection,taux_humidite,arrosages_consec,deja_arrose, timer1, timer2,timer3, timer4);
	bouton3 = digitalRead(BOUTON3);

	timer1=millis();	//timer faisant le compte du temps en continu
	delay(125);	
	if(!bouton3){	//si le bouton de selection est appuye
		selection++;	//incremente le menu pour le switchcase
		timer2=timer1;	//Commence le decompte pour les menus sans modif
	}


	/* affichage ainsi qu'arrosage � intervals r�gl�s */
	switch (selection) {     
	
		case 0 : //affichage par defaut
					//if(affichage_courant!=0)	{			//On reaffiche seulement si les donnees a afficher ont changees
					if((timer1 - timer5) >= 5000){			
						affichage_courant = affichage_defaut(taux_humidite);
					}
					if(deja_arrose == 0 || timer1-timer4 > delai_arrosage)
					   arrosage(&taux_humidite,temps_arrosage,RELAIS_A,&arrosages_consec,SONDEVCC,SONDEHUMIDITE,&deja_arrose,&selection);
			break;

		case 1 : //Affichage pour modifier l'arrosage set
					      //arrosage(&taux_humidite,temps_arrosage,RELAIS_A,&arrosages_consec,SONDEVCC,SONDEHUMIDITE,&deja_arrose,&selection );
					modif=modifier_arrosage_set(&arrosage_set);//Verifie si la valeur d'arrosage_set a changee
					if(affichage_courant!=1 || modif ==1){		//si les donnees a afficher ont changees
						affichage_courant =	affichage_selection1(taux_humidite,arrosage_set);	//On reaffiche
						timer2 = timer1;		//timer 2 tiens un decompte si on ne modifie rien
						modif=0;
					}
			break;

		case 2 : //Affichage pour modifier le temps d'arrosage
					     // arrosage(&taux_humidite,temps_arrosage,RELAIS_A,&arrosages_consec,SONDEVCC,SONDEHUMIDITE,&deja_arrose,&selection );
					modif=modifier_temps_arrosage(&temps_arrosage); //Verifie si temps_arrosage a change
					if(affichage_courant!=2 || modif==1){		//si les donnees a afficher ont changees
						affichage_courant =	affichage_selection2(temps_arrosage);	//On reaffiche
						timer2 = timer1;		//timer 2 tiens un decompte si on ne modifie rien
						modif=0;
					}
					
			break;

		case 3 :	// Affichage lors de l'attente apres arrosage
					if(timer4<=0)
					{
						timer4=timer1;
					}
					if (deja_arrose !=1){
						selection = 0;
						break;}

						affichage_serial(bouton1,bouton2,bouton3,selection,taux_humidite,arrosages_consec,deja_arrose, timer1, timer2,timer3, timer4);
						timer1=millis();
						affichage_courant = affichage_selection3(timer1,timer4,delai_arrosage);
						Serial.print(timer1-timer4);
						Serial.print("\n");
					
					if(timer1-timer4 > delai_arrosage){
						timer2=timer1;
						deja_arrose=0;
						timer4 =0;
						selection=0;
					}
			break;
	}


	if((timer1-timer2 >rafraichissement && deja_arrose==0) || (selection>=3 && deja_arrose==0))
		selection=0;

	if(timer1-timer2 >rafraichissement && deja_arrose==1)
		selection=3;

	if(deja_arrose==1 && selection>=4)
		selection=0;

}
