#include <stdio.h>
#include "view/login.h"
#include "controller/login.h"
#include "model/db.h"
#include "utils/dotenv.h"
#include "utils/io.h"
#include "utils/validation.h"

#define check_env_failing(varname)                                          \
if(getenv((varname)) == NULL) {                                             \
        fprintf(stderr, "[FATAL] env variable %s not set\n", (varname));    \
        ret = false;                                                        \
}
static bool validate_dotenv(void)
{
	bool ret = true;

	check_env_failing("HOST");
	check_env_failing("DB");
	check_env_failing("LOGIN_USER");
	check_env_failing("LOGIN_PASS");
	check_env_failing("ADMINISTRATOR_USER");
	check_env_failing("ADMINISTRATOR_PASS");
	check_env_failing("AGENCY_USER");
	check_env_failing("AGENCY_PASS");

	return ret;
}
#undef set_env_failing

int main()
{      
        if(env_load(".", false) != 0){
		puts("Errore nel caricaemnto delle variabili d'ambiente\n");
		return 1;
	}	
        if(!validate_dotenv()){
		puts("Errore,variabili d'ambiente non ancora ben settate\n");
		return 1;
	}	

	if(!init_db()){
		puts("Errore nell'inizializzazione del db\n");
		return 1;
	}
		do {
			if(!login())
				fprintf(stderr, "Login non riuscito\n");
			db_switch_to_login();
		} while(ask_for_relogin());

        fini_db()
       	puts("La tua connessione al db è stata chiusa correttamente\n");
	puts("Sessione chiusa\n");
	puts("Arrivederci!");
	return 0;
}
