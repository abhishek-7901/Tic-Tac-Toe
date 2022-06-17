#include<bits/stdc++.h> 
#include <iostream> 
#include <cstdio> 
#include<vector> 
#include <stdio.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <assert.h> 
#define deb(x) cerr << #x << "=" << x << endl
#define deb2(x, y) cerr << #x << "=" << x << "," << #y << "=" << y << endl
#define deb3(x,y,z) cerr << #x << "=" << x << "," << #y << "=" << y << "," << #z << "=" << z<<endl

#define PORT 8080
#define SA struct sockaddr

#define MAX 80
#define TIMEOUT 15 

using namespace std ; 

struct information{
	int flag ; 
	// 0 for display message 
	// 1 for displaying the board 
	// 2 to say it is your turn 
	// 3 to say it is time to end 
	int client ; 
	char msg[MAX] ; 	
    
} ;

typedef struct information info  ; 

class Client{
public:
    // vector<string> grid ; 
    char grid[10] ; 
    string turn = "X" ; 
    string you ; 
    string opp ;

    string winner = " " ; 
    bool game_over = false ; 
    char buff[MAX] ; 
    int client_id = -1 ; 
    int sockfd, connfd ; 
    struct sockaddr_in servaddr, cli;
    info incoming ; 
    info outgoing ; 
    bool timeout_flag = true ; 

    Client(){
        for (int i = 0 ; i < 9; i++){
            grid[i] = ' ' ; 
        }
        grid[9] = '\0' ; 

        sockfd = socket(AF_INET, SOCK_STREAM, 0);
        if (sockfd == -1) {
            printf("socket creation failed...\n");
            exit(0);
        }


        bzero(&servaddr, sizeof(servaddr));
        servaddr.sin_family = AF_INET;
	    // servaddr.sin_addr.s_addr = inet("192.") ;
        if (inet_aton(argv[1], &servaddr.sin_addr.s_addr) == 0) {
            fprintf(stdout, "Bad Hostname!\n");
            exit(1);
        } 
	    servaddr.sin_port = htons(PORT);
        
        if (connect(sockfd, (SA*)&servaddr, sizeof(servaddr)) != 0) {
            printf("connection with the server failed...\n");
            exit(0);
        }

        get_client_id() ;
        // cout << "client id is: " ; 
        // cout << client_id<< endl ;  
        
        start_game1() ; 
    }
    void display_board(string& s){
        // assert(s.size() == 9) ;
        if (s.size() != 9){
            cout << "some error is there" << endl ; 
            cout << s << endl ; 
            cout << s.size() << endl  ; 
            cout << "error in display_board" << endl ;
            exit(0) ;
        }

        for(int i = 0 ; i < 3 ; i ++){
            string row = "" ; 
            for (int j = 0 ; j < 3 ; j++){
                int id = 3*i + j ; 
                row += s[id] ;

                if(j < 2){
                    row += "|" ; 
                }

            }
            cout << row << endl ; 
        }
    }

