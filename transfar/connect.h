#ifndef CONNECT_H
#define CONNECT_H
typedef struct Connect{
    Server *srv;
    Socket sock;
    char state;
    char type;
    
}Connect,ConnectPtr *;




#endif