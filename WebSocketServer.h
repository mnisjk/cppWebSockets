/** --------------------------------------------------------------------------
 *  WebSocketServer.h
 *
 *  Base class that WebSocket implementations must inherit from.  Handles the
 *  client connections and calls the child class callbacks for connection
 *  events like onConnect, onMessage, and onDisconnect
 *
 *  Author    : Jason Kruse <jason@jasonkruse.com> or @mnisjk
 *  Copyright : 2014
 *  License   : BSD (see LICENSE)
 *  -------------------------------------------------------------------------- 
 **/

#ifndef _WEBSOCKETSERVER_H
#define _WEBSOCKETSERVER_H
#include <stdint.h>
#include <map>
#include <string>
#include <list>
#include <stdio.h>
#include <ctime>
#include <iostream>
#include <sstream>
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
        time_t             createTime;
    };

    // Manages connections. Unfortunately this is public because static callback for 
    // libwebsockets is defined outside the instance and needs access to it.
    map<int,Connection*> connections;

    // Constructor / Destructor
    WebSocketServer( int port, const string certPath = "", const string& keyPath = "" );
    ~WebSocketServer( );
    
    void run( );
    void poll( uint64_t timeout = 1000 );
    void send( int socketID, string data );
    
    // Key => value storage for each connection
    string getValue( int socketID, const string& name );
    void   setValue( int socketID, const string& name, const string& value );

    // Overridden by children
    virtual void onConnect(    int socketID                        ) = 0;
    virtual void onMessage(    int socketID, const string& data    ) = 0;
    virtual void onDisconnect( int socketID                        ) = 0;
    virtual void   onError(    int socketID, const string& message ) = 0;


    // Wrappers, so we can take care of some maintenance
    void onConnectWrapper(    int socketID );
    void onDisconnectWrapper( int socketID );

protected:
    
    void log( const string& message );
    void log( const char* message );

private:
    int    _port;    
    string _keyPath;
    string _certPath;
};

// WebSocketServer.h
#endif
