#include <stdio.h>

#include "login.h"
#include "../utils/io.h"

void view_login(struct credenziali *cred)
{
	clear_screen();
	puts("\n\n\t    #           m           #                         \n");
	puts("\t mmm#   mmm   mm#mm   mmm   #mmm    mmm    mmm    mmm \n");
   	puts("\t#\" \"#  \"   #    #    \"   #  #\" \"#  \"   #  #   \"  #\"  #\n");
	puts("\t#   #  m\"\"\"#    #    m\"\"\"#  #   #  m\"\"\"#   \"\"\"m  #\"\"\"\"\n");
        puts("\t\"#m##  \"mm\"#    \"mm  \"mm\"#  ##m#\"  \"mm\"#  \"mmm\"  \"#mm\"\n\n\n");
	puts("#################################");
	puts("#   SISTEMA INFORMATIVO ASL DB   #");
	puts("#################################\n");
	get_input("Username: ", USERNAME_LEN, cred->username, false);
	get_input("Password: ", PASSWORD_LEN, cred->password, true);
}

bool ask_for_relogin(void)
{
	return yes_or_no("Vuoi fare il login con un user differente?", 'y', 'n', false, true);
}
