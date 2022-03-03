#include <stdio.h>

#include "../model/db.h"
#include "amministratore.h"
#include "../utils/io.h"
#include "../utils/validation.h"

int get_administrator_action(void)
{
	char options[3] = {'1','2', '3'};
	char op;

	clear_screen();
	puts("\n\n\t    #           m           #                         \n");
	puts("\t mmm#   mmm   mm#mm   mmm   #mmm    mmm    mmm    mmm \n");
   	puts("\t#\" \"#  \"   #    #    \"   #  #\" \"#  \"   #  #   \"  #\"  #\n");
	puts("\t#   #  m\"\"\"#    #    m\"\"\"#  #   #  m\"\"\"#   \"\"\"m  #\"\"\"\"\n");
        puts("\t\"#m##  \"mm\"#    \"mm  \"mm\"#  ##m#\"  \"mm\"#  \"mmm\"  \"#mm\"\n\n\n");
	puts("#################################");
	puts("#   MODALITA' AMMINISTRATORE   #");
	puts("#################################\n");
	puts("1) Stampa dati di un membro del personale reparto");
	puts("3) Quit");

	op = multi_choice("Select an option", options, 3);
	return op - '1';
}

void recupera_amministratore_cf_personale(struct personale *personale)
{
	clear_screen();
	printf("## INFORMAZIONI PERSONALE ##\n\n");
	get_input("Prego inserire il codice fiscale del membro del personale di cui si vogliono i dati: ", CHAR_LEN , personale->codiceFiscale);
}
void stampa_personale(struct personale *personale)
{



	clear_screen();
	puts("## INFORMAZIONI SUL MEMBRO DEL PERSONALE CERCATO ##\n\n");

	
	printf("CF:%s\nNOME:%s\nCOGNOME:%s\nDOMICILIO:%s\nTIPO:%s\nASSOCIAZIONE:%s\nCODR:%d\CODO:%d\n",
			personale->codiceFiscale,
			personale->nome,
			personale->cognome,
 			personale->domicilio,
			(personale->tipo) == personaleVolontario ? "personaleVolontario" : "personaleMedico",
			(personale->associazione) != NULL ? (personale->associazione) : "nessuna associazione",
			personale->codiceReparto,
			personale->codiceOspedale);
	fflush(stdout);
	
}
void stampa_lab(struct lab *lab)
{



	clear_screen();
	puts("## INFORMAZIONI SUI LABORATORI ##\n\n");


	for(size_t i = 0; i < lab->num_entries; i++) {
		printf("CODICE_LAB:%d\nCODICE_OSPEDALE:%d\nNOME_LAB:%d\n",
			lab->lab[i].codiceL,
			lan->lab[i].codiceOspedale,
			lab->lab[i].nome);
			fflush(stdout);
	}
	
	
}
