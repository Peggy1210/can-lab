#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <time.h>
#include <arpa/inet.h> 
#include <math.h>

/*****************notice**********************
 * 
 * You can follow the comment inside the code.
 * This kind of comment is for basic part.
 * =============== 
 * Some hints...
 * ===============
 * 
 * This kind of comment is for bonus part.
 * ---------------
 * Some hints...
 * ---------------
 * 
 * 
 * 
 *********************************************/

// ==============
//  Packet header 
// ==============
typedef struct header {
	unsigned int seq_num;
	unsigned int ack_num;
	unsigned char is_last;
	int last_no_byte;
} Header;

// ==================
//  Udp packet & data 
// ==================
typedef struct udp_pkt {
	Header header;
	char data[1024];
} Udp_pkt; 

// ============
//  Declaration
// ============
int sockfd = 0;
Udp_pkt snd_pkt, rcv_pkt;
struct sockaddr_in info, client_info;
socklen_t len;
time_t t1, t2;

// =====================
//  Simulate packet loss
// =====================
int isLoss(double prob) {
    double thres = prob * RAND_MAX;
    
    if(prob >= 1) return 1; // loss
    return (rand() <= thres); // not loss
}

// Some busy function
int frequency_of_primes(int n) {
  int i,j;
  int freq = n - 1;
  for(i=2; i<=n; ++i) for(j=sqrt(i); j>1; --j) if(i % j == 0) { --freq; break; }
  return freq;
}

// ==================================
//  You should complete this cunction
// ==================================
int recvFile(FILE *fd) {
	printf("FILE_EXISTS\n");

	char* str;
	char fileName[30];

	// ==================================================================
	//  Split the command into "download" & "filename", just get filename
	// ==================================================================
	str = strtok(snd_pkt.data, " \n");
	str = strtok(NULL, " \n");
	
	sprintf(fileName, "download_");
	strcat(fileName, str);
	
	// Open file
	fd = fopen(fileName, "wb");
	
	printf("Receiving...\n");
	memset(snd_pkt.data, '\0', sizeof(snd_pkt.data));
	while(1) {
		// =======================
		//  Simulation packet loss
		// =======================

		recvfrom(sockfd, &rcv_pkt, sizeof(rcv_pkt), 0, (struct sockaddr *) &info, (socklen_t *) &len);

		if(isLoss(0.5)) {
			printf("\tOops! Packet loss!\n");
		} else {
			// ==============================================
			//  Actually receive packet and write into buffer
			// ==============================================
			printf("\tReceive a packet seq_num = %d\n", rcv_pkt.header.seq_num);

			// ====================
			//  Reply ack to server
			// ====================
			snd_pkt.header.ack_num = rcv_pkt.header.seq_num;
			snd_pkt.header.seq_num = 0;
			snd_pkt.header.is_last = 0;
			sendto(sockfd, &snd_pkt, sizeof(snd_pkt), 0, (struct sockaddr *) &info, len);
			printf("\tSend ACK of packet %d\n", rcv_pkt.header.seq_num);

			// ==============================================
			//  Write buffer into file if is_last flag is set
			// ==============================================
			if(rcv_pkt.header.is_last == 1) {
				fwrite(rcv_pkt.data, 1, rcv_pkt.header.last_no_byte, fd);
				break;
			} else {
				fwrite(rcv_pkt.data, 1, sizeof(rcv_pkt.data), fd);
			}
		}

		// Clear packet data
		memset(rcv_pkt.data, '\0', sizeof(rcv_pkt.data));
	}

	fseek(fd, 0, SEEK_SET);
	fseek(fd, 0, SEEK_END);
	int filesize = ftell(fd);
	printf("End of receiving\nTotal byte received: %d\n", filesize);

	// Close file
	fclose(fd);

	return 0;
}

int main(int argc, char *argv[]) {
	// ==============
	//  Create socket
	// ==============
	// int sockfd = 0;
	sockfd = socket(AF_INET, SOCK_DGRAM, 0);
	if (sockfd == -1) {
		printf("Fail to create a socket.");
		return -1;
	}


	// ==================
	//  Input server info
	// ==================
	// struct sockaddr_in info;
	bzero(&info, sizeof(info));
	info.sin_family = AF_INET;

	char* server_ip = (char *) malloc(sizeof(char) * 30);
	int server_port;

	printf("give me an IP to send: ");
	scanf("%s", server_ip);
	printf("server's's port? ");
	scanf("%d", &server_port);


	// ==================================
	//  Just test how to convert the type
	// ==================================	
	info.sin_addr.s_addr = inet_addr(server_ip);
	info.sin_port = htons(server_port);

	server_port = htons(info.sin_port);
	server_ip = inet_ntoa(info.sin_addr);
	// printf("server %s : %d\n", server_ip, server_port);


	// ====================================
	//  Create send packet & receive packet
	// ====================================
	memset(snd_pkt.data, '\0', sizeof(snd_pkt.data));
	len = sizeof(info);

	printf("Waiting for a commands...\n");
	getchar();
	while(fgets(snd_pkt.data, 30, stdin)) {
        
        // ================================
        //  command "exit": close the client
        // ================================
		if(strncmp(snd_pkt.data, "exit", 4) == 0) {
            break;
        // ==============================================================
        //  command "download filename": download the file from the server
        // ==============================================================
        }
		else if(strncmp(snd_pkt.data, "download", 8) == 0) {
			snd_pkt.header.seq_num = 0;
			snd_pkt.header.ack_num = 0;
			snd_pkt.header.is_last = 1;
			//We first set is_last to 1 so that server know its our first message.
			int numbytes;
			FILE* fd;
			// ========================
			//  Send filename to server
			// ========================
			if((numbytes = sendto(sockfd, &snd_pkt, sizeof(snd_pkt), 0, (struct sockaddr *) &info, len)) == -1) {
				perror("error");
				return 0;
			}
			printf("client: sent %d bytes to %s\n", numbytes, inet_ntoa(info.sin_addr));
			// =========================================
			//  Get server response if file exist or not
			// =========================================
			if((numbytes = recvfrom(sockfd, &rcv_pkt, sizeof(rcv_pkt), 0, (struct sockaddr *) &info, (socklen_t *) &len)) == -1) {
				printf("recvfrom error\n");
				return 0;
			}	
			printf("client: receive %d bytes from %s\n", numbytes, inet_ntoa(info.sin_addr));
			// printf("%s", buf);
			
			// ====================
			//  File does not exist
			// ====================
			if(strcmp(rcv_pkt.data, "FILE_NOT_EXISTS") == 0) {
				printf("FILE_NOT_EXISTS\n");
			} 
			// ==========================
			//  File exists, receive file
			// ==========================
			else if(strcmp(rcv_pkt.data, "FILE_EXISTS") == 0) {
			    t1 = time(NULL);
				recvFile(fd);
				t2 = time(NULL);
				printf("Total cost %ld secs\n", t2 - t1);
			}	
			
		} else {
			printf("Illegal command\n");	
		}
        
        printf("Waiting for a commands...\n");
    }

}


