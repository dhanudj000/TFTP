#include "tftp.h"

#define PORT 	 5000
#define MAX_BUFF 512
#define IP	 "127.0.0.1"

int main()
{
	int sockfd,cli_len,file_fd,n,block=0,tries=0,data_len = -1;
	void *recv_buff = malloc(MAX_BUFF) , *send_buff = malloc(MAX_BUFF);
	struct sockaddr_in serv,cli;
	serv.sin_port = htons(PORT);
	serv.sin_family = AF_INET;
	serv.sin_addr.s_addr = inet_addr(IP);
	sockfd = socket(AF_INET , SOCK_DGRAM , 0);
	bind(sockfd ,(struct sockaddr*)&serv , sizeof(struct sockaddr_in));
	cli_len = sizeof(struct sockaddr_in);
reconnect:
	print(YELLOW , "Waiting for connection.....\n");
	recvfrom(sockfd , recv_buff , 512 , 0 , (struct sockaddr*)&cli , &cli_len);
	//perror("recvfrom");
	SET_COL(_GREEN);
	printf("Connection established.\nClient PORT: %d\nClient IP  :%s\n", ntohs(cli.sin_port) , inet_ntoa(cli.sin_addr));
	SET_COL(_RESET);
	create_pack(ACK , NULL , 0 ,send_buff , 0);
	sendto(sockfd , send_buff , 512 , 0 , (struct sockaddr*)&cli , cli_len);
	print(MAGENTA , "Acknowledgement sent.\n");
	struct timeval t;
	fd_set set;
	while(1)
	{
		recvfrom(sockfd , recv_buff , MAX_BUFF , 0 , (struct sockaddr*)&cli , &cli_len);
		perror("recvfrom");
		switch(*(short*)recv_buff)
		{
			case RRQ :
				print(BLUE , "Recieved RRQ request.\n");
				file_fd = open(recv_buff+2 , O_RDONLY);
				if(file_fd == -1)
				{
					create_pack(ERR , NULL , ERR1 , send_buff , 0);
					sendto(sockfd , send_buff , MAX_BUFF , 0 , (struct sockaddr*)&cli , cli_len);
					break;
				}
				while(n = read(file_fd , send_buff+6 , 506))
				{
					block+=1;
					create_pack(DATA , NULL , block , send_buff , n);
retransmit:				sendto(sockfd , send_buff , MAX_BUFF , 0 , (struct sockaddr*)&cli , cli_len);
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
						printf("Tried retransmitting data 5 times.Disconnecting client....");
						create_pack(ERR , NULL , ERR4 , send_buff , 0);
						sendto(sockfd , send_buff , MAX_BUFF , 0, (struct sockaddr*)&cli , cli_len);
						goto reconnect;
					}
					print(YELLOW , "Data packet lost , Retransmitting data\n");
					goto retransmit;
					default :
					recvfrom(sockfd , recv_buff , MAX_BUFF , 0 , (struct sockaddr*)&cli , &cli_len);
					}
					if(*(short*)recv_buff = ACK)
						continue;
				}
				create_pack(DATA , NULL , block+1 , send_buff , 0);
				sendto(sockfd , send_buff , MAX_BUFF , 0 , (struct sockaddr*)&cli , cli_len);
				block = 0;
				break;
			case WRQ :
				print(BLUE , "Recieved WRQ request.\n");
				file_fd = open(recv_buff+2 , O_WRONLY | O_CREAT | O_TRUNC , 0666);
				if(file_fd == -1)
				{
					create_pack(ERR , NULL , ERR2 , send_buff , 0);
					sendto(sockfd , send_buff , MAX_BUFF , 0 , (struct sockaddr*)&cli , cli_len);
					continue;
				}
				create_pack(ACK , NULL , 0 , send_buff , 0);
				sendto(sockfd , send_buff , MAX_BUFF , 0 , (struct sockaddr*)&cli , cli_len);
				while(recvfrom(sockfd , recv_buff , MAX_BUFF , 0 , (struct sockaddr*)&cli , &cli_len))
				{
					//perror("recvfrom");
					printf("%d %d.\n",*(short*)recv_buff , *(short*)(recv_buff+2));
                                        switch(*(short*)recv_buff)
                                        {
                                                case DATA:
                                                        data_len = *(short*)(recv_buff+4);
                                                        n = write(file_fd , recv_buff+6 , data_len);
                                                        create_pack(ACK , NULL , *(short*)(recv_buff+2) , send_buff , n);
                                                        sendto(sockfd , send_buff , MAX_BUFF , 0 ,(struct sockaddr*)&cli , cli_len);
							perror("sendto");
                                                        print(MAGENTA , "Acknowledgement sent for block : "); printf("%d\n",*(short*)(recv_buff+2));
                                                        break;
                                                case ERR :
                                                        data_len = 0;
                                                        print_error(recv_buff);
                                                        break;
                                        }
                                        if(data_len == 0)
                                        {
                                                print(GREEN , "Transfer Success!!!!!\n");
                                                data_len = -1;
                                                if(file_fd != -1)
                                                        close(file_fd);
                                                //first_entry = 1;
                                                break;
                                        }
                                }
                                file_fd = -1;
				break;
			case ERR:
				goto reconnect;
		}
	}
}
