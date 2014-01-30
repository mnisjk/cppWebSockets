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

int main(int argc, char **argv)
{
    EchoServer es = EchoServer( 8080 );
    es.run( );
}

EchoServer::EchoServer( int port ) : WebSocketServer( port )
{
    log( "Initializing echo server" );
}

EchoServer::~EchoServer( )
{
    log( "Deconstructing server" );
}


void EchoServer::onConnect( int socketID )
{
    log( "New connection" );
}

void EchoServer::onMessage( int socketID, const string& data )
{
    // Reply back with the same message
    log( "Received: " + data );
    this->send( socketID, data );
}

void EchoServer::onDisconnect( int socketID )
{
    log( "Disconnect" );
}

void EchoServer::onError( int socketID, const string& message )
{
    log( "Error: " + message );
}


