#pragma once
#include <stdbool.h>
#include <stdlib.h>

extern bool init_db(void);
extern void fini_db(void);

#define DATE_LEN 11
#define TIME_LEN 6
#define DATETIME_LEN (DATE_LEN + TIME_LEN)

#define PASSWORD_LEN 45
#define USERNAME_LEN 45
struct credenziali {
	char username[USERNAME_LEN];
	char password[PASSWORD_LEN];
};

typedef enum {
	LOGIN,
	AMMINISTRATORE,
	PERSONALE_CUP,
	PAZIENTE,
	LOGIN_FALLITO
} ruolo_t;

typedef enum {
	personaleMedico,
	personaleVolontario

} tipo_t;


extern role_t attempt_login(struct credenziali *cred);

#define CHAR_LEN 45
#define CITTA_LEN 45
#define TIPO_LEN 45

struct personale {
	char codiceFiscale[CHAR_LEN];
	char nome[CHAR_LEN];
	char cognome[CHAR_LEN];
	char domicilio[CHAR_LEN];
	tipo_t tipo;
	char associazione[CHAR_LEN];
	unsigned codiceReparto;
	unsigned codiceOspedale;
};

struct lab_entry {
	unsigned codiceL;
	unsigned codiceOspedale;
	char nome[CHAR_LEN];
};

struct lab {
	unsigned num_entries;
	struct lab_entry lab[];
};

extern void do_register_flight(struct flight *flight);


struct occupancy_entry {
	char idVolo[ID_LEN];
	char cittaPart[CITTA_LEN];
	char partenza[DATETIME_LEN];
	char cittaArr[CITTA_LEN];
	char arrivo[DATETIME_LEN];
	unsigned prenotati;
	unsigned disponibili;
	double occupazione;
};
struct occupancy {
	unsigned num_entries;
	struct occupancy_entry occupancy[];
};

extern struct occupancy *do_get_occupancy(void);
extern void occupancy_dispose(struct occupancy *occupancy);


extern void db_switch_user(ruolo_t ruolo);

#define NAME_SURNAME_LEN 45
struct booking {
	char idVolo[ID_LEN];
	char giorno[DATE_LEN];
	char name[NAME_SURNAME_LEN];
	char surname[NAME_SURNAME_LEN];
};

extern int do_booking(struct booking *info);

struct booking_info {
	char name[NAME_SURNAME_LEN];
	char surname[NAME_SURNAME_LEN];
};

struct flight_info {
	char idVolo[ID_LEN];
	char cittaPart[CITTA_LEN];
	char cittaArr[CITTA_LEN];
	char giorno[DATE_LEN];
	size_t num_bookings;
	struct booking_info *bookings;
};

struct booking_report {
	size_t num_flights;
	struct flight_info flights[];
};

extern struct booking_report *do_booking_report(void);
extern void booking_report_dispose(struct booking_report *report);
