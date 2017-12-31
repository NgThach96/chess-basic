#include "decode.h"
#include "string.h"
#include "stdio.h"

decoded getresults(char *code)
{
	decoded temp;
	char header[20];
	char varible[20];
	sscanf(code, "%s %s", header, varible);
	if (!strcmp(header, cmd_move))
	{
		temp.cmd = make_move;
		strcpy(temp.var, varible);
	//Thạch
	}
	else if(!strcmp(header, cmd_pcolor))
	{
		temp.cmd = make_pcolor;
		strcpy(temp.var, varible);
	}
	else if(!strcmp(header,cmd_draw))
	{
		temp.cmd = make_draw;
		strcpy(temp.var, varible);	
	}
	else if(!strcmp(header,cmd_lose))
	{
		temp.cmd = make_lose;
		strcpy(temp.var, varible);	
	}
	return temp;
}