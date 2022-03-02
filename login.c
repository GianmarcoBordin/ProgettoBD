#include <stdio.h>

#include "login.h"
#include "../utils/io.h"

void view_login(struct credenziali *cred)
{
	clear_screen();
	puts("#################################");
	puts("#   SISTEMA INFORMATIVO ASL DB   #");
	puts("#################################\n");
	puts("\n\n\t    #           m           #                         \n");
	puts("\t mmm#   mmm   mm#mm   mmm   #mmm    mmm    mmm    mmm \n");
   	puts("\t#\" \"#  \"   #    #    \"   #  #\" \"#  \"   #  #   \"  #\"  #\n");
	puts("\t#   #  m\"\"\"#    #    m\"\"\"#  #   #  m\"\"\"#   \"\"\"m  #\"\"\"\"\n");
        puts("\t\"#m##  \"mm\"#    \"mm  \"mm\"#  ##m#\"  \"mm\"#  \"mmm\"  \"#mm\"\n\n\n");
	get_input("Username: ", USERNAME_LEN, cred->username, false);
	get_input("Password: ", PASSWORD_LEN, cred->password, true);
}

bool ask_for_relogin(void)
{
	return yes_or_no("Do you want to log in as a different user?", 'y', 'n', false, true);
}
