#include <iostream>
#include <vector>
#include <fstream>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 

using namespace std;

struct fib{
    char ch;
    int freq=0;
    int priority;
    int size=0;
    string code="";
    char* hostname;
    int portNumber;
};
void error(const char *msg)//got all this from BB, I added the "cosnt" to get rid of some errors on complier 
{
    perror(msg);
    exit(1);
}
void *sender(void *ptr){
    fib *p= (fib *)ptr;
    int sockfd, portno, n;
    struct sockaddr_in serv_addr;
    struct hostent *server;
    char buffer[256];

    //-----------------All of the following code is from BB, will comment on my code-----------------//

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) 
        error("ERROR opening socket");

    server = gethostbyname(p->hostname);//Passing the hostname from the struct
    if (server == NULL) {
        fprintf(stderr,"ERROR, no such hostname\n");
        exit(0);
    }
    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr, 
         (char *)&serv_addr.sin_addr.s_addr,
         server->h_length);
    serv_addr.sin_port = htons(p->portNumber);//Passing portNumber from struct

    if (connect(sockfd,(struct sockaddr *)&serv_addr,sizeof(serv_addr)) < 0) 
        error("ERROR connecting");
    bzero(buffer,256);
    fgets(buffer,255,stdin);
    n = write(sockfd,&p->priority,sizeof(p->priority));//Passing the priority number to the server to get fib code
    if (n < 0) 
         error("ERROR writing to socket");
    bzero(buffer,256);
    n = read(sockfd,buffer,255);//Getting the response from server which is stored in buffer

    p->code=buffer;//Storing the code from buffer into my struct code
    if (n < 0) 
         error("ERROR reading from socket");
    close(sockfd);//Closing socket after use

    return NULL;
}
void sorter(vector<fib> &vec, string method){
    for(int i=0; i<vec.size()-1; i++){
        for(int j=0; j<vec.size(); j++){
            if(method=="Ascii"){//Checking which method is being used to sort
                if(int(vec[i].ch)>int(vec[i+1].ch)){//Checking if the first Ascii is greater than the next one if so, swap them
                   char t1;//Temp char place holder to swap later
                   int t2;//Temp int place holder to swap later
                   t1=vec[i].ch;//Setting first value to temp var
                   t2=vec[i].freq;//Setting first value to temp var
                   vec[i].ch=vec[i+1].ch;//Setting first value to next value in vector
                   vec[i].freq=vec[i+1].freq;//Setting first value to next value in vector
                   vec[i+1].ch=t1;//Setting second value to the temp var
                   vec[i+1].freq=t2;//Setting second value to the temp var
                   i--;//Since there is a swap we ahve to check make sure the first value isn't greater than the second one, only a case when looking at the middle/end of the vector
                }
            }
            else if(method=="Frequency"){
                if(vec[i].freq>vec[i+1].freq){
                    char t1;//Temp char place holder to swap later
                    int t2;//Temp int place holder to swap later
                    t1=vec[i].ch;//Setting first value to temp var
                    t2=vec[i].freq;//Setting first value to temp var
                    vec[i].ch=vec[i+1].ch;//Setting first value to next value in vector
                    vec[i].freq=vec[i+1].freq;//Setting first value to next value in vector
                    vec[i+1].ch=t1;//Setting second value to the temp var
                    vec[i+1].freq=t2;//Setting second value to the temp var
                    i--;//Since there is a swap we ahve to check make sure the first value isn't greater than the second one, only a case when looking at the middle/end of the vector
                }
            }
        }
    }
    
}
int main(int argc, char *argv[]){
    int numInputs, frequency;
    char letter;
    string compFile, temp, decompressedMessage, compressed;
    cin>>numInputs;//Grabbing initial line of input
    cin.ignore();//Ignoring newline after the initial int

    pthread_t tid[numInputs];
    vector<fib> symbols(numInputs);//Creating Vector of Structs(fib)

    /*Inputting to Vector*/for(int i=0; i<numInputs; i++){
        getline(cin, temp);//Grabbing entire line which includes char, frequency and storing into string to manipulate later
        if(temp[0]==' '){//Checking if the 'Char" at the beginning is a space
            symbols[i].ch=' ';//Setting the char to space
            symbols[i].freq=(int)temp[2]-48;//Taking Ascii value and -48 to get the true integer
        }
        else{
            symbols[i].ch=temp[0];//Not a space so just add it normally
            symbols[i].freq=(int)temp[2]-48;//Taking Ascii value and -48 to get the true integer
        } 
        symbols[i].size=numInputs;//Setting size for all structs to access later
        symbols[i].portNumber=atoi(argv[2]);//Setting portnumber for all structs to access inside child threads
        symbols[i].hostname=argv[1];//Setting hostname for all structs to access inside child threase
    }
    
    getline(cin, compFile);//Grabbing compressed file
    sorter(symbols, "Ascii");//Sorting Vector by Acsii, incase of ties later will already be in correct Ascii order
    sorter(symbols, "Frequency");//Sorting Vector by Frequency 

    for(int i=numInputs; i>0; i--)
        symbols[i-1].priority=numInputs-i+1;//Setting priority from 1 to numInputs
    for(int i=0; i<numInputs; i++)
        pthread_create(&tid[i], NULL, sender, &symbols[i]);//Creating child threads
    for(int i=0; i<numInputs; i++)
        pthread_join(tid[i],NULL);//Joining child threads back to main threads
    
    ifstream myfile(compFile);//Opening compressed file
    getline(myfile, compressed);//Storing contents of compressed file to string 
    myfile.close();//Closing since I don't need anything else from file

    while(compressed!=""){//Running loop until the compressed word is empty, meaning it's fully decoded
    int endLocation=compressed.find("11");//Finding location of '11' because all fib codes end with 11
    
    string fibCode=compressed.substr(0, endLocation+2);//Taking the beginning of the compressed message and going to the location of 11 and adding 2 because substr isn't inclusive
    for(int i=0;i<numInputs; i++){
        if(fibCode==symbols[i].code)//Checking if the code from the compressed message matches any of the codes from the
            decompressedMessage+=(symbols[i].ch);//Adding the char of the matching code to the decompressedMessage string to return later
    }
    fibCode="";//Resetting fibCode back to empty to avoid an overwriting when getting a new code from the compressed message
    compressed=compressed.substr(endLocation+2,compressed.size());//Deleteing everything until the frist location of '11' because it is no longer neeeded
    }
    
    cout<<"Decompressed message = "<<decompressedMessage<<endl;//Outputting the decompressedMessage
    return 0;  
}