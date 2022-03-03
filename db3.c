#include <stdlib.h>
#include <stdio.h>
#include <my_sys.h>
#include <mysql.h>
#include <string.h>

#include "db.h"
#include "../utils/db.h"

static char *opt_host_name=NULL;/* host(default=localhost)*/
static chat *opt_user_name=NULL;/* username(default=login)*/
static char *opt_password=NULL;/* password(default=none)*/
static unsigned int opt_port_num=0;/*numero porta*/
static char *opt_socket_name=NULL;/*nome socket*/
static char *opt_db_name=NULL;/*inserire il  nome database al quale connettersi*/
static unsigned int opt_flags=CLIENT_MULTI_STATEMENTS | CLIENT_MULTI_RESULTS | CLIENT_COMPRESS | CLIENT_INTERACTIVE | CLIENT_REMEMBER_OPTIONS;/*flags di connessione*/
static MYSQL *conn;/*puntatore all'handler di connessione*/

/*puntatori alle credenziali per my_sql_change_user()*/
const char *db=NULL;//inserire il nome del db
const char *log="login";
const char *log_pass="login";
const char *admin="amministratore";
const char *admin_pass="amministratore";
const char *cup="personale_cup";
const char *cup_pass="personale_cup";
const char *paz="paziente";
const char *paz_pass="paziente";

/*puntatori agli statements*/
static MYSQL_STMT *login_procedure;
static MYSQL_STMT *dati_personale_procedura;
static MYSQL_STMT *dati_lab_procedura;


static void close_prepared_stmts(void)
{
	if(login_procedure) {
		mysql_stmt_close(login_procedure);
		login_procedure = NULL;
	}
	if(dati_personale_procedura) {
		mysql_stmt_close(dati_personale_procedura);
		dati_personale_procedura = NULL;
	}
	if(dati_lab_procedura) {
		mysql_stmt_close(dati_lab_procedura);
		dati_lab_procedura = NULL;
	}
	
}

static bool initialize_prepared_stmts(ruolo_t ruolo)
{
	switch(ruolo) {

		case LOGIN:
			if(!setup_prepared_stmt(&login_procedure, "call login(?, ?, ?)", conn)) {
				print_stmt_error(login_procedure, "Non è stato possibile inizializzare lo statement poichè setup_prepared_stmt(),nella procedura login è fallita\n");
				return false;
			}
			break;
		case AMMINISTRATORE:
			/*procedure per ruolo amministratore*/
			if(!setup_prepared_stmt(&dati_personale_procedura, "call stampa_dati_personale(?)", conn)) {
				print_stmt_error(dati_personale_procedura, "Non è stato possibile inizializzare lo statement poichè setup_prepared_stmt(),nella procedura dati_personale è fallita\n");
				return false;
			}
			if(!setup_prepared_stmt(&dati_lab_procedura, "call stampa_laboratori(?)", conn)) {
				print_stmt_error(dati_lab_procedura, "Non è stato possibile inizializzare lo statement poichè setup_prepared_stmt(),nella procedura dati_lab è fallita\n");
				return false;
			}
			break;
		case PERSONALE_CUP:
			/*procedure per ruolo personale_cup*/
			if(!setup_prepared_stmt(&booking, "call registra_prenotazione(?, ?, ?, ?, ?)", conn)) {
				print_stmt_error(booking, "Unable to initialize booking statement\n");
				return false;
			}
			if(!setup_prepared_stmt(&booking_report, "call report_prenotazioni()", conn)) {
				print_stmt_error(booking_report, "Unable to initialize booking report statement\n");
				return false;
			}
			break;
		case PAZIENTE:
			/*procedure per ruolo paziente*/
			if(!setup_prepared_stmt(&booking, "call registra_prenotazione(?, ?, ?, ?, ?)", conn)) {
				print_stmt_error(booking, "Unable to initialize booking statement\n");
				return false;
			}
			if(!setup_prepared_stmt(&booking_report, "call report_prenotazioni()", conn)) {
				print_stmt_error(booking_report, "Unable to initialize booking report statement\n");
				return false;
			}
			break;
		default:
			close_prepared_stmts();
			finish_with_error(conn,"Error ruolo non esistente");
			fprintf(stderr, "[FATAL] ruolo non esistente\n");
			exit(EXIT_FAILURE);
	}

	return true;
}




