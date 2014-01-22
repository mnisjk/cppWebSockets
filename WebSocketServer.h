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
    
    map< int, list<string> > buffers;

    // Overriden by children
    virtual void onConnect( int socketID ) = 0;
    virtual void onMessage( int socketID, string data ) = 0;
    void send     ( int socketID, string data );
protected:
    int port;    

private:
    // Attributes
};

// WebSocketServer.h
#endif
