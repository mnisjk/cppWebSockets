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

#include "Util.h"
#include <cstdio>

constexpr auto LOG_PREFIX = "[cppWebSockets] ";

void Util::log( const std::string& message )
{
    const std::string& logMessage = LOG_PREFIX + message;
    // NOLINTNEXTLINE(cppcoreguidelines-pro-type-vararg, hicpp-vararg)
    syslog( LOG_WARNING, "%s", logMessage.c_str( ) );

#ifdef LOG_TO_STDOUT
    std::cout << logMessage << std::endl;
#endif
}

void Util::log( const char* message )
{
    log( std::string( message ) );
}

