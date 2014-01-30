// Util.h
#ifndef _UTIL_H
#define _UTIL_H

#include <string>
#include <syslog.h>
#include <iostream>
#include <sstream>
/*
#include <stdint.h>
#include <map>
#include <list>
#include <syslog.h>
#include <ctime>
#include "../lib/libwebsockets.h"
*/
using namespace std;

/*
// Move to utils class if more utility functions are needed
template<typename T>
inline string toString(T t) { stringstream s; s << t; return s.str(); }
*/

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

