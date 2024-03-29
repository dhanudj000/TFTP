/*
NAME		: KONIREDDY DHANUNJAYA REDDY
DATE		: 27/2/2024
DESCRIPTION	: THIS FILE IS USED TO CREATE DATA PACKETS AND PRINT ERROR AND DATA IN DIFFERENT COLOURS
*/
#include <stdio.h>
#include <stdio_ext.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <netinet/ip.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/select.h>
//INCLUDING REQUIRED HEADER FILES

#define RRQ	1
#define WRQ	2
#define DATA	3
#define ACK	4
#define ERR	5

#define ERR1	1
#define ERR2	2
#define ERR3	3
#define ERR4 	4

#define RED      1
#define GREEN    2
#define YELLOW   3
#define BLUE     4
#define MAGENTA  5 
#define CYAN     6
#define RESET    7
#define CLEAR 	 8

#define _RED     "\x1b[31m"
#define _GREEN   "\x1b[32m"
#define _YELLOW  "\x1b[33m"
#define _BLUE    "\x1b[34m"
#define _MAGENTA "\x1b[35m"
#define _CYAN    "\x1b[36m"
#define _RESET   "\x1b[0m"
#define _CLEAR 	"\e[1;1H\e[2J"

#define SET_COL(a) printf(a)



#ifndef MAC
#define MAC	1
void print(int colour , char *line);
//FUNCTION TO PRINT TECT IN COLOUR
void create_pack(int opcode,char *file,int block,void* buff,int len);
//FUNCTION TO CREATE A PACKET
int init_con(int s_fd , void *buff , struct sockaddr_in *addr , char* ip , fd_set* set , struct timeval* t);
//FUNCTION TO INITIATE A CONNECTION
void read_cmd(char *cmd);
//FUNCTION TO READ COMMAND 
void print_error(void *buff);
//FUNCTION TO PRINT ERROR FRAME
#endif
