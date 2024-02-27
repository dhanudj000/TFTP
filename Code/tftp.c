#include "tftp.h"

void create_pack(int opcode , char *file , int block , void *buff , int len)
{
	switch(opcode)
	{
		case RRQ :
			*(short*)buff = RRQ;
			strcpy(buff+2 , file);
			break;
		case WRQ :
			*(short*)buff = WRQ;
			strcpy(buff+2 , file);
			break;
		case DATA:
			*(short*)buff 	  = DATA;
			*(short*)(buff+2) = block;
			*(short*)(buff+4) = len;
			break;
		case ACK :
			*(short*)buff	  = ACK;
			*(short*)(buff+2) = block;
			break;
		case ERR :
			*(short*)buff	  = ERR;
			*(short*)(buff+2) = block;
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

void print(int colour , char *line)
{
        char *col;
        switch(colour)
        {
                case RED        :
                        col = _RED;     break;
                case GREEN      :
                        col = _GREEN;   break;
                case YELLOW     :
                        col = _YELLOW;  break;
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
        printf("%s",line);
        col = _RESET;
        printf("%s",col);

}

void print_error(void *buff)
{
        print(RED , "Recieved Error Message : ");
        printf("ERR NO - %d  ERR MSG - %s\n" , *(short*)(buff+2) , (char*)(buff+4));
}

