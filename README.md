cppWebSockets
===========

A simple, lightweight c++ WebSockets server library wrapped around the popular libwebsockets c library. 

### Usage

Create a class that extends `WebSocketServer` and implement the following callbacks

```
void onConnect(    int socketID                        ); // New websocket connection
void onMessage(    int socketID, const string& data    ); // Message received from connected client
void onDisconnect( int socketID                        ); // Client disconnect
void   onError(    int socketID, const string& message ); // Networking error
```

Then simply instantiate your server and call `run()` 

```
MyServer s = MyServer( 8080 ); // MyServer extends WebSocketServer listening on port 8080
s.run( );
```

At any arbitrary time, you can push a message to a client by calling `send( int socketID, string data )`.

If your server is more complex and needs to monitor its own connections in addition to WebSocket connections, you can manage your own event loop. Instead of calling `s.run( )`, use the `s.wait( )` function.  A good illustration of this is located in [examples/multiPollServer/multiPollServer.cpp](/mnisjk/cppWebSockets/blob/master/examples/multiPollServer/multiPollServer.cpp)


### Examples

Check out the [examples](/mnisjk/cppWebSockets/blob/master/examples/) directory for fully implemented illustrations.  There is a basic echo and chat server as well as a more complex server that manages multiple `poll( )` loops. They should demonstrate how easy this library is to use and serve as basic scaffolding for your projects.


### Features

* Implement your own web socket server in less than 50 lines of c++.
* OpenSSL support
* WebSocket RFC6455 implementation
* Abstracts away all c pointers and managing when sockets are writable
* Push data to any client at any time
* Key => Value storage for any socket with `setValue( int socketID, const string& name, const string& value );` and `getValue( int socketID, const string& name );`


### Dependencies

This is built on top of [warmcat](http://warmcat.com/)'s wonderful, lightweight [libwebsocket](http://libwebsockets.org/) c library.  Follow the instructions on [his site](http://libwebsockets.org/) to install. On Linux, make sure you have the header file `/usr/local/include/libwebsockets.h` and the library files `/usr/local/lib/libwebsockets.so*`


### Compile and run

All examples have Makefiles, so simply run `make` and then run the example.  When creating your own projects, your compile commands will look like the following:

```
g++ -w -DLOG_TO_STDOUT=1 -omyserver Util.cpp WebSocketServer.cpp myserver.cpp -lwebsockets
./myserver
```
