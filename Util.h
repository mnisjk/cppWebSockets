/** --------------------------------------------------------------------------
 *  Util.h
 *
 *  A few very basic utility functions for WebSocketServer
 *
 *  Author    : Jason Kruse <jason@jasonkruse.com> or @mnisjk
 *  Copyright : 2014
 *  License   : BSD (see LICENSE)
 *  --------------------------------------------------------------------------
 **/

#ifndef UTIL_H
#define UTIL_H

#include <iostream>
#include <sstream>
#include <string>
#include <syslog.h>

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
/// Util
/// ---------

class Util
{
public:
    static void   log( const std::string& message );
    static void   log( const char* message );
    template<typename T>
    static inline std::string toString(T t) { std::stringstream s; s << t; return s.str(); }
};

// Util.h
#endif

