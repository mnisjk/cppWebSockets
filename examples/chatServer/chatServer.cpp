#include "../../WebSocketServer.h"

using namespace std;

// For any real project this should be defined separately in a header file
class ChatServer : public WebSocketServer
{
public: 
    ChatServer( int port );
    ~ChatServer( );
    virtual void onConnect( int socketID                        );
    virtual void onMessage( int socketID, const string& data    );
    virtual void   onError( int socketID, const string& message );
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

void ChatServer::onError( int socketID, const string& message )
{
    log( "Error: " + message );
}

void ChatServer::onMessage( int socketID, const string& data )
{
    // Reply back with the same message
    log( "Received: " + data );
    //#define SFOREACHMAPCONST( _CT0_, _CT1_, _C_, _I_ ) for( map< _CT0_ , _CT1_ >::const_iterator _I_ = (_C_).begin( ); _I_ != (_C_).end( ); ++_I_ )
    for( map<int,Connection*>::const_iterator it = this->connections.begin( ); it != this->connections.end( ); ++it )
        this->send( it->first, data );
}

