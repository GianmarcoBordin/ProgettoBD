#include <stdbool.h>

#include "login.h"
#include "amministratore.h"
#include "personale_cup.h"
#include "paziente.h"
#include "../view/login.h"
#include "../model/db.h"


bool login(void)
{
	struct credenziali cred;
	view_login(&cred);
	ruolo_t role = attempt_login(&cred);

	switch(role) {
		case AMMINISTRATORE:
			amministratore_controller();
			break;
		case PERSONALE_CUP:
			cup_controller();
			break;
		case PAZIENTE:
			paziente_controller();
			break;
		default:
			return false;
	}

	return true;
}
