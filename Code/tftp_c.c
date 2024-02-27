/*
NAME		: KONIREDDY DHANUNJAYA REDDY
DATE		: 27/2/2024
DESCRIPTION	: THIS FILE IS USED TO CREATE A CLIENT
*/
#include "tftp.h"
//SETTING PORT NUMBER
#define PORT	 5000
#define MAX_BUFF 512
int main()
{
	int sockfd , ser_len = sizeof(struct sockaddr),n , file_fd = -1 , data_len = -1;
	int tries= 0,block=0;
	void* send_buff = malloc(MAX_BUFF) , *recv_buff = malloc(MAX_BUFF);
	char con_flag = 1 , cmd[50] , *ptr , first_entry = 1;
	struct sockaddr_in serv_addr;
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port   = htons(PORT);
	sockfd = socket(AF_INET , SOCK_DGRAM , 0);
	struct timeval t;
	fd_set set;
	//INITIALIZING AND ASSIGNING ALL REQUIRED MEMBERS
	while(1)
	{
		//INITIALIZING THE CON NECTION
		if(con_flag)
		{
			print(GREEN , "Connect to a server : connect <ip_address>\n");
			read_cmd(cmd);
			if(!strcmp(strtok(cmd , " ") , "connect"))
			{
				//CHECKING THE IP ADDRES  IF IT IS CONNECTABLE
connect:			if(init_con(sockfd , send_buff , &serv_addr , strtok(NULL , " ") , &set , &t))
				{
					print(GREEN , "Connection success!\n");
					con_flag = 0;
				}
				//IF NOT PRINTING THE ERROR
				else
				{
					print(YELLOW , "Connection not established. Try again !!!!\n");
					continue;
				}
			}
			else
			{
				continue;
			}
		}
		else
		{
			//READING CMD FROM USER
			read_cmd(cmd);
			//PARSING THE COMMAND FIRST
			ptr = strtok(cmd , " ");
			//CHECKING THE COMMAND
			if(!strcmp(ptr , "get"))
			{
				//GETTING THE SECONG ARGUMENT FROM THE CMD
				ptr = strtok(NULL , " ");
				//CREATING A RRQ REQUEST BASED ON GIVEN FILE NAME
				create_pack(RRQ ,ptr , 0 , send_buff , 0);
				//printf("socket - %d\n",sockfd);
				//SENDING A RRQ REQUEST
				sendto(sockfd , send_buff , 512 , 0 , (struct sockaddr*)&serv_addr , ser_len);
				//perror("sendto";
				print(BLUE , "Sending RRQ frame...\n");
				//RECIEVING THE DATA FROM THE SERVER
				while(recvfrom(sockfd , recv_buff ,MAX_BUFF , 0, NULL , NULL))
				{
					switch(*(short*)recv_buff)
					{
						//IF THE RECIEVED FRAME IS DATA FRAME THEN WRITING THE DATA INTO OPENED FILE
						case DATA:
							data_len = *(short*)(recv_buff+4);
							if(*(short*)(recv_buff + 2) == 1)
							{
					//			first_entry = 0;
								file_fd = open(ptr , O_CREAT | O_TRUNC | O_WRONLY , 0666);
							}
							n = write(file_fd , recv_buff+6 , data_len);
							create_pack(ACK , NULL , *(short*)(recv_buff+2) , send_buff , n);
							sendto(sockfd , send_buff , MAX_BUFF , 0 ,(struct sockaddr*)&serv_addr , ser_len);
							print(MAGENTA , "Acknowledgement sent for block : "); printf("%d\n",*(short*)(recv_buff+2));
							break;
						//IF THE ERROR IS RECIEVED , JUST PRINTING THE ERROR AND CONTINUING
						case ERR :
							data_len = 0;
							print_error(recv_buff);
							break;
					}
					//CHECKING IF THE DATA FRAME IS LAST FRAME FOR THE FILE
					if(data_len == 0)
					{
						print(GREEN , "Transfer Success!!!!!");
						data_len = -1;
						if(file_fd != -1)
							close(file_fd);
						//first_entry = 1;
						break;
					}
				}
				//ASSIGNING A VALUE BEFORE ASSINGING A FD OF OPENED FILE SUCH THAT IT WILL BE KNOWN TO CLOSE OR NOT
				file_fd = -1;		
			}
			else if(!strcmp(ptr , "put"))
			{
				//PARSING THE COMMAND TO GET 2ND ARG
				ptr = strtok(NULL , " ");
				file_fd = open(ptr , O_RDONLY);
				//OPENING THE FILE NAD CHECKING IF IT'S OPEN OR NOT
				if(file_fd == -1)
				{
					print(YELLOW , "File not present.\n");
					continue;
				}
				//CREATONG A WRQ REQUEST
				create_pack(WRQ , ptr , 0 , send_buff , 0);
				sendto(sockfd , send_buff , 512 , 0 , (struct sockaddr*)&serv_addr , ser_len);
				print(BLUE , "Sending WRQ...\n");
				//RECIEVING THE ACKNOWLEDGEMNET AND CHECKING IF FILE IS OPENED OR NOT IN SERVER
				recvfrom(sockfd , recv_buff , 512 , 0, NULL , NULL);
				switch(*(short*)recv_buff)
				{
					case ACK:
						print(GREEN , "Created file in Server.\n");
						print(YELLOW , "Transferring data.\n");
						break;
					case ERR:
						print(RED , "Failed to create file in server.\n");
						print_error(recv_buff);
					default :continue;
				}
				//SENDING DATA FRAMES
				while(n = read(file_fd , send_buff+6 , 506))
                                {
                                        block+=1;
                                        create_pack(DATA , NULL , block , send_buff , n);
retransmit:                             sendto(sockfd , send_buff , MAX_BUFF , 0 , (struct sockaddr*)&serv_addr , ser_len);
                                        perror("sendto");
					t.tv_sec = 5;
                                        FD_ZERO(&set);
                                        FD_SET(sockfd , &set);
                                        switch(select(sockfd+1 , &set , NULL , NULL , &t))
                                        {
                                        case -1 :
                                        print(RED , "Error with select system call.\n");
                                        exit(1);
                                        case 0 :
                                        if(tries++ == 5)
                                        {
                                                tries = 0;
                                                printf("Tried retransmitting data 5 times.\nStopping data transmission.\nSending Error frame.\n");
                                                create_pack(ERR , NULL , ERR4 , send_buff , 0);
						sendto(sockfd , send_buff , MAX_BUFF , 0 , (struct sockaddr*)&serv_addr , ser_len);
                                                *(short*)recv_buff = ERR;
						break;
                                        }
                                        print(YELLOW , "Data packet lost , Retransmitting data\n");
                                        goto retransmit;
                                        default :
					tries=0;
                                        recvfrom(sockfd , recv_buff , MAX_BUFF , 0 , NULL , NULL);
                                        }
                                        if(*(short*)recv_buff = ACK)
                                                continue;
					else
						break;
                                }
                                create_pack(DATA , NULL , block+1 , send_buff , 0);
                                sendto(sockfd , send_buff , MAX_BUFF , 0 , (struct sockaddr*)&serv_addr , ser_len);
                                block = 0;

			}
			else if(!strcmp(ptr , "close"))
			{
				con_flag = 1;
				print(MAGENTA , "....Disconnected\n");
				create_pack(ERR , NULL , ERR4 , send_buff , 0);
                                sendto(sockfd , send_buff , MAX_BUFF , 0 , (struct sockaddr*)&serv_addr , ser_len);
			}
			else if(!strcmp(ptr , "connect"))
			{
				con_flag = 1;
				goto connect;
			}
			else if(!strcmp(ptr , "help"))
			{
				print(BLUE , "connect <IP_ADDR> - To connect to a server.\nget <FILE_NAME> - To get file fom server.\nput <FILE_NAME> - To send file to server.\nclose - to close the client.\n");
			}
			else
			{
				// IF CMD ENTER IS WRONG PRINTING SUGGESTION
				print(CYAN , "Invalid Cmd -\nEnter 'help' to see commands.\n");
			}
		}
	}
}

