#include "../../Util.h"
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
}

ChatServer::~ChatServer( )
{
}


void ChatServer::onConnect( int socketID )
{
    // Give this connection a random user ID
    const string& handle = "User #" + Util::toString( socketID );
    Util::log( "New connection: " + handle );
    
    // Associate this handle with the connection
    this->setValue( socketID, "handle", handle );

    // Let everyone know the new user has connected
    for( map<int,Connection*>::const_iterator it = this->connections.begin( ); it != this->connections.end( ); ++it )
        this->send( it->first, handle + " has connected." );
}

void ChatServer::onMessage( int socketID, const string& data )
{
    // Send the received message to all connected clients in the form of 'User XX: message...'
    Util::log( "Received: " + data );
    const string& message = this->getValue( socketID, "handle" ) + ": " + data;

    // Iterate over the connections and send the message
    for( map<int,Connection*>::const_iterator it = this->connections.begin( ); it != this->connections.end( ); ++it )
        this->send( it->first, message );
}

void ChatServer::onDisconnect( int socketID )
{
    const string& handle = this->getValue( socketID, "handle" );
    Util::log( "Disconnected: " + handle );
    
    // Let everyone know the user has disconnected
    const string& message = handle + " has disconnected.";
    for( map<int,Connection*>::const_iterator it = this->connections.begin( ); it != this->connections.end( ); ++it )
        if( it->first != socketID )
            // The disconnected connection gets deleted after this function runs, so don't try to send to it
            // (It's still around in case the implementing class wants to perform any clean up actions)
            this->send( it->first, message );
}

void ChatServer::onError( int socketID, const string& message )
{
    Util::log( "Error: " + message );
}

