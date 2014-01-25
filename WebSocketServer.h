// WebSocketServer.h
#ifndef _WEBSOCKETSERVER_H
#define _WEBSOCKETSERVER_H
#include <stdint.h>
#include <map>
#include <string>
#include <list>
#include <syslog.h>
#include "../lib/libwebsockets.h"

using namespace std;

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
/// WebSocketServer
/// ---------
class WebSocketServer
{
public:
    // Represents a client connection
    struct Connection
    {
        list<string>       buffer;     // Ordered list of pending messages to flush out when socket is writable
        map<string,string> keyValueMap;
    };

    // Manages connections. Unfortunately this is public because static callback for 
    // libwebsockets is defined outside the instance and needs access to it.
    map<int,Connection*> connections;
    int port;    

    // Constructor / Destructor
    WebSocketServer( int port );
    ~WebSocketServer( );
    void run( );
    void send( int socketID, string data );
    
    // Key => value storage for each connection
    string getValue( int socketID, const string& name );
    void   setValue( int socketID, const string& name, const string& value );

    // Overridden by children
    virtual void onConnect( int socketID                        ) = 0;
    virtual void onMessage( int socketID, const string& data    ) = 0;
    virtual void   onError( int socketID, const string& message ) = 0;

protected:
    void log( const string& message );
    void log( const char* message );

private:
};

// WebSocketServer.h
#endif
