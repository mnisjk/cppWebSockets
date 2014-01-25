websocket++
===========

A simple c++ wrapper around the popular libwebsockets c library.

### Usage

Create a class that extends `WebSocketServer` and implement the following callbacks

```
void onConnect( int socketID                        ); // New websocket connection
void onMessage( int socketID, const string& data    ); // Message received from connected client
void   onError( int socketID, const string& message ); // Networking error

```

Then simply instantiate your server and call `run()` 

```
MyServer s = MyServer( 8080 /* bind to port 8080 */ ); // MyServer extends WebSocketServer
s.run( );
```

At any arbitrary time, you can push a message to a client by calling `send( int socketID, string data )`.

And that's it.

### Examples

Check out the `examples` directory for fully implemented illustrations.  There is a basic echo and chat server.  If your server needs to monitor other connections (or any file descriptor for that matter), instead of using `run( )`, use an event loop with `poll( )`. An example doing this is **TODO: fill in a class. 


### Features

* Implement your own web socket server in **TODO: figure out num lines
* Abstracts away all c pointers and managing when sockets are writable
* Push to any client at any time
* Key --> Value storage for any socket with `setValue( int socketID, const string& name, const string& value );` and `getValue( int socketID, const string& name );`

### Dependencies

libwebsockets **TODO: how to install on ubuntu

### Compile and run

```
g++ -w -DINSTALL_DATADIR=/usr/share -omyserer WebSocketServer.cpp myserver.cpp main.cpp -lwebsockets
./myserver
```
