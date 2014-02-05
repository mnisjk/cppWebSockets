/** --------------------------------------------------------------------------
 *  multiPollServer.cpp
 *
 *  A basic example of how to implement a WebSocketServer *AND* listen to other
 *  fds. This is useful when your server needs to handle connections from 
 *  WebSocket clients and other TCP connections at the same time.
 *
 *  It also demonstrates how to send to an arbitrary WebSocket connection at
 *  any time.
 *
 *  This program listens for input on stdin and WebSockets on port 8080 
 *  simultaneously. When new data is available on stdin (line buffered),
 *  it sends that string out to all connected WebSocket clients.
 *
 *  Author    : Jason Kruse <jason@jasonkruse.com> or @mnisjk
 *  Copyright : 2014
 *  License   : BSD (see LICENSE)
 *  -------------------------------------------------------------------------- 
 **/

#include <string>
#include <fcntl.h>
#include "../../Util.h"
#include "../../WebSocketServer.h"

#define TIMEOUT    50
#define PROMPT     "message> "

using namespace std;

// For any real project this should be defined separately in a header file
class MultiPollServer : public WebSocketServer
{
public: 
    MultiPollServer( int port );
    ~MultiPollServer( );
    virtual void onConnect(    int socketID                        );
    virtual void onMessage(    int socketID, const string& data    );
    virtual void onDisconnect( int socketID                        );
    virtual void   onError(    int socketID, const string& message );
};


void showPrompt( )
{
    cout << PROMPT;
    fflush( stdout );
}

int main( int argc, char **argv )
{
    // Set up the stdin polling
    int p;
    char buf[1024];
    struct pollfd pfds[1];
    pfds[0].fd = 0;
    pfds[0].events = POLLIN;
    
    // Start the WebSocket server
    MultiPollServer s = MultiPollServer( 8080 );
    
    cout << "Type a message followed by enter to send to all connected clients." << endl << endl; 
    showPrompt( );
    while( 1 )
    {
        // Handle websocket stuff
        s.wait( TIMEOUT );

        // Handle stdin
        p = poll( pfds, 1, TIMEOUT );
        if( p > 0 && pfds[0].revents & POLLIN )
        {
            int len = read( 0, buf, 1024 );
            if( !len )
                return 0;
            buf[len] = 0; // make sure the c-string terminator is where we want it to be since this buf is re-used.
            string input = string( buf );
            input.resize( input.size( ) - 1 ); // Remove the trailing \n
            
            // Send it out to WS clients.
            Util::log( "Sending '" + input  + "' to " + Util::toString( s.getNumberOfConnections( ) ) + " client(s)." );
            s.broadcast( input ); 
            
            // Diplay the prompt.
            showPrompt( );
        }
    }
}

MultiPollServer::MultiPollServer( int port ) : WebSocketServer( port )
{
}

MultiPollServer::~MultiPollServer( )
{
}


void MultiPollServer::onConnect( int socketID )
{
    Util::log( "New connection" );
}

void MultiPollServer::onMessage( int socketID, const string& data )
{
    // Should never get hit.
}

void MultiPollServer::onDisconnect( int socketID )
{
    Util::log( "Disconnect" );
}

void MultiPollServer::onError( int socketID, const string& message )
{
    Util::log( "Error: " + message );
}


