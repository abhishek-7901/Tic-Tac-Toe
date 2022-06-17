#include<bits/stdc++.h> 
#include <iostream> 
#include <cstdio> 
#include <stdio.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sstream>
#include <string>  
#include <vector> 
#include <thread> 
#include <mutex> 

#define MAX 80
#define PORT 8080

#define TIMEOUT 20
using namespace std::chrono ; 
#define deb(x) cerr << #x << "=" << x << endl
#define deb2(x, y) cerr << #x << "=" << x << "," << #y << "=" << y << endl
#define deb3(x,y,z) cerr << #x << "=" << x << "," << #y << "=" << y << "," << #z << "=" << z<<endl

#define SA struct sockaddr


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



using namespace std ; 


mutex m;
int player_count = 0 ; 

class Server{
public: 
	int listenfd,len;
	struct sockaddr_in servaddr ; 
	struct sockaddr_in clients[2]; 
	int connfds[2] ;
	string grid[3][3] ; 
	info outgoing ;
	info incoming ; 
	struct timeval tv ; 
	clock_t game_start ; 
    ofstream LOG ; 

    void operator()(int arr[], int cnt){

		for(int i= 0 ; i < 2 ; i++){ 
			connfds[i] = arr[i] ; 
		} 
		string s = "log" ; 
		s += to_string(cnt) ; 
		s += ".txt" ; 
        LOG = ofstream(s) ; 
        

		tv.tv_sec = TIMEOUT ; 
		tv.tv_usec = 0 ; 
		// outgoing = (info*)malloc(sizeof(info)) ; 

		for (int i = 0 ; i < 3 ; i++){
			for (int j = 0 ; j < 3 ; j++){
				grid[i][j] = "_" ; 
			}
		}  
		// deb2(tokens[0], tokens[1]) ;

		
		
		len = sizeof(clients[0]);
		// make_connections() ; 	
		set_time_out() ; 
		start_game() ; 
		close(listenfd) ; 
	}

	void set_time_out(){
		for (int i = 0 ; i < 2 ; i++){
			setsockopt(connfds[i], SOL_SOCKET, SO_RCVTIMEO, (char *)&tv, sizeof(struct timeval)); 
		}
	}
	string convert_to_string(){
		string s = "";
		for (int i =0 ; i < 3 ; i++){
			for (int j = 0 ; j < 3 ; j++){
				s += grid[i][j] ; 
			}
		} 

		return s ; 
	}

	int check_win(){

		auto check_win_row = [&](int row){
			string s = grid[row][0] ;
			if (s == "_"){
				return 0 ; 
			} 
			for (int col = 0 ; col < 3 ; col ++){
				if (s != grid[row][col]){
					return 0 ; 
				} 
			}
			return 1 ; 
		}; 

		auto check_win_col = [&](int col){
			string s = grid[0][col] ;
			if (s == "_"){
				return 0 ; 
			} 
			for (int row = 0 ; row < 3 ; row ++){
				if (s != grid[row][col]){
					return 0 ; 
				} 
			}
			return 1 ; 
		}; 

		for(int i = 0 ; i < 3 ; i++){
			if (check_win_row(i)){
				// cout << "this is a mistake 0" << endl ; 
				// cout << i << endl ; 
				return 1 ; 
			} 
		}
		for(int i = 0 ; i < 3 ; i++){
			if (check_win_col(i)){
				// cout << "this is a mistake 1" << endl ; 
				return 1 ; 
			} 
		}
		
		if (grid[0][0] == grid[1][1] && grid[1][1] == grid[2][2] and grid[0][0] != "_"){
			// cout << "this is a mistake2 " << endl ; 
			return 1 ; 
		} 
		if (grid[0][2] == grid[1][1] && grid[1][1] == grid[2][0] and grid[0][2] != "_"){
			// cout << "this is a mistake 3" << endl ; 
			return 1 ; 
		}
		return 0  ; 
	}
	bool is_timeout(){
		if (errno == EAGAIN or errno == EWOULDBLOCK){ 
			return true  ; 
		} 
		return false ; 
	}

void start_game(){
        game_start = clock() ; 
        clock_t curr ; 
		int player = 0 ; 
		int waiter = 1 ; 

		bool draw = true ; 
		string tokens[2] = {"X", "O"} ; 

		send_clients(0, "Welcome to Tic Tac Toe") ; 
		for (int i =0  ; i < 9 ; i++){

			outgoing.client = player ; 
			send_clients(2, convert_to_string()) ; 

			int recvBytes = recv(connfds[player], (info*)&incoming, sizeof(incoming) + 1, 0) ; 
			if (recvBytes == 0){
				draw = false ; 
				LOG << "PLAYER " << player  << " has TIMED OUT" << endl ; 
				break ; 
			}
			if (recvBytes < 0){
				perror("recv") ; 
				if (is_timeout()) {
					draw= false ;
					LOG << "PLAYER " << player  << " has TIMED OUT" << endl ; 

					break ; 
					// send_client(connfds[player], 0, "You have been disconnected because you are taking too much time") ; 
					// send_client(connfds[player], 4, " ") ; 
				}
                curr = clock() - game_start ; 
				// perror("recv failed") ;  
                LOG << "Time: " << curr << "ms Player " << player << " disconnected" << endl ;

				// cout << "this was not expected" << endl ;
				// cout << "player " << player << " has disconnected" << endl ; 
				send_client(connfds[waiter], 0, "Sorry your partner disconnected") ; 
				send_client(connfds[waiter], 4, " ") ; 
				close(listenfd) ; 
				exit(0)  ; 
			}

			// update_board(tokens[player]) ;  
            int x = incoming.msg[0] - '0' ; 
		    int y = incoming.msg[1] - '0' ; 

		    grid[x][y] = tokens[player] ; 
            curr = clock() - game_start ; 
            assert(incoming.msg[2] == '\0') ; 
            LOG <<  "Time: " << curr << "ms Player " << player << " played " << incoming.msg[0] << " " << incoming.msg[1] << endl ; 
            
            if (check_win()){
                curr = clock() - game_start ; 
                LOG << "Time: " << curr << "ms Player " << player << " won" << endl ; 
				outgoing.client = player ; 
				send_clients(3,convert_to_string()) ;  
				
				draw = false ; 
				break ; 
			}

			player = 1 - player ; 
			waiter = 1 - waiter ; 
		}
		// cout << "stack smashed ?? " << endl ; 
		if (draw == true){
            curr = clock() - game_start ; 
            LOG << "Time: " << curr << "ms Game ended in a draw" << endl ; 
			cout << " it is a draw " << endl ; 
			outgoing.client = -1 ;
			send_clients(3, convert_to_string()) ; 
		}
	
	if (want_to_play_again()){
        curr = clock() - game_start ; 
		LOG << "Time : " << curr << " " << "ms players are playing again " <<  endl ;
        LOG << endl ; 
        LOG << endl ; 
        LOG << endl ; 
        // LOG << "Time "
        for(int i = 0 ; i < 3 ; i++ ){
			for(int j = 0 ; j < 3 ; j++){
				grid[i][j] = "_" ;  
			}
		}

		send_clients(0, "\n") ; 
		send_clients(0, "\n") ; 
		send_clients(0, "Symbols are same as the previous game") ; 
		start_game() ; 
	}

	//for ending the game
		send_clients(4, " ") ; 
		return ;
	}

