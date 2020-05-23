// 
// 
// 

#include "Fonction.h"


/**************************Procedure effacer ligne *************************************/
/*Cette procedure permet d'effacer une ligne sur l'�cran LCD

	parametres d'entree:	 
		la colonne � partir de laquelle on veut effacer: nCol
		la ligne sur laquelle on veut effacer: nLigne
*/

void effacer_ligne(int nCol, int nLigne){

  affichage.setCursor(nCol, nLigne);
  affichage.print("                "); //efface ligne
  affichage.setCursor(nCol, nLigne);

return;
}

/**********************Affichage Selection 0, par defaut***********************************/
/*Cette procedure affiche un message a lecran
	Renvoie son numero d'affichage pour affichage_courant
*/
int affichage_defaut(float taux_humidite)
{

  effacer_ligne(0,0);
  effacer_ligne(0,1);
  affichage.setCursor(0, 0);
  affichage.print("Ici rantanplan  ");
  affichage.setCursor(0, 1);
  affichage.print("Humidite    ");
  affichage.print(taux_humidite);
	return 0;
}

/**********************Affichage selection 1***********************************/
/*Cette procedure affiche un message a lecran

	parametres d'entree:	 
		taux d'humidite mesure: taux_humidite
		le taux auquel il faut arroser: arrosage_set
	Renvoie son numero d'affichage pour affichage_courant

		*/
int affichage_selection1(int taux_humidite, int arrosage_set)
{

	effacer_ligne(0,0);
	effacer_ligne(0,1);
	affichage.setCursor(0, 0);
	affichage.print("Humidite: ");
	affichage.print(taux_humidite);
	affichage.setCursor(0,1);
	affichage.print("ArrosageSet:");
	affichage.print(arrosage_set);

return 1;
}

/**********************Affichage selection 2***********************************/
/*Cette procedure affiche un message a lecran

	parametres d'entree:	 
		le temps d'arrosage pour la quantite d'eau desiree: long temps_arrosage
	Renvoie son numero d'affichage pour affichage_courant
*/
int affichage_selection2(long temps_arrosage)
{

	effacer_ligne(0,0);
	affichage.print("Temps/arrosages:");
	effacer_ligne(0,1);
	affichage.print(temps_arrosage/1000);
	affichage.print("   Secondes");

return 2;
}

int affichage_selection3(unsigned long timer1,unsigned long timer4,unsigned long delai_arrosage)
{

	effacer_ligne(0,0);
	affichage.print("Deja arrose");
	effacer_ligne(0, 1);                    //enleve message d'arrosage
	affichage.print("Delai(min): ");
	effacer_ligne(12, 1);
	

	if(timer1-timer4 < 60000)
					affichage.print(((delai_arrosage-(timer1-timer4))/60000)+1);
	else
		affichage.print(1);
	delay(125);
return 3;
}


int modifier_temps_arrosage(long* temps_arrosage)
{

	long temps_arrosage_preced=*temps_arrosage;

	bouton1 = digitalRead(BOUTON1);  //Lire si les boutons sont press�s
	bouton2 = digitalRead(BOUTON2);
				
	if (bouton1 != 1)   //Si un bouton est appuy�
		*temps_arrosage += 2500;
				
	else if (bouton2 != 1) 
		*temps_arrosage -= 2500;

	if(temps_arrosage_preced == *temps_arrosage)
		return 0;
	else
		return 1;
}

int modifier_arrosage_set(int* arrosage_set)
{

	int arrosage_set_preced = *arrosage_set;
				bouton1 = digitalRead(BOUTON1);  //Lire si les boutons sont press�s
				bouton2 = digitalRead(BOUTON2);
					
				if (bouton1 != 1)   //Si un bouton est appuy�
					*arrosage_set += 10;
				
				else if (bouton2 != 1) 
					*arrosage_set -= 10;
	if(arrosage_set_preced == *arrosage_set)
		return 0;
	else
		return 1;
}

/*
Affichage en serial des donnees pour debug, la fonction peut etre mise en commentaire.
*/
void affichage_serial(int bouton1, int bouton2,
					  int bouton3, int selection, int taux_humidite,int arrosages_consec,int deja_arrose,unsigned long timer1,unsigned long timer2,unsigned long timer3,unsigned long timer4)
{/*
	Serial.print("b1:");
	Serial.print(bouton1);
	Serial.print(" b2:");
	Serial.print(bouton2);
	Serial.print(" b3:");
	Serial.print(bouton3);
	Serial.print(" Selec:");
	Serial.print(selection);
	Serial.print(" Consec:");
	Serial.print(arrosages_consec);
	Serial.print(" dejaarros:");
	Serial.print(deja_arrose);
	Serial.print(" t1:");
	Serial.print(timer1);
	Serial.print(" t2:");
	Serial.print(timer2);
	Serial.print(" t3:");
	Serial.print(timer3);
	Serial.print(" t4:");
	Serial.print(timer4);
	Serial.print(" \n");
return;*/
}
/**************************S�quece d'arrosage *************************************/
/*Cette procedure permet l'arrosage du plant connecte au RELAIS_A en activant celui-ci 
pour une duree de TEMPS_ARROSAGE

	parametres d'entree:	 
		le temps d'arrosage pour la quantite d'eau desiree: long temps_arrosage
		
*/
int arrosage(float* taux_humidite,long temps_arrosage,int relais,int* arrosages_consec,int sonde_vcc,int SONDEHUMIDITE,int* deja_arrose,int* selection)
{

	digitalWrite(SONDEVCC,HIGH);
  delay(20);
   * taux_humidite = analogRead(HUM_PIN);  //Lis le taux d'humidite
   digitalWrite(SONDEVCC,LOW);
	
		/* verification et d�collage de la ventilation si necessaire */

	if(*taux_humidite <= arrosage_set && !isnan(*taux_humidite) && *deja_arrose==0)
	{
      digitalWrite(RELAIS_A, LOW);          //envoie du courant � la pompe

      Serial.print("Arrosage... \n"); //affichage
      effacer_ligne(0, 0);
	  effacer_ligne(0, 1);
      affichage.print("Arrosage...");

      delay(temps_arrosage);                  //Laisse l'eau pomper

      digitalWrite(RELAIS_A, HIGH);
      effacer_ligne(0, 0);                    //enleve message d'arrosage	
	  *arrosages_consec+=1;
	  *deja_arrose=1;
	  *selection=3;
	  return 1;
	}
	
return 0;
}