bool init_db(void)
{
	unsigned int timeout = 300;
	bool reconnect = true;
	/*inizializzo l'handler di connessione*/
	conn= my_sql_init(NULL);
	if(conn==NULL){
		fprintf(stderr,"mysql_init() fallita\n");
		exit(EXIT_FAILURE);
	}

	/*mi connetto al server */
	if(my_sql_real_connect(conn,opt_host_name,opt_user_name,opt_password,opt_db_name,opt_port_num,opt_socket_name,opt_flags)=NULL){
		fprintf(stderr,"my_sql_real_connect() è fallita\n");
		my_sql_close(conn);
		exit(EXIT_FAILURE);
	}
	if (mysql_options(conn, MYSQL_OPT_CONNECT_TIMEOUT, &timeout)) {
		print_error(conn, "mysql_options() è fallita\n");
	}
	if(mysql_options(conn, MYSQL_OPT_RECONNECT, &reconnect)) {
		print_error(conn, "mysql_options() è fallita\n");
	}

	return initialize_prepared_stmts(LOGIN);
}	

void fini_db(void)
{
	close_prepared_stmts();
	puts("Chiusura dei prepared statements completata con successo!\n");
	mysql_close(conn);
	puts("Chiusura della connessione col db completata con successo!\n");

}	

role_t attempt_login(struct credenziali *cred)
{
	MYSQL_BIND param[3]; 
	int role = 0;

	// Preparo i parametri in input per il login
	set_binding_param(&param[0], MYSQL_TYPE_VAR_STRING, cred->username, strlen(cred->username));
	set_binding_param(&param[1], MYSQL_TYPE_VAR_STRING, cred->password, strlen(cred->password));
	set_binding_param(&param[2], MYSQL_TYPE_LONG, &role, sizeof(role));
	
	// Faccio il binding dei parametri per lo stmt per la procedura di login
	if(mysql_stmt_bind_param(login_procedure, param) != 0) { 
		print_stmt_error(login_procedure, "Errore, non è stato possibile fare il binding dei parametri per il login\n");
		role = LOGIN_FALLITO;
		goto out;
	}

	// Eseguo lo stmt
	if(mysql_stmt_execute(login_procedure) != 0) {
		print_stmt_error(login_procedure, "Errore, non è stato possibile eseguire la procedura di login\n");
		role = LOGIN_FALLITO;
		goto out;
	}

	// Preparo il parametro di ouput dalla procedura
	set_binding_param(&param[0], MYSQL_TYPE_LONG, &role, sizeof(role));

	if(mysql_stmt_bind_result(login_procedure, param)) {
		print_stmt_error(login_procedure, "Errore, non è stato possibile collegare il buffer per contenre i risultati della procedura login\n");
		role = LOGIN_FALLITO;
		goto out;
	}

	// Prendo il risultato della procedura
	if(mysql_stmt_fetch(login_procedure)) {
		print_stmt_error(login_procedure, "Errore, non è stato possibile recuperare i risultati della procedura login dal db\n");
		role = LOGIN_FALLITO;
		goto out;
	}

    out:
	// Consumo le tabelle ritornate nel caso esistano
	while(mysql_stmt_next_result(login_procedure) != -1);
	// Libero il result set
	mysql_stmt_free_result(login_procedure);
	mysql_stmt_reset(login_procedure);
	// Restituisco il ruolo 
	return role;
}


