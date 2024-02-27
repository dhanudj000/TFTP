/*
NAME		: KONIREDDY DHANUNJAYA REDDY
DATE		: 27/2/2024
DESCRIPTION	: THIS FILE IS USED TO CREATE DATA PACKETS AND PRINT ERROR AND DATA IN DIFFERENT COLOURS
*/
#include "tftp.h"
//FUNCTION CREATE PACKET
void create_pack(int opcode , char *file , int block , void *buff , int len)
{
	switch(opcode)					//VERIFYING OPCODE TO CHECK WHICH FRAME IS TO CREATE
	{
		case RRQ :
			*(short*)buff = RRQ;		
			strcpy(buff+2 , file);		//COPYING OPCODE AND FILE NAMEE FOR RRQ
			break;
		case WRQ :
			*(short*)buff = WRQ;
			strcpy(buff+2 , file);		//COPYING OPCODE AND FILENAME FOR WRQ
			break;
		case DATA:
			*(short*)buff 	  = DATA;
			*(short*)(buff+2) = block;	//COPYING OPCODE , BLOCK , LENGTH FOR DATA FRAME
			*(short*)(buff+4) = len;
			break;
		case ACK :
			*(short*)buff	  = ACK;
			*(short*)(buff+2) = block;	//COPYING OPCODE AND BLOCK NUMBER FOR ACK FRAME
			break;
		case ERR :
			*(short*)buff	  = ERR;
			*(short*)(buff+2) = block;	//COPYING ERRNO AND ERR MSG FOR ERROR FRAME
			switch(block)
			{
				case ERR1:strcpy(buff+4 , "FILE NOT FOUND");		break;
				case ERR2:strcpy(buff+4 , "FILE CAN'T BE OPENED");	break;
				case ERR3:strcpy(buff+4 , "ILLEGAL TFTP OPERATION");	break;
				case ERR4:strcpy(buff+4 , "CONNECTION NOT STABLE");	break;
			}
			break;
	}
}

void print(int colour , char *line)			//FUNCTION TO PRINT TEXT IN DIFFERNT COLOUR
{
        char *col;
        switch(colour)
        {
                case RED        :
                        col = _RED;     break;
                case GREEN      :
                        col = _GREEN;   break;
                case YELLOW     :
                        col = _YELLOW;  break;		//SETTING THE SPECIFIED COLOUR
                case BLUE       :
                        col = _BLUE;    break;
                case MAGENTA    :
                        col = _MAGENTA;  break;
                case CYAN       :
                        col = _CYAN;    break;
                case CLEAR      :
                        col = _CLEAR;   break;
        }
        printf("%s",col);
        printf("%s",line);				//PRINTING THE TEXT IN GIVEN COLOUR
        col = _RESET;
        printf("%s",col);

}

void print_error(void *buff)				//FUNCTION PRINT ERROR FRAME DATA
{
        print(RED , "Recieved Error Message : ");
        printf("ERR NO - %d  ERR MSG - %s\n" , *(short*)(buff+2) , (char*)(buff+4));
}

