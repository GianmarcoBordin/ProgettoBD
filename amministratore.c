#include <stdbool.h>
#include <stdio.h>
#include <string.h>


#include "amministratore.h"
#include "../model/db.h"
#include "../view/amministratore.h"
#include "../utils/io.h"

static bool dati_personale(void)
{
	struct personale personale;
	memset(&personale, 0, sizeof(personale));
	recupera_amministratore_cf_personale(&personale);
	struct personale *personale_o=esegui_stampa_dati_personale(&personale);
	if(personale_o != NULL){
		stampa_personale(personale);
		libera_personale(personale);
	}
	return false;
}


static bool dati_lab(void)
{
	struct lab *lab = esegui_stampa_lab();
	if(lab != NULL) {
		stampa_lab(lab);
		libera_lab(lab);
	}
	return false;
}


static bool quit(void) {
	return true;
}




void amministratore_controller(void)
{
	db_switch_user(AMMINISTRATORE);
	int v=0;//tutto ok

	while(true) {
		int action = get_administrator_action();
		switch(action) {

			case (action==0):
				if(dati_personale()){
					fprintf(stderr, "Errore durante l'esecuzione della procedura dati_personale\n");
					v=-1;
				}	
				break;
			case (action==1):
				if(dati_lab()){
					fprintf(stderr, "Errore durante l'esecuzione della procedura dati_lab\n");
					v=-1;
				}
				break;
			case (action==2):
		
				break;
			default:
				fprintf(stderr, "Errore...azione non esistente\n");
				continue;
			
		}
		if(v==-1)
			break;

		press_anykey();
	}
}
