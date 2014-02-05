/** --------------------------------------------------------------------------
 *  echoServer.cpp
 *
 *  A basic example of how to implement a WebSocketServer.  Creats a WS server
 *  bound to port 8080.  It listens for connections, and when a message is 
 *  received, it echos the same message back to the client.
 *
 *  Author    : Jason Kruse <jason@jasonkruse.com> or @mnisjk
 *  Copyright : 2014
 *  License   : BSD (see LICENSE)
 *  -------------------------------------------------------------------------- 
 **/

// Log to stdout for easy debugging.
#define LOG_TO_STDOUT 1

#include "../../Util.h"
#include "../../WebSocketServer.h"

using namespace std;

// For any real project this should be defined separately in a header file
class EchoServer : public WebSocketServer
{
public: 
    EchoServer( int port );
    ~EchoServer( );
    virtual void onConnect(    int socketID                        );
    virtual void onMessage(    int socketID, const string& data    );
    virtual void onDisconnect( int socketID                        );
    virtual void   onError(    int socketID, const string& message );
};

int main( int argc, char **argv )
{
    EchoServer es = EchoServer( 8080 );
    es.run( );
}

EchoServer::EchoServer( int port ) : WebSocketServer( port )
{
}

EchoServer::~EchoServer( )
{
}


void EchoServer::onConnect( int socketID )
{
    Util::log( "New connection" );
}

void EchoServer::onMessage( int socketID, const string& data )
{
    // Reply back with the same message
    Util::log( "Received: " + data );
    this->send( socketID, data );
}

void EchoServer::onDisconnect( int socketID )
{
    Util::log( "Disconnect" );
}

void EchoServer::onError( int socketID, const string& message )
{
    Util::log( "Error: " + message );
}


