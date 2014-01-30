/** --------------------------------------------------------------------------
 *  Util.cpp
 *
 *  A few very basic utility functions for WebSocketServer
 *
 *  Author    : Jason Kruse <jason@jasonkruse.com> or @mnisjk
 *  Copyright : 2014
 *  License   : BSD (see LICENSE)
 *  -------------------------------------------------------------------------- 
 **/

#include <stdio.h>
#include "Util.h"

using namespace std;

#define LOG_PREFIX      "[websocket++] "

void Util::log( const string& message )
{
    //**TODO: Update to be used defined stdout
    const string& logMessage = LOG_PREFIX + message;
    if( 1 )
        printf( "%s\n", logMessage.c_str( ) ); // << endl;
    
    //**TODO: Trim this message?
    syslog( LOG_WARNING, "%s", logMessage.c_str( ) );
}

void Util::log( const char* message )
{
    log( string( message ) );
}