    void send_server(int move1, int move2){
        strcpy(outgoing.msg, "\0") ; 
        outgoing.msg[0] = '0' + move1 - 1;
        outgoing.msg[1] = '0' + move2 -1; 
        outgoing.msg[2] = '\0' ; 
        send(sockfd, &outgoing, sizeof(outgoing) + 1, 0) ; 
    }
    void send_server_msg(string s){
        strcpy(outgoing.msg, "\0") ; 
        strcpy(outgoing.msg, s.c_str()) ; 
        outgoing.msg[s.size()] = '\0' ; 
        send(sockfd, &outgoing, sizeof(outgoing) + 1, 0) ; 
    } 
    bool invalid(int move1, int move2, string grid){
        if (move1 < 1 || move1 > 3 || move2 < 1 || move2 > 3){
            return true ; 
        }
        if (grid[3*(move1-1) + (move2-1)] != '_'){
            return true ; 
        }
        return false ; 
    } 
    string read_string(){
        string s ; 
        unset_flag() ; 
        // cout<<"Before performing cin operation"<<endl;

        //Below cin operation should be executed within stipulated period of time
        fd_set readSet;
        FD_ZERO(&readSet);
        FD_SET(STDIN_FILENO, &readSet);
        struct timeval tv = {TIMEOUT, 0};  // 10 seconds, 0 microseconds;
        if (select(STDIN_FILENO+1, &readSet, NULL, NULL, &tv) < 0) perror("select");

        bool b = (FD_ISSET(STDIN_FILENO, &readSet)) ? (bool)(cin>>s) : false;

        if(b==1){
            // cout << " got input" << endl ; 
            return s ;

        }
        else{
            cout << "TIMED OUT" << endl ;
            set_flag() ; 
            // exit(0)  ;  
        }

    }
    void set_flag(){
        timeout_flag = true ; 
    }
    void unset_flag(){
        timeout_flag = false ;  
    } 
    pair<int,int> read_int(){
        unset_flag() ; 
        int n,m;
        // cout<<"Before performing cin operation"<<endl;

        //Below cin operation should be executed within stipulated period of time
        fd_set readSet;
        FD_ZERO(&readSet);
        FD_SET(STDIN_FILENO, &readSet);
        FD_SET(STDOUT_FILENO, &readSet);
        FD_SET(sockfd, &readSet);

        struct timeval tv = {TIMEOUT, 0};  // 10 seconds, 0 microseconds;
        if (select(STDIN_FILENO+1, &readSet, NULL, NULL, &tv) < 0) perror("select");
//1 
//1  
        bool b = (FD_ISSET(STDIN_FILENO, &readSet)) ? (bool)(cin>>n >> m ) : false;

        if(b==1){
            // cout << " got input" << endl ; 
            return {n,m} ; 

        }
        else{
            cout << "TIMED OUT" << endl ;
            set_flag() ; 
            // exit(0)  ;  

        }

    }
    // string read_move(){
    //     int moves[2] ; 
    //     for(int i = 0 ; i < 2 ; i++){
    //         moves[i] = read_int() ;
    //     }
    //     return to_string(moves[0]) + " " + to_string(moves[1]) ;
    // }
    void start_game1(){
        // cout << "$1" << endl ; 
        // cout << " entered the intended function " << endl; 
        // cout << "Connected to the game server. Your player ID is " << client_id << endl ;  
        while(1){
            // strcpy(buff, "") ;
            strcpy(incoming.msg, "\0")  ; 

            int recBytes = recv(sockfd, (info*)&incoming, sizeof(incoming) + 1, 0) ; 

            if (recBytes < 0){
                cout << "error in recv" << endl ;  
                exit(0) ; 
            }
            if (recBytes == 0){
                cout << "server closed" << endl ; 
                exit(0) ; 
            } 
            int flag = incoming.flag ; 

            // string str = string(buff) ; 
            // int flag = stoi(str) ; 

            if (flag == 0){
                //message has arrived 
                strcpy(buff, incoming.msg) ;
                string str = string(buff) ;
                cout << str << endl ; 
            }
            else if (flag == 1){
                //board display
                strcpy(buff, incoming.msg) ; 
                string str = string(buff) ; 
                display_board(str) ; 
            }
            else if(flag == 2){
                //input give
                strcpy(buff, incoming.msg) ;
                string str = string(buff) ; 
                display_board(str) ;
                int move1, move2 ; 

                int client = incoming.client ; 
                if (client == client_id){
                    cout << "Enter (ROW, COL) for placing your mark: " << endl ; 
                    // fflush(stdin) ; 
                    // cin.ignore(numeric_limits<streamsize>::max(),'\n');
                    pair<int,int> move = read_int() ;   
                    if (timeout_flag){
                        continue; ; 
                    }
                    int move1 = move.first ; 
                    int move2 = move.second ; 

                    while(invalid(move1, move2, str)){
                        // fflush(stdin) ; 
                        // cin.ignore(numeric_limits<streamsize>::max(),'\n');

                        cout << "Invalid move. Enter again: " << endl ; 
                        display_board(str) ; 
                        move = read_int() ;   
                        move1 = move.first ; 
                        move2 = move.second ; 
                        if (timeout_flag){
                            break ; 
                        } 
                        // deb2(move1, move2) ;
                    }

                    send_server(move1, move2) ; 
                }   

            }
            else if (flag == 3){
                // send the id of the client who won 
                strcpy(buff, incoming.msg) ;
                string str = string(buff) ; 
                display_board(str) ;
                int client = incoming.client ; 
                if (client == -1){
                    cout << "It was a draw" << endl ; 
                }
                else if (client == client_id){
                    cout << "you win !" << endl ; 
                }
                else {
                    cout << "you lose" << endl ; 
                }
                // exit(0) ; 
            }
            else if (flag == 4){
                cout << "game has ended" << endl ; 
                // close(sockfd) ; 
                exit(0) ;
            }
            else if (flag == 5){
                cout << "If you want to play again type \'YES\' "<< endl ;
   
                string choice ; 
                choice = read_string() ; 
                if (timeout_flag){
                    exit(0) ; 
                }
                send_server_msg(choice) ; 
            }
        }
    }

    void get_client_id(){
        recv(sockfd, buff, sizeof(buff), 0) ; 
        string str = string(buff) ; 
        int id  = stoi(str) ;
        client_id = id ; 
    }

}; 


int main(){
    Client() ;
    return 0 ; 
}