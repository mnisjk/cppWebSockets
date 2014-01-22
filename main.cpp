#include <stdio.h> 
#include <iostream>
#include "../lib/libwebsockets.h"
#include "server.h"

using namespace std;

int main(int argc, char **argv)
{
    Server s = Server( 69 );
    s.run( );
}

