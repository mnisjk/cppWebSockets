#include "../../WebSocketServer.h"

using namespace std;

// For any real project this should be defined separately in a header file
class ChatServer : public WebSocketServer
{
public: 
    ChatServer( int port );
    ~ChatServer( );
    virtual void onConnect(    int socketID                        );
    virtual void onMessage(    int socketID, const string& data    );
    virtual void onDisconnect( int socketID                        );
    virtual void   onError(    int socketID, const string& message );
};

int main(int argc, char **argv)
{
    ChatServer cs = ChatServer( 8080 );
    cs.run( );
}

ChatServer::ChatServer( int port ) : WebSocketServer( port )
{
    log( "Initializing chat server" );
}

ChatServer::~ChatServer( )
{
    log( "Deconstructing server" );
}


void ChatServer::onConnect( int socketID )
{
    log( "New connection" );
}

void ChatServer::onMessage( int socketID, const string& data )
{
    // Reply back with the same message
    log( "Received: " + data );
    for( map<int,Connection*>::const_iterator it = this->connections.begin( ); it != this->connections.end( ); ++it )
        this->send( it->first, data );
}

void ChatServer::onDisconnect( int socketID )
{
    log( "Disconnect" );

}

void ChatServer::onError( int socketID, const string& message )
{
    log( "Error: " + message );
}

