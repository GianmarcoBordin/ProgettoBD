#include <stdio.h>
#include "view/login.h"
#include "controller/login.h"
#include "model/db.h"
#include "utils/dotenv.h"
#include "utils/io.h"
#include "utils/validation.h"




int main()
{      
        if(env_load(".", false) != 0){
		puts("Errore nel caricaemnto delle variabili d'ambiente\n");
		return 1;
	}		

	if(!init_db()){
		puts("Errore nell'inizializzazione del db\n");
		return 1;
	}
		do {
			if(!login())
				fprintf(stderr, "Login non riuscito\n");
			
			db_switch_user(LOGIN);
		
		} while(ask_for_relogin());

        fini_db();
       	puts("La tua connessione al db è stata chiusa correttamente\n");
	puts("Sessione chiusa\n");
	puts("Arrivederci!");
	return 0;
}