void db_switch_user(ruolo_t ruolo)
{
switch(ruolo) {

		case LOGIN:
			close_prepared_stmts();
			if(mysql_change_user(conn, log, log_pass, db)) {
				fprintf(stderr, "Errore, mysql_change_user() è fallita: %s\n", mysql_error(conn));
				exit(EXIT_FAILURE);
			}
			if(!initialize_prepared_stmts(LOGIN)) {
				fprintf(stderr, "Errore non è stato possibile inizializzare lo stmt per la procedura login.\n");
				exit(EXIT_FAILURE);
			}
			break;
		case AMMINISTRATORE:
			close_prepared_stmts();
			if(mysql_change_user(conn, admin, admin_pass, db)) {
				fprintf(stderr, "Errore, mysql_change_user() è fallita: %s\n", mysql_error(conn));
				exit(EXIT_FAILURE);
			}
			if(!initialize_prepared_stmts(AMMINISTRATORE)) {
				fprintf(stderr, "Errore non è stato possibile inizializzare gli stmts per lo user amministratore.\n");
				exit(EXIT_FAILURE);
			}
			break;
		case PERSONALE_CUP:
			close_prepared_stmts();
			if(mysql_change_user(conn, cup, cup_pass, db)) {
				fprintf(stderr, "Errore, mysql_change_user() è fallita: %s\n", mysql_error(conn));
				exit(EXIT_FAILURE);
			}
			if(!initialize_prepared_stmts(PERSONALE_CUP)) {
				fprintf(stderr, "Errore non è stato possibile inizializzare gli stmts per lo user personale_cup.\n");
				exit(EXIT_FAILURE);
			}
			break;
		case PAZIENTE:
			close_prepared_stmts();
			if(mysql_change_user(conn, paz, paz_pass, db)) {
				fprintf(stderr, "Errore, mysql_change_user() è fallita: %s\n", mysql_error(conn));
				exit(EXIT_FAILURE);
			}
			if(!initialize_prepared_stmts(PAZIENTE)) {
				fprintf(stderr, "Errore non è stato possibile inizializzare gli stmts per lo user paziente.\n");
				exit(EXIT_FAILURE);
			}
			break;
		default:
			close_prepared_stmts();
			finish_with_error(conn,"Error ruolo non esistente");
			fprintf(stderr, "[FATAL] ruolo non esistente\n");
			exit(EXIT_FAILURE);
	}

}

struct personale *esegui_stampa_dati_personale(struct personale *personalei)
{
	int status;
	size_t row = 0;
	MYSQL_BIND param[8];//usato per input e output
	char codiceFiscale[CHAR_LEN];
	char nome[CHAR_LEN];
	char cognome[CHAR_LEN];
	char domicilio[CHAR_LEN];
	tipo_t tipo;
	char associazione[CHAR_LEN];
	int codiceReparto;
	int codiceOspedale;
	struct personale *personale=NULL;

	// Preparo parametri di input alla procedura
	set_binding_param(&param[0], MYSQL_TYPE_VAR_STRING, personalei->codiceFiscale, CHAR_LEN);

	if(mysql_stmt_bind_param(dati_personale_procedura, param) != 0) { 
		print_stmt_error(dati_personale_procedura, "Errore...non è stato possibile fare il binding dei parametri in input a dati_personale_procedura\n");
		goto out;
	}


	// Eseguo la procedura
	if(mysql_stmt_execute(dati_personale_procedura) != 0) {
		print_stmt_error(stampa_dati_personale_procedura, "Errore...non è stato possibile eseguire stampa_dati_personale_procedura");
		goto out;
	}

	mysql_stmt_store_result(dati_personale_procedura);

	personale = malloc(sizeof(*personale));
	if(personale == NULL)
		goto out;
	memset(personale, 0, sizeof(*personale));

	// Preparo ad accogliere i risultati
	mysql_stmt_store_result(dati_personale_procedura);

	set_binding_param(&param[0], MYSQL_TYPE_VAR_STRING, codiceFiscale, CHAR_LEN);
	set_binding_param(&param[1], MYSQL_TYPE_VAR_STRING, nome, CHAR_LEN);
	set_binding_param(&param[2], MYSQL_TYPE_VAR_STRING, cognome, CHAR_LEN);
	set_binding_param(&param[3], MYSQL_TYPE_VAR_STRING, domicilio, CHAR_LEN);
	set_binding_param(&param[4], MYSQL_TYPE_ENUM, tipo, sizeof(tipo_t));
	set_binding_param(&param[5], MYSQL_TYPE_VAR_STRING, associazione, CHAR_LEN);
	set_binding_param(&param[6], MYSQL_TYPE_LONG, &codiceReparto, sizeof(codiceReparto));
	set_binding_param(&param[7], MYSQL_TYPE_LONG, &codiceOspedale, sizeof(codiceOspedale));

