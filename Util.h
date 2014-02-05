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

#ifndef _UTIL_H
#define _UTIL_H

#include <string>
#include <syslog.h>
#include <iostream>
#include <sstream>

using namespace std;

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
/// Util
/// ---------

class Util
{
public:
    static void   log( const string& message );
    static void   log( const char* message );
    template<typename T>
    static inline string toString(T t) { stringstream s; s << t; return s.str(); }
};

// Util.h
#endif

