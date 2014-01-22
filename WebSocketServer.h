// WebSocketServer.h
#ifndef _WEBSOCKETSERVER_H
#define _WEBSOCKETSERVER_H
#include <map>
#include <string>
#include <list>

using namespace std;

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
/// WebSocketServer
/// ---------
class WebSocketServer
{
public:
    // Constructor / Destructor
    WebSocketServer( int port );
    ~WebSocketServer( );
    void run( );
    void send( int socketID, string data );
    
    // Handles writing out to sockets
    map<int, list<string> > buffers;

    // Overriden by children
    virtual void onConnect( int socketID ) = 0;
    virtual void onMessage( int socketID, string data ) = 0;
protected:
    int port;    

private:
};

// WebSocketServer.h
#endif
