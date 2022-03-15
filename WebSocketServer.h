/** --------------------------------------------------------------------------
 *  WebSocketServer.h
 *
 *  Base class that WebSocket implementations must inherit from.  Handles the
 *  client connections and calls the child class callbacks for connection
 *  events like onConnect, onMessage, and onDisconnect.
 *
 *  Author    : Jason Kruse <jason@jasonkruse.com> or @mnisjk
 *  Copyright : 2014
 *  License   : BSD (see LICENSE)
 *  --------------------------------------------------------------------------
 **/

#ifndef WEBSOCKETSERVER_H
#define WEBSOCKETSERVER_H

#include "libwebsockets.h"
#include <cstdint>
#include <cstdio>
#include <ctime>
#include <iostream>
#include <list>
#include <map>
#include <sstream>
#include <string>
#include <sys/time.h>

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
        std::list<std::string>       buffer;     // Ordered list of pending messages to flush out when socket is writable
        std::map<std::string, std::string> keyValueMap;
        time_t             createTime;
    };

    // Manages connections. Unfortunately this is public because static callback for
    // libwebsockets is defined outside the instance and needs access to it.
    std::map<int,Connection*> &connections() { return _connections; }

    // Constructor / Destructor
    explicit WebSocketServer( int port, const std::string &certPath = "", const std::string& keyPath = "" );
    virtual ~WebSocketServer( );

    void run(       int timeout = 50     );
    void wait(      int timeout = 50     );
    void send(      int socketID, const std::string &data );
    void broadcast( const std::string &data               );

    // Key => value storage for each connection
    std::string getValue( int socketID, const std::string& name ) const;
    void   setValue( int socketID, const std::string& name, const std::string& value );
    int    getNumberOfConnections( ) const;

    // Overridden by children
    virtual void onConnect(    int socketID                        ) = 0;
    virtual void onMessage(    int socketID, const std::string& data    ) = 0;
    virtual void onDisconnect( int socketID                        ) = 0;
    virtual void   onError(    int socketID, const std::string& message ) = 0;


    // Wrappers, so we can take care of some maintenance
    void onConnectWrapper(    int socketID );
    void onDisconnectWrapper( int socketID );
    void onErrorWrapper( int socketID, const std::string& message );

protected:
    // Nothing, yet.

private:
    int                  _port;
    std::string          _keyPath;
    std::string          _certPath;
    struct lws_context  *_context;
    std::map<int,Connection*> _connections;

    void _removeConnection( int socketID );
    // private copy constructor to keep it from being used (the libwebsockets C library needs special care)
    WebSocketServer(const WebSocketServer &rhs);
    WebSocketServer(const WebSocketServer &&rhs) noexcept;
    WebSocketServer& operator=(const WebSocketServer &rhs);
    WebSocketServer& operator=(const WebSocketServer &&rhs) noexcept;
};

// WebSocketServer.h
#endif
