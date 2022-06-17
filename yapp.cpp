#include<bits/stdc++.h> 
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <netinet/ip_icmp.h>
#include <time.h>
#include <fcntl.h>
#include <signal.h>
#include <time.h>
#include<iostream> 
#include<cstdio> 


#define deb(x) cerr << #x << "=" << x << endl
#define deb2(x, y) cerr << #x << "=" << x << "," << #y << "=" << y << endl
#define deb3(x,y,z) cerr << #x << "=" << x << "," << #y << "=" << y << "," << #z << "=" << z<<endl
using namespace std :: chrono ; 

using namespace std ; 
#define PING_PKT_S 64
#define TIMEOUT_SECS 10
#define PORT_NUMBER 8888

struct ping_pkt
{
	struct icmphdr hdr;
	char msg[PING_PKT_S-sizeof(struct icmphdr)];
};

void send_ping(int sockfd, struct sockaddr_in *dst){

    struct ping_pkt pkt ; 
    struct timeval tv ; 
    tv.tv_sec = TIMEOUT_SECS ;
    tv.tv_usec = 0 ;
    // cout << "time out time " << 
    // deb(tv.tv_sec) ; 
//set packet information 
    bzero(&pkt, sizeof(pkt)) ;
    //indicates this is a ping request  
    pkt.hdr.type = ICMP_ECHO ; 

// set the timeout
    if (setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, (const char*)&tv, sizeof(tv)) < 0){
        perror("setsockopt failed\n");
        exit(1);
    }

//record the starting time
    auto start = high_resolution_clock::now();

    //send the packet
    if (sendto(sockfd, &pkt, sizeof(pkt), 0, (struct sockaddr*)dst, sizeof(*dst)) < 0){
        perror("sendto failed\n");
        exit(1); 
    }

    struct sockaddr_in tmp ;
    int addrlen = sizeof(tmp) ; 

    if (recvfrom(sockfd, &pkt, sizeof(pkt), 0, (struct sockaddr*)&tmp, (socklen_t*)&addrlen) < 0){
        // perror("recvfrom failed\n");
        puts("Host is not reachable\n"); 
        exit(1); 
    } 
//record the ending time
    auto end = high_resolution_clock::now(); 
// calculate the round trip time
    auto rtt = duration_cast<milliseconds>(end - start);

    cout << "Round trip time: " << rtt.count() << " ms" << endl; 

}

int main(int argc, char** argv){
    // cout << sizeof (struct icmphdr) << endl ; 
    
// checking if the right number of arguments are passed 
    if(argc!=2)
	{
		printf("Bad Hostname!");
		return 0;
	}

    
//getting a socket
    struct sockaddr_in  dst; 
    int sockfd ; 
    sockfd = socket(AF_INET, SOCK_DGRAM, IPPROTO_ICMP) ; 
    // sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP) ; 

    if (sockfd == -1) {
        perror("Error creating socket");
    }
    // puts("socket created") ; 
    
// destination socket initialization  
    dst.sin_family = AF_INET ; 
    if (inet_aton(argv[1], &dst.sin_addr) == 0) {
        fprintf(stdout, "Bad Hostname!\n");
        exit(1);
    }
    dst.sin_port = htons(PORT_NUMBER) ;

    // puts("ping function called")  ; 
    send_ping(sockfd, &dst)  ; 
}