	// void recieve_
	bool want_to_play_again(){
		// send_clients(0, "If you want to play again, type YES") ;
		send_clients(5, " ") ; 
		bool flag =false ; 

		for (int player = 0 ; player < 2 ; player ++){
			flag = false; 
			int waiter = 1 - player ; 
			int recvBytes = recv(connfds[player], (info*)&incoming, sizeof(incoming) + 1, 0) ; 
			string response = string(incoming.msg) ;
			if (response == "YES"){
				flag = true ; 
			} 
			// deb(response) ; 
			if (recvBytes <= 0){
				// cout << "this was not expected" << endl ;
				// cout << "player " << player << " has disconnected" << endl ; 
				send_client(connfds[waiter], 0, "Sorry your partner disconnected") ; 
				send_client(connfds[waiter], 4, " ") ; 
				close(listenfd) ; 
				exit(0)  ; 
			}
			if (flag == false){
				send_client(connfds[waiter], 0, "Sorry your partner disconnected") ; 
				return false ; 
			}
		}
		return true ; 
	}
	void send_clients(int f, string msg){
		for(int i =0  ; i < 2 ; i++){
			send_client(connfds[i], f, msg) ; 
		}
	}
	void send_client(int fd, int f, string msg){
		strcpy(outgoing.msg, msg.c_str()) ; 
		outgoing.flag = f ;
		send(fd, &outgoing, sizeof(outgoing)+1, 0) ;
	}
	string integer_to_string(int i) {
		stringstream ss;
		ss << i;
		return ss.str();
	} 
	

	//accepts connections and sends the clients ids 
	// void make_connections(){
    //     string tokens[2] = {"X", "O"} ; 
	// 	int cnt = 0 ;
	// 	puts("Game server started. Waiting for players.") ;
	// 	for(int i =0 ; i < 2 ;i++){
	// 		connfds[i] = accept(listenfd, (SA*)&clients[i], (socklen_t*)&len);
	// 		// cout << "hi" << endl ; 
	// 		if (connfds[i] < 0) {
	// 			printf("server acccept failed...\n");
	// 			exit(0);
	// 		}
	// 		else{
	// 			string str = integer_to_string(i) ;  
	// 			send(connfds[i], str.c_str(), str.size()+1, 0 ) ; 
	// 			printf("server accepted the client %d \n", cnt++) ; 
	// 		}
	// 		sleep(1) ; 
	// 		if ( i == 0){
	// 		    send_client(connfds[0], 0, "Connected to the game server. Your player ID is 1") ; 
	// 			send_client(connfds[0], 0, "Waiting for a partner to join...") ; 
	// 		}
    //         else {
    //             send_client(connfds[1], 0, "Connected to the game server. Your player ID is 2") ; 
	// 			// send_client(connfds[i], 0, "Connected to the game server...") ; 
    //         }
	// 	} 

