#include <stdlib.h>
#include <stdio.h>
#include <my_sys.h>
#include <mysql.h>
#include <string.h>

static char *opt_host_name=NULL;/* host(default=localhost)*/
static chat *opt_user_name=NULL;/* username(default=login)*/
static char *opt_password=NULL;/* password(default=none)*/
static unsigned int opt_port_num=0;/*numero porta*/
static char *opt_socket_name=NULL;/*nome socket*/
static char *opt_db_name=NULL;/*inserire il  nome database al quale connettersi*/
static unsigned int opt_flags=CLIENT_MULTI_STATEMENTS | CLIENT_MULTI_RESULTS | CLIENT_COMPRESS | CLIENT_INTERACTIVE | CLIENT_REMEMBER_OPTIONS;/*flags di connessione*/
static MYSQL *conn;/*puntatore all'handler di connessione*/

/*puntatori agli statements*/
static MYSQL_STMT *login_procedure;


static void close_prepared_stmts(void)
{
	if(login_procedure) {
		mysql_stmt_close(login_procedure);
		login_procedure = NULL;
	}
	
}

static bool initialize_prepared_stmts(ruolo_t ruolo)
{
	switch(ruolo) {

		case LOGIN:
			if(!setup_prepared_stmt(&login_procedure, "call login(?, ?, ?, ?)", conn)) {
				print_stmt_error(login_procedure, "Non è stato possibile inizializzare lo statement poichè setup_prepared_stmt(),nella procedura login è fallita\n");
				return false;
			}
			break;
		case AMMINISTRATORE:
			/*procedure per ruolo amministratore*/
			if(!setup_prepared_stmt(&register_flight, "call registra_volo(?, ?, ?, ?, ?, ?, ?)", conn)) {
				print_stmt_error(register_flight, "Unable to initialize register flight statement\n");
				return false;
			}
			if(!setup_prepared_stmt(&get_occupancy, "call report_occupazione_voli()", conn)) {
				print_stmt_error(register_flight, "Unable to initialize get occupancy statement\n");
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
	int esenzione=0;

	// Preparo i parametri per il primo login
	set_binding_param(&param[0], MYSQL_TYPE_VAR_STRING, cred->username, strlen(cred->username));
	set_binding_param(&param[1], MYSQL_TYPE_VAR_STRING, cred->password, strlen(cred->password));
	set_binding_param(&param[2], MYSQL_TYPE_LONG, &role, sizeof(role));
	set_binding_param(&param[3], MYSQL_TYPE_LONG,&esenzione,sizeof(esenzione));

	if(mysql_stmt_bind_param(login_procedure, param) != 0) { // Note _param
		print_stmt_error(login_procedure, "Could not bind parameters for login");
		role = FAILED_LOGIN;
		goto out;
	}

	// Run procedure
	if(mysql_stmt_execute(login_procedure) != 0) {
		print_stmt_error(login_procedure, "Could not execute login procedure");
		role = FAILED_LOGIN;
		goto out;
	}

	// Prepare output parameters
	set_binding_param(&param[0], MYSQL_TYPE_LONG, &role, sizeof(role));

	if(mysql_stmt_bind_result(login_procedure, param)) {
		print_stmt_error(login_procedure, "Could not retrieve output parameter");
		role = FAILED_LOGIN;
		goto out;
	}

	// Retrieve output parameter
	if(mysql_stmt_fetch(login_procedure)) {
		print_stmt_error(login_procedure, "Could not buffer results");
		role = FAILED_LOGIN;
		goto out;
	}

    out:
	// Consume the possibly-returned table for the output parameter
	while(mysql_stmt_next_result(login_procedure) != -1) {}

	mysql_stmt_free_result(login_procedure);
	mysql_stmt_reset(login_procedure);
	return role;
}


