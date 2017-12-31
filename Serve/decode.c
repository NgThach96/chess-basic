#include "decode.h"
#include "string.h"
#include "stdio.h"

decoded getresults(char *code)
{
	decoded temp;
	char header[20];
	char varible[20];
	sscanf(code, "%s %s", header, varible);
	if (strcmp(header, cmd_host) == 0)
	{
		temp.cmd = make_host;
		strcpy(temp.var, varible);
		//Thạch
	}
	else if (strcmp(header, cmd_cnts) == 0)
	{
		temp.cmd = make_cnts;
		strcpy(temp.var, varible);
	}
	else if (strcmp(header, cmd_cnth) == 0)
	{
		temp.cmd = make_cnth;
		strcpy(temp.var, varible);
	}
	return temp;
}