	// 	for(int i = 0; i < 2 ; i++){
	// 		string s ;
	// 		s  = "Your partner's ID is ";
	// 		s += integer_to_string(1 - i + 1) ; 
	// 		s += "." ; 
	// 		s += " Your symbol is " ; 
	// 		s += tokens[i]; 
	// 		send_client(connfds[i], 0, s) ; 
	// 		// string str  = convert_to_string() ; 
	// 		// cout << " grid to be sent " << str << endl ;
	// 		// send_client(connfds[i], 1, convert_to_string()) ; 
	// 	}
	// 	// deb2(tokens[0], tokens[1]) ; 
	// 	// cout<< " grid has been sent " << endl ;
	// }; 

}; 

void send_client(int fd, int f, string msg){ 
	struct information outgoing ; 
	strcpy(outgoing.msg, msg.c_str()) ; 
	outgoing.flag = f ;
	send(fd, &outgoing, sizeof(outgoing)+1, 0) ;
}

void send_clients(int f, string msg, int* connfds){ 
	for(int i =0  ; i < 2 ; i++){
		send_client(connfds[i], f, msg) ; 
	}
}

string integer_to_string(int i) {
	stringstream ss;
	ss << i;
	return ss.str();
} 


//accepts connections and sends the clients ids 
void make_connections(int listenfd, int* connfds, SA* clients){
	if ((listen(listenfd, 5)) != 0) {
		printf("Listen failed...\n");
		exit(0);
	}
	socklen_t* len = (socklen_t*)sizeof(clients[0]) ; 
	string tokens[2] = {"X", "O"} ; 
	int cnt = 0 ;
	puts("Game server started. Waiting for players.") ;
	for(int i =0 ; i < 2 ;i++){
		connfds[i] = accept(listenfd, (SA*)&clients[i], (socklen_t*)&len);
		// cout << "hi" << endl ; 
		if (connfds[i] < 0) {
			printf("server acccept failed...\n");
			exit(0);
		}
		else{
			string str = integer_to_string(i) ;  
			send(connfds[i], str.c_str(), str.size()+1, 0 ) ; 
			printf("server accepted the client %d \n", cnt++) ; 
		}
		sleep(1) ; 
		if ( i == 0){
			send_client(connfds[0], 0, "Connected to the game server. Your player ID is 1") ; 
			send_client(connfds[0], 0, "Waiting for a partner to join...") ; 
		}
		else {
			send_client(connfds[1], 0, "Connected to the game server. Your player ID is 2") ; 
			// send_client(connfds[i], 0, "Connected to the game server...") ; 
		}
	} 

	for(int i = 0; i < 2 ; i++){
		string s ;
		s  = "Your partner's ID is ";
		s += integer_to_string(1 - i + 1) ; 
		s += "." ; 
		s += " Your symbol is " ; 
		s += tokens[i]; 
		send_client(connfds[i], 0, s) ; 
		// string str  = convert_to_string() ; 
		// cout << " grid to be sent " << str << endl ;
		// send_client(connfds[i], 1, convert_to_string()) ; 
	}
	m.lock() ; 
	player_count += 2; 
	m.unlock() ; 
}

int make_bind(){
	struct sockaddr_in servaddr; 
	int listenfd = socket(AF_INET, SOCK_STREAM, 0)  ; 
	if (listenfd == -1){
		printf("socket creating failed.. \n") ;
		exit(0) ; 
	}

	bzero(&servaddr, sizeof(servaddr));

	// assign IP, PORT
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = INADDR_ANY;
	servaddr.sin_port = htons(PORT);

	// Binding newly created socket to given IP and verification
	if ((bind(listenfd, (SA*)&servaddr, sizeof(servaddr))) != 0) {
		printf("socket bind failed...\n");
		exit(0);
	}

	return listenfd ; 
	// Now server is ready to listen and verification

}
class thread_obj {
public:
    void operator()(int x)
    {
        for (int i = 0; i < x; i++)
            cout << "Thread using function"
                  " object as  callable\n";
    }
};
// Driver function
int main()
{
	// Server s ; 
	// thread th1(thread_obj(), 5) ; 
	// th1.join() ; 
	// th1.detach() ; 
	int listenfd = make_bind() ; 
	// int connfds[2] ;
	SA clients[2] ; 
	// make_connections(listenfd, connfds, clients) ; 
	// thread th2(Server(), ref(connfds)) ;  
	// thread th3(Server(), 5) ; 
	// th2.detach() ; 

	int cnt = 0 ;

	int* connfds ; 
	connfds = new int[2] ; 
	while(1){
		make_connections(listenfd, connfds, clients) ; 
		thread th2(Server(), ref(connfds), ++cnt) ;  
		// thread th3(Server(), 5) ; 
		th2.detach() ;
	}
	// th3.join() ; 
	// th1.join() ; 
	// th1.join() ; 
	// cout << " stack  smashedddd >>>> " << endl ; 
}

