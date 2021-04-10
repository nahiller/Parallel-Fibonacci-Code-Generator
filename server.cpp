#include <iostream>
#include <vector>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/wait.h>
#define MESSAGESIZE 500
using namespace std;
struct message
{
    char message[MESSAGESIZE];
    char symbol;
};

//--------Fireman() is from BB--------//

void fireman(int)
{
   while (waitpid(-1, NULL, WNOHANG) > 0){
      //std::cout << "A child process ended" << std::endl;
   }
}
void error(char *msg)
{
    perror(msg);
    exit(1);
}
int fib(int n){
    
    if(n<=1){//Base cause for recurrsive function
        return n;
    }
    return fib(n-1)+fib(n-2);
}
string fibDecode(int n){
    string answer="";
    string temp="";
    string temp2="";
    vector <int> fibb(n);
    for(int i=0; i<n; i++){
        fibb[i]=fib(i+2);//I want the starting value of the vector to be 1 so I call fib with i+2 to get 1 as the starting value and it increments from there
    }
    for(int i=fibb.size()-1; i>=0; i--){
        if(n>=fibb[i]){//Checking if the value of n is greater than each entry of the vector, starting from the greatest number and working down
            answer.append("1");//If greater or equal add a 1 to the string
            n=n-fibb[i];//Subtract the value of the vector it was larger or = to
        }
        else if(answer!="")//Making sure to add the correct number of 0's at the beginning
            answer.append("0");
    }
    for(int i=answer.length()-1; i>=0; i--){
        temp="";
        temp=answer.at(i);
        temp2.append(temp);//This is reversing the string because I worked from higher to lower/right to left on the vector
    }
    temp2.append("1");//Adding the additional 1 to the end to signify the end of the code
    answer=temp2;//Having answer equal the reversed string 

    return answer;
}
int main(int argc, char *argv[])
{

    //-----------------All of the following code is from BB, will comment on my code-----------------//


     int sockfd, newsockfd, portno, clilen;
     char buffer[256];
     struct sockaddr_in serv_addr, cli_addr;
     int n,rec;
     if (argc < 2) {
         fprintf(stderr,"ERROR, no port provided\n");
         exit(1);
     }
     sockfd = socket(AF_INET, SOCK_STREAM, 0);
     if (sockfd < 0) 
        error("ERROR opening socket");
     bzero((char *) &serv_addr, sizeof(serv_addr));
     portno = atoi(argv[1]);
     serv_addr.sin_family = AF_INET;
     serv_addr.sin_addr.s_addr = INADDR_ANY;
     serv_addr.sin_port = htons(portno);
     if (bind(sockfd, (struct sockaddr *) &serv_addr,
              sizeof(serv_addr)) < 0) 
              error("ERROR on binding");
     listen(sockfd,5);
     clilen = sizeof(cli_addr);
     //----------Fireman() is from BB, cleans up all zombie processes that haven't been killed properly----------//          
    
     signal(SIGCHLD, fireman); 
     while (true){//Need to maintain a loop because there are gonna be multiple connections to the server, no while loop means only one connection will be made
        newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, (socklen_t *)&clilen);
        if(fork()==0){
            if (newsockfd < 0) 
                 error("ERROR on accept");
            bzero(buffer,256);
            n = read(newsockfd,&rec,sizeof(rec));//Reading the info from the client and storing it into rec, which is the priority number that is being sent over
            if (n < 0) error("ERROR reading from socket");
    
            string t= fibDecode(rec);//Calling my fibDecode function with the priority number that was sent from client
            n = write(newsockfd,t.c_str(),sizeof(t)+1);//Sending answer back to the client .c_str() was given to me by Prof. Rincon, I think it converts to dynamic char from a string type
            if (n < 0) error("ERROR writing to socket");
    
            sleep(5);
            _exit(0);
        }
         
    }
   return 0;
}