void read_cmd(char *cmd)
{
	__fpurge(stdin);
	printf("Enter cmd : ");	
	scanf("%[^\n]", cmd);
}

int init_con(int s_fd , void *buff , struct sockaddr_in *addr , char* ip , fd_set* set , struct timeval* t)
{
	t->tv_sec = 5;
	FD_ZERO(set);
	FD_SET(s_fd , set);
	(addr->sin_addr).s_addr = inet_addr(ip);
	printf("%s\n",ip);
	create_pack(ACK , NULL , 0 , buff , 0);
	sendto(s_fd , buff , 512 , 0 , (struct sockaddr*)addr , sizeof(struct sockaddr));
	perror("sendto");
	switch( select(s_fd+1 , set , NULL , NULL , t) )
	{
		case -1:
			print(RED , "Some error occured with 'select' call.\n");
			free(buff);
			close(s_fd);
			exit(1);
		case  0:
			print(YELLOW , "Timeout before recieving acknowledgement.\n");
			return 0;
		default:
			recvfrom(s_fd , buff , 512 , 0 , NULL , NULL);
			perror("recvfrom");
			if(*(short*)buff == ACK && *(short*)(buff+2) == 0) 
			{
				print(MAGENTA , "Server Acknowledged.\n");
				return 1; 
			}
			print(RED , "Not Acknowledged.\n");
			return 0;

	}
}
