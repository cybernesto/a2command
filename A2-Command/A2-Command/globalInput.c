#include <conio.h>

#include "Configuration.h"
#include "constants.h"
#include "globals.h"
#include "screen.h"

unsigned char  waitForEnterEsc(void)
{
	unsigned char key = 0;

	revers(TRUE); 
	//textcolor(color_text_other); 
	cputsxy(size_x - 5, 0, "(RET)"); 
	revers(FALSE);

	while(key != CH_ESC
		&& key != CH_ENTER)
	{
		key = cgetc();
	}
	
	return key;
}