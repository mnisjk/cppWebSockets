#include <stdio.h>
#include <string.h>
#include <sys/time.h>
#include "../lib/libwebsockets.h"
#include "WebSocketServer.h"

using namespace std;

WebSocketServer *self;

// **TODO: Should consider using templating or child classing

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
            self->onConnect( libwebsocket_get_socket_fd( wsi ) );
            break;

        case LWS_CALLBACK_SERVER_WRITEABLE:
            fd = libwebsocket_get_socket_fd( wsi );
            while( !self->buffers[fd].empty( ) )
            {
                string message = self->buffers[fd].front( ); 
                n = sprintf( (char *)p, "%s", message.c_str( ) );
                m = libwebsocket_write( wsi, p, n, LWS_WRITE_TEXT );
                if( m < n ) 
                    self->onError( fd, "Error writing to socket" );
                else
                    // Only pop the message if it was sent successfully.
                    self->buffers[fd].pop_front( ); 
            }
            break;

        case LWS_CALLBACK_RECEIVE:
            self->onMessage( libwebsocket_get_socket_fd( wsi ), string( (const char *)in ) );
            break;
        
        default:
            break;
	}
	return 0;
}


// **TODO: set these up via class calls
static struct libwebsocket_protocols protocols[] = {
	{
		"test",
		callback_test,
		0, // user data struct not used
		10,
	},{ NULL, NULL, 0, 0 } // terminator
};

void WebSocketServer::send( int socketID, string data )
{
    lwsl_notice( "Send!!\n" );
    // Push this onto the buffer. It will be written out when the socket is writable.
    buffers[socketID].push_back( data );
}


WebSocketServer::WebSocketServer( int port )
{
	this->port = port;
    lwsl_notice("libwebsockets test server - "
			"(C) Copyright 2010-2013 Andy Green <andy@warmcat.com> - "
						    "licensed under LGPL2.1\n");
   
    // Some of the libwebsocket stuff is define statically outside the class. This 
    // allows us to call instance variables from the outside.  Unfortunately this
    // means some attributes must be public that otherwise would be private. 
    self = this;
}

WebSocketServer::~WebSocketServer( )
{

}

void WebSocketServer::run( )
{
    lwsl_notice( "Run\n" ); 
	
    //**TODO update these to more c++ things/have a config
    //**TODO take in options via command line
    const char *iface = NULL;
	int use_ssl = 0;
    char *resource_path = "blahblah";
	char cert_path[1024];
	char key_path[1024];
	int opts = 0;
	unsigned int oldus = 0;
	
	struct lws_context_creation_info info;
	memset(&info, 0, sizeof info);
	info.port = 7681;
	info.iface = iface;
	info.protocols = protocols;
#ifndef LWS_NO_EXTENSIONS
	info.extensions = libwebsocket_get_internal_extensions();
#endif
	// **TODO: update style
    if (!use_ssl) {
		info.ssl_cert_filepath = NULL;
		info.ssl_private_key_filepath = NULL;
	} else {
		if (strlen(resource_path) > sizeof(cert_path) - 32) {
			throw "resource path too long";
		}
		sprintf(cert_path, "%s/libwebsockets-test-server.pem",
								resource_path);
		if (strlen(resource_path) > sizeof(key_path) - 32) {
			throw "resource path too long";
		}
		sprintf(key_path, "%s/libwebsockets-test-server.key.pem",
								resource_path);

		info.ssl_cert_filepath = cert_path;
		info.ssl_private_key_filepath = key_path;
	}
	info.gid = -1;
	info.uid = -1;
	info.options = opts;
 
    struct libwebsocket_context *context;
    context = libwebsocket_create_context( &info );
	if( !context )
		throw "libwebsocket init failed";
    
    // Event loop
    while( 1 )
    {
        struct timeval tv;
		gettimeofday(&tv, NULL);

		// This provokes the LWS_CALLBACK_SERVER_WRITEABLE for every
		// as soon as it can take more packets (usually immediately)
		if( ( (unsigned int)tv.tv_usec - oldus ) > 50000 ) {
			libwebsocket_callback_on_writable_all_protocol( &protocols[0] );
			oldus = tv.tv_usec;
		}

        if( libwebsocket_service( context, 50 ) < 0 )
            throw "Error polling for socket activity.";
    }
}

