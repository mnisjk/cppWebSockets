// server.h
#ifndef _SERVER_H
#define _SERVER_H

#include "WebSocketServer.h"

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
/// WebSocketServer
/// ---------
class Server : public WebSocketServer
{
public: // External Auth
    // Constructor / Destructor
    Server( int port );
    ~Server( );
    virtual void onConnect( int socketID );
    virtual void onMessage( int socketID, string data );

private:
    // Attributes
};

// server.h
#endif

