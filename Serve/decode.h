#ifndef _decode_h_
#define _decode_h_
static const char *cmd_host = "HOST";
static const int make_host = 1;
static const char *cmd_cnts = "CNTS";
static const int make_cnts = 2;
static const char *cmd_cnth = "CNTH";
static const int make_cnth = 3;

typedef struct
{
	int cmd;
	char var[15];
} decoded;

decoded getresults(char *code);
#endif