	if(mysql_stmt_bind_result(dati_personale_procedura, param)) {
		print_stmt_error(dati_personale_procedura, "Errore...non è stato possibile fare il binding dei parametri di output di dati_personale_procedura\n");
		free(personale);
		personale = NULL;
		goto out;
	}

	/* Collego le informazioni ricevute */
	
		status = mysql_stmt_fetch(dati_personale_procedura);

		if (status == 1 || status == MYSQL_NO_DATA)
			break;

		strcpy(personale->codiceFiscale, codiceFiscale);
		strcpy(personale->nome, nome);
		strcpy(personale->cognome, cognome);
 		strcpy(personale->domicilio, domicilio);
		personale->tipo=tipo;
		strcpy(personale->associazione, associazione);
		personale->codiceReparto = codiceReparto;
		personale->codiceOspedale = codiceOspedale;

	
    out:
	// Consumo le tabelle ritornate nel caso esistano
	while(mysql_stmt_next_result(dati_personale_procedura) != -1);
	//Libero risorse
	mysql_stmt_free_result(dati_personale_procedura);
	mysql_stmt_reset(dati_personale_procedura);
	return personale;
}


struct lab *esegui_stampa_lab(void)
{
	int status;
	size_t row = 0;
	MYSQL_BIND param[3];//usato per output
	unsigned codiceL;
	unsigned codiceOspedale;
	char nome[CHAR_LEN];
	struct lab *lab = NULL;



	// Eseguo la procedura
	if(mysql_stmt_execute(dati_lab_procedura) != 0) {
		print_stmt_error(dati_lab_procedura, "Errore...non è stato possibile eseguire la procedura dati_lab");
		goto out;
	}

	mysql_stmt_store_result(dati_lab_procedura);

	// Alloco memoria per contenere i risultati dello stmt 
	lab = malloc(sizeof(*lab) + sizeof(struct lab_entry) * mysql_stmt_num_rows(dati_lab_procedura));
	if(lab == NULL)
		goto out;
	memset(lab, 0, sizeof(*lab) + sizeof(struct lab_entry) * mysql_stmt_num_rows(dati_lab_procedura));
	lab->num_entries = mysql_stmt_num_rows(dati_lab_procedura);

	// Preparo i campi per l'ouput
	mysql_stmt_store_result(dati_lab_procedura);

	set_binding_param(&param[0], MYSQL_TYPE_LONG, &codiceL, sizeof(codiceL));
	set_binding_param(&param[1], MYSQL_TYPE_LONG, &codiceOspedale, sizeof(codiceOspedale));
	set_binding_param(&param[2], MYSQL_TYPE_VAR_STRING, nome, CHAR_LEN);


	if(mysql_stmt_bind_result(dati_lab_procedura, param)) {
		print_stmt_error(dati_lab_procedura, "Errore...non è stato possibile fare il binding dei parametri di output di dati_lab_procedura\n");
		free(dati_lab_procedura);
		dati_lab_procedura = NULL;
		goto out;
	}

	/* Raccolgo le informazioni sui laboratori */
	while (true) {
		status = mysql_stmt_fetch(dati_lab_procedura);

		if (status == 1 || status == MYSQL_NO_DATA)
			break;

		lab->lab[row].codiceL = codiceL;
		lab->lab[row].codiceOspedale = codiceOspedale;
		strcpy(lab->lab[row].nome, nome);

		row++;
	}
    out:
	// Consumo le tabelle ritornate nel caso esistano
	while(mysql_stmt_next_result(dati_lab_procedura) != -1);
	//Libero risorse
	mysql_stmt_free_result(dati_lab_procedura);
	mysql_stmt_reset(dati_lab_procedura);
	return lab;
}

void libera_personale(struct personale*personale)
{
	free(personale);
}

void libera_lab(struct lab*lab)
{
	free(lab);
}


