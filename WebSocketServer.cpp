/** --------------------------------------------------------------------------
 *  WebSocketServer.cpp
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

#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <fcntl.h>
#include "../lib/libwebsockets.h"
#include "Util.h"
#include "WebSocketServer.h"

using namespace std;

// 0 for unlimited
#define MAX_BUFFER_SIZE 0

// Nasty hack because certain callbacks are statically defined
WebSocketServer *self;

static int callback_main(   struct libwebsocket_context *context, 
                            struct libwebsocket *wsi, 
                            enum libwebsocket_callback_reasons reason, 
                            void *user, 
                            void *in, 
                            size_t len )
{
    int fd;
    unsigned char buf[LWS_SEND_BUFFER_PRE_PADDING + 512 + LWS_SEND_BUFFER_POST_PADDING];
    unsigned char *p = &buf[LWS_SEND_BUFFER_PRE_PADDING];
    
    switch( reason ) {
        case LWS_CALLBACK_ESTABLISHED:
            self->onConnectWrapper( libwebsocket_get_socket_fd( wsi ) );
            libwebsocket_callback_on_writable( context, wsi );
            break;

        case LWS_CALLBACK_SERVER_WRITEABLE:
            fd = libwebsocket_get_socket_fd( wsi );
            while( !self->connections[fd]->buffer.empty( ) )
            {
                string message = self->connections[fd]->buffer.front( ); 
                int charsSent = libwebsocket_write( wsi, (unsigned char*)message.c_str( ), message.length( ), LWS_WRITE_TEXT );
                if( charsSent < message.length( ) ) 
                    self->onErrorWrapper( fd, string( "Error writing to socket" ) );
                else
                    // Only pop the message if it was sent successfully.
                    self->connections[fd]->buffer.pop_front( ); 
            }
            libwebsocket_callback_on_writable( context, wsi );
            break;
        
        case LWS_CALLBACK_RECEIVE:
            self->onMessage( libwebsocket_get_socket_fd( wsi ), string( (const char *)in ) );
            break;

        case LWS_CALLBACK_CLOSED:
            self->onDisconnectWrapper( libwebsocket_get_socket_fd( wsi ) );
            break;

        default:
            break;
    }
    return 0;
}

static struct libwebsocket_protocols protocols[] = {
    {
        "/",
        callback_main,
        0, // user data struct not used
        MAX_BUFFER_SIZE,
    },{ NULL, NULL, 0, 0 } // terminator
};

WebSocketServer::WebSocketServer( int port, const string certPath, const string& keyPath )
{
    this->_port     = port;
    this->_certPath = certPath;
    this->_keyPath  = keyPath; 

    lws_set_log_level( 0, lwsl_emit_syslog ); // We'll do our own logging, thank you.
    struct lws_context_creation_info info;
    memset( &info, 0, sizeof info );
    info.port = this->_port;
    info.iface = NULL;
    info.protocols = protocols;
#ifndef LWS_NO_EXTENSIONS
    info.extensions = libwebsocket_get_internal_extensions( );
#endif
    
    if( !this->_certPath.empty( ) && !this->_keyPath.empty( ) )
    {
        Util::log( "Using SSL certPath=" + this->_certPath + ". keyPath=" + this->_keyPath + "." );
        info.ssl_cert_filepath        = this->_certPath.c_str( );
        info.ssl_private_key_filepath = this->_keyPath.c_str( );
    } 
    else 
    {
        Util::log( "Not using SSL" );
        info.ssl_cert_filepath        = NULL;
        info.ssl_private_key_filepath = NULL;
    }
    info.gid = -1;
    info.uid = -1;
    info.options = 0;
    this->_context = libwebsocket_create_context( &info );
    if( !this->_context )
        throw "libwebsocket init failed";
    Util::log( "Server started on port " + Util::toString( this->_port ) ); 

    // Some of the libwebsocket stuff is define statically outside the class. This 
    // allows us to call instance variables from the outside.  Unfortunately this
    // means some attributes must be public that otherwise would be private. 
    self = this;
}

WebSocketServer::~WebSocketServer( )
{
    // Free up some memory
    for( map<int,Connection*>::const_iterator it = this->connections.begin( ); it != this->connections.end( ); ++it )
    {
        Connection* c = it->second;
        this->connections.erase( it->first );
        delete c;
    }
}

void WebSocketServer::onConnectWrapper( int socketID )
{
    Connection* c = new Connection;
    c->createTime = time( 0 );
    this->connections[ socketID ] = c;
    this->onConnect( socketID );
}

void WebSocketServer::onDisconnectWrapper( int socketID )
{
    this->onDisconnect( socketID );
    this->_removeConnection( socketID );
}

void WebSocketServer::onErrorWrapper( int socketID, const string& message )
{
    Util::log( "Error: " + message + " on socketID '" + Util::toString( socketID ) + "'" ); 
    this->onError( socketID, message );
    this->_removeConnection( socketID );
}

void WebSocketServer::send( int socketID, string data )
{
    // Push this onto the buffer. It will be written out when the socket is writable.
    this->connections[socketID]->buffer.push_back( data );
}

void WebSocketServer::setValue( int socketID, const string& name, const string& value )
{
    this->connections[socketID]->keyValueMap[name] = value;
}

string WebSocketServer::getValue( int socketID, const string& name )
{
    return this->connections[socketID]->keyValueMap[name];
}

void WebSocketServer::run( uint64_t timeout )
{
    // Event loop
    while( 1 )
    {
        this->wait( timeout );
    }
}

bool WebSocketServer::wait( uint64_t timeout )
{
    if( libwebsocket_service( this->_context, timeout ) < 0 )
        throw "Error polling for socket activity.";
}

void WebSocketServer::_removeConnection( int socketID )
{
    Connection* c = this->connections[ socketID ];
    this->connections.erase( socketID );
    delete c;
}


