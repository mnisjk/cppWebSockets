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

#include "libwebsockets.h"
#include "Util.h"
#include "WebSocketServer.h"
#include <cstdlib>
#include <cstring>
#include <exception>
#include <fcntl.h>
#include <string>
#include <sys/time.h>
#include <vector>

// 0 for unlimited
constexpr int MAX_BUFFER_SIZE=0;

// Nasty hack because certain callbacks are statically defined
static WebSocketServer *self;

static int callback_main(   struct lws *wsi,
                            enum lws_callback_reasons reason,
                            void * /* user */,
                            void *in,
                            size_t len )
{
    int fd = -1;

    switch( reason ) {
        case LWS_CALLBACK_ESTABLISHED:
            self->onConnectWrapper( lws_get_socket_fd( wsi ) );
            lws_callback_on_writable( wsi );
            break;

        case LWS_CALLBACK_SERVER_WRITEABLE:
            fd = lws_get_socket_fd( wsi );
            while( !self->connections()[fd]->buffer.empty( ) )
            {
                std::string message = self->connections()[fd]->buffer.front( );
                size_t msgLen = message.size();

                std::vector<unsigned char> buf;
                buf.resize(LWS_SEND_BUFFER_PRE_PADDING + msgLen + LWS_SEND_BUFFER_POST_PADDING);
                unsigned char *ptr = &buf[LWS_SEND_BUFFER_PRE_PADDING];
                std::memcpy(ptr, message.c_str(), message.size());
                auto charsSent = size_t(lws_write( wsi, ptr, msgLen, LWS_WRITE_TEXT ));
                if( charsSent < msgLen )
                {
                    self->onErrorWrapper( fd, std::string( "Error writing to socket" ) );
                }
                else
                {
                    // Only pop the message if it was sent successfully.
                    self->connections()[fd]->buffer.pop_front();
                }
            }
            lws_callback_on_writable( wsi );
            break;

        case LWS_CALLBACK_RECEIVE:
            self->onMessage( lws_get_socket_fd( wsi ), std::string( reinterpret_cast<const char *>(in), len ) );
            break;

        case LWS_CALLBACK_CLOSED:
            self->onDisconnectWrapper( lws_get_socket_fd( wsi ) );
            break;

        default:
            break;
    }
    return 0;
}

WebSocketServer::WebSocketServer( int port, const std::string &certPath, const std::string& keyPath ):
    _port(port),
    _keyPath(keyPath),
    _certPath(certPath)
{
    struct lws_protocols protocols[] =
    {
        {
            "/",
            callback_main,
            0, // user data struct not used
            MAX_BUFFER_SIZE,
            0,
            nullptr,
            0
        },
        { nullptr, nullptr, 0, 0, 0, nullptr, 0
        } // terminator
    };

    lws_set_log_level( 0, lwsl_emit_syslog ); // We'll do our own logging, thank you.
    struct lws_context_creation_info info = {};
    info.port = this->_port;
    info.iface = nullptr;
    info.protocols = &protocols[0];
#ifndef LWS_NO_EXTENSIONS
    info.extensions = lws_get_internal_extensions( );
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
        info.ssl_cert_filepath        = nullptr;
        info.ssl_private_key_filepath = nullptr;
    }
    info.gid = gid_t(-1);
    info.uid = gid_t(-1);
    info.options = 0;

    // keep alive
    info.ka_time = 60; // 60 seconds until connection is suspicious
    info.ka_probes = 10; // 10 probes after ^ time
    info.ka_interval = 10; // 10s interval for sending probes
    _context = lws_create_context( &info );
    if(_context == nullptr)
    {
        throw std::invalid_argument("libwebsocket init failed");
    }
    Util::log( "Server started on port " + Util::toString( _port ) );

    // Some of the libwebsocket stuff is define statically outside the class. This
    // allows us to call instance variables from the outside.  Unfortunately this
    // means some attributes must be public that otherwise would be private.
    self = this;
}

WebSocketServer::~WebSocketServer( )
{
    // Free up some memory
    for( auto it = this->connections().begin( ); it != this->connections().end( ); ++it )
    {
        Connection* c = it->second;
        this->connections().erase( it->first );
        delete c;
    }
}

void WebSocketServer::onConnectWrapper( int socketID )
{
    auto *c = new Connection;
    c->createTime = time( nullptr );
    this->connections()[ socketID ] = c;
    this->onConnect( socketID );
}

void WebSocketServer::onDisconnectWrapper( int socketID )
{
    this->onDisconnect( socketID );
    this->_removeConnection( socketID );
}

void WebSocketServer::onErrorWrapper( int socketID, const std::string& message )
{
    Util::log( "Error: " + message + " on socketID '" + Util::toString( socketID ) + "'" );
    this->onError( socketID, message );
    this->_removeConnection( socketID );
}

void WebSocketServer::send( int socketID, const std::string &data )
{
    // Push this onto the buffer. It will be written out when the socket is writable.
    connections()[socketID]->buffer.emplace_back( data.c_str() );
}

void WebSocketServer::broadcast(const std::string &data )
{
    for( auto it = this->connections().begin( ); it != this->connections().end( ); ++it )
    {
        send( it->first, data );
    }
}

void WebSocketServer::setValue( int socketID, const std::string& name, const std::string& value )
{
    this->connections()[socketID]->keyValueMap[name] = value;
}

std::string WebSocketServer::getValue( int socketID, const std::string& name ) const
{
    auto it = _connections.find(socketID);
    if (it != _connections.end())
    {
        return it->second->keyValueMap[name];
    }
    return std::string();
}
int WebSocketServer::getNumberOfConnections( ) const
{
    return int(_connections.size( ));
}

void WebSocketServer::run( int timeout )
{
    while( true )
    {
        wait( timeout );
    }
}

void WebSocketServer::wait( int timeout )
{
    if( lws_service( this->_context, int(timeout) ) < 0 )
    {
        throw std::out_of_range("Error polling for socket activity.");
    }
}

void WebSocketServer::_removeConnection( int socketID )
{
    Connection* c = _connections[ socketID ];
    _connections.erase( socketID );
    delete c;
}
