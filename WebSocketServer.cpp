#include <stdio.h>
#include <string.h>
#include <sys/time.h>
#include "../lib/libwebsockets.h"
#include "WebSocketServer.h"

//**TODO figure out if actually using any std functions
using namespace std;

// Nasty hack because certain callbacks are statically defined
WebSocketServer *self;

// **TODO: rename 
static int callback_test(   struct libwebsocket_context *context, 
                            struct libwebsocket *wsi, 
                            enum libwebsocket_callback_reasons reason, 
                            void *user, 
                            void *in, 
                            size_t len )
{
    int n, m, fd;
    unsigned char buf[LWS_SEND_BUFFER_PRE_PADDING + 512 + LWS_SEND_BUFFER_POST_PADDING];
    unsigned char *p = &buf[LWS_SEND_BUFFER_PRE_PADDING];
    
    switch (reason) {
        case LWS_CALLBACK_ESTABLISHED:
            self->onConnectWrapper( libwebsocket_get_socket_fd( wsi ) );
            break;

        case LWS_CALLBACK_SERVER_WRITEABLE:
            fd = libwebsocket_get_socket_fd( wsi );
            while( !self->connections[fd]->buffer.empty( ) )
            {
                string message = self->connections[fd]->buffer.front( ); 
                n = sprintf( (char *)p, "%s", message.c_str( ) );
                m = libwebsocket_write( wsi, p, n, LWS_WRITE_TEXT );
                if( m < n ) 
                    self->onError( fd, "Error writing to socket" );
                else
                    // Only pop the message if it was sent successfully.
                    self->connections[fd]->buffer.pop_front( ); 
            }
            break;
        
        case LWS_CALLBACK_RECEIVE:
            self->onMessage( libwebsocket_get_socket_fd( wsi ), string( (const char *)in ) );
            break;

        case LWS_CALLBACK_CLOSED:
            self->onDisconnect( libwebsocket_get_socket_fd( wsi ) );
            break;
        
        default:
            break;
    }
    return 0;
}


// **TODO: set these up via class calls, if possible
static struct libwebsocket_protocols protocols[] = {
    {
        "test",
        callback_test,
        0, // user data struct not used
        10,
    },{ NULL, NULL, 0, 0 } // terminator
};

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
    Connection* c = this->connections[ socketID ];
    this->connections.erase( socketID );
    delete c;
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

void WebSocketServer::log( const string& message )
{
    //**TODO: Update to be used defined stdout
    if( 1 )
        printf( "%s\n", message.c_str( ) ); // << endl;
    
    //**TODO: Trim this message?
    syslog( LOG_WARNING, "%s", message.c_str( ) );
}

void WebSocketServer::log( const char* message )
{
    log( string( message ) );
}

WebSocketServer::WebSocketServer( int port, const string certPath, const string& keyPath )
{
    this->_port     = port;
    this->_certPath = certPath;
    this->_keyPath  = keyPath; 

    // Some of the libwebsocket stuff is define statically outside the class. This 
    // allows us to call instance variables from the outside.  Unfortunately this
    // means some attributes must be public that otherwise would be private. 
    self = this;
}

WebSocketServer::~WebSocketServer( )
{
    //**TODO: free up the connection list.
}

void WebSocketServer::run( )
{
    
    //**TODO update these to more c++ things/have a config
    //**TODO take in options via command line
    //const char *iface = NULL;
    //int use_ssl = 0;
    char *resource_path = "blahblah";
    //char cert_path[1024];
    //char key_path[1024];
    int opts = 0;
    unsigned int oldus = 0;
    
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
        info.ssl_cert_filepath        = this->_certPath.c_str( );
        info.ssl_private_key_filepath = this->_keyPath.c_str( );
    } 
    else 
    {
        info.ssl_cert_filepath        = NULL;
        info.ssl_private_key_filepath = NULL;
    }
    info.gid = -1;
    info.uid = -1;
    info.options = opts;
 
    struct libwebsocket_context *context;
    context = libwebsocket_create_context( &info );
    if( !context )
        throw "libwebsocket init failed";
    log( "Web socket server started" ); 

    // Event loop
    while( 1 )
    {
        struct timeval tv;
        gettimeofday(&tv, NULL);
        if( ( (unsigned int)tv.tv_usec - oldus ) > 50000 ) 
        {
            libwebsocket_callback_on_writable_all_protocol( &protocols[0] );
            oldus = tv.tv_usec;
        }
        if( libwebsocket_service( context, 50 ) < 0 )
            throw "Error polling for socket activity.";
    }
}

void WebSocketServer::poll( uint64_t timeout )
{
    throw "Unimplemented"; 
}

