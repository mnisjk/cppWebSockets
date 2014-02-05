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

#define LOG_PREFIX      "[cppWebSockets] "

void Util::log( const string& message )
{
    const string& logMessage = LOG_PREFIX + message;
    syslog( LOG_WARNING, "%s", logMessage.c_str( ) );

#ifdef LOG_TO_STDOUT
        printf( "%s\n", logMessage.c_str( ) );
#endif 
}

void Util::log( const char* message )
{
    log( string( message ) );
}

