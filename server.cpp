//#include "../lib/libwebsockets.h"
#include "../lib/libwebsockets.h"
#include "WebSocketServer.h"
#include "server.h"

Server::Server( int port ) : WebSocketServer( port )
{
    lwsl_notice( "Child constructor" );
}

Server::~Server( )
{

}

void Server::onConnect( int socketID )
{
    lwsl_notice( "ON CONNECT!!!" );
}

void Server::onMessage( int socketID, const string& data )
{
    lwsl_notice( "ON Message!!!\n" );
    lwsl_notice( data.c_str( ) );
    send( socketID, "Blah\n" );
}

void Server::onError( int socketID, const string& message )
{
    lwsl_notice( "ON Errror!!!\n" );
    lwsl_notice( message.c_str( ) );
}

