PocketSocket
============

Objective-C websocket library for building things that work in realtime on iOS and OS X.

### Features

* Conforms fully to [RFC6455](http://tools.ietf.org/html/rfc6455) websocket protocol
* Support for websocket compression via the [permessage-deflate](http://tools.ietf.org/html/draft-ietf-hybi-permessage-compression-17) extension
* Passes all [Autobahn.ws](http://autobahn.ws) tests
* Client & Server modes (see notes below)
* TLS/SSL support
* Standalone `PSWebSocketDriver` for easy BYO networking IO

### Dependencies

* CFNetworking.framework
* Foundation.framework
* Security.framework
* CommonCrypto.framework (iOS)
* System.framework (OS X)
* libz.dylib

### Cocoapods Installation 

Add `pod 'PocketSocket'` to your Podfile and run `pod install`.

### Major Components

* **`PSWebSocketDriver`** - Networkless driver to deal with the websocket protocol. It solely operates with parsing raw bytes into events and sending events as raw bytes.
* **`PSWebSocket`** - Networking based socket around `NSInputStream` and `NSOutputStream` deals with ensuring a connection is maintained. USes the `PSWebSocketDriver` internally on the input and output. 

### Using PSWebSocket as a client

The client supports both the `ws` and secure `wss` protocols. It will automatically negotiate the certificates for you from the certificate chain on the device it’s running and support for pinned certificates is planned.

The client will always request the server turn on compression via the permessage-deflate extension. If the server accepts the request it will be enabled for the entire duration of the connection and used on all messages.

If the initial `NSURLRequest` specifies a timeout greater than 0 the connection will timeout if it cannot open within that interval, otherwise it could wait forever depending on the system.


```objc
#import <PSWebSocket/PSWebSocket.h>

@interface AppDelegate() <PSWebSocketDelegate>

@property (nonatomic, strong) PSWebSocket *socket;

@end
@implementation AppDelegate

- (BOOL)application:(UIApplication *)application didFinishLaunchingWithOptions:(NSDictionary *)launchOptions {
    self.window = [[UIWindow alloc] initWithFrame:[[UIScreen mainScreen] bounds]];
    self.window.backgroundColor = [UIColor whiteColor];
    [self.window makeKeyAndVisible];
    
    // create the NSURLRequest that will be sent as the handshake
    NSURLRequest *request = [NSURLRequest requestWithURL:[NSURL URLWithString:@"wss://example.com"]];
    
    // create the socket and assign delegate
    self.socket = [PSWebSocket clientSocketWithRequest:request];
    self.socket.delegate = self;
    
    // open socket
    [self.socket open];
    
    return YES;
}

#pragma mark - PSWebSocketDelegate

- (void)webSocketDidOpen:(PSWebSocket *)webSocket {
    NSLog(@"The websocket handshake completed and is now open!");
    [webSocket send:@"Hello world!"];
}
- (void)webSocket:(PSWebSocket *)webSocket didReceiveMessage:(id)message {
    NSLog(@"The websocket received a message: %@", message);
}
- (void)webSocket:(PSWebSocket *)webSocket didFailWithError:(NSError *)error {
    NSLog(@"The websocket handshake/connection failed with an error: %@", error);
}
- (void)webSocket:(PSWebSocket *)webSocket didCloseWithCode:(NSInteger)code reason:(NSString *)reason wasClean:(BOOL)wasClean {
    NSLog(@"The websocket closed with code: %@, reason: %@, wasClean: %@", @(code), reason, (wasClean) ? @"YES" : @"NO");
}

@end

```

### Using PSWebSocket with a server

Using PSWebSocket with a server is a bit trickier. Most HTTP servers parse incoming requests and detach incoming websocket requests to be managed independently of the server. PSWebSocket expects the server to setup an `NSInputStream` and `NSOutputStream` and have already read the incoming handshake request before creating a PSWebSocket instance to manage the connection.

##### Steps to take:

1. Formulate a `NSURLRequest` from the already read request headers, **any extra data past the headers that was read from the socket must be put into the HTTPBody of the NSURLRequest**
2. Pass in the already opened `NSInputStream` and `NSOutputStream` for the given socket connection
3. Unschedule the passed in input and output streams from any server run loop
4. Open the websocket

As a helper you can use `[PSWebSocket isWebSocketRequest:(NSURLRequest *)]` to check if the headers are suitable for a websocket connection.


### Using PSWebSocketDriver

The driver is the core of the websocket. It deals the handshake request/response lifecycle, packing messages into websocket frames to be sent over the wire and parsing websocket frames received over the wire.

It supports both client and server mode and has an identical API for each.

To create an instance of it you use either `clientDriverWithRequest:` or `serverDriverWithRequest:` in the client mode you are to pass in a `NSURLRequest` that will be sent as a handshake request. In server mode you are to pass in the `NSURLRequest` that was the handshake request. It is extremely important that the `HTTPBody` in server mode include any extra data past the end of the headers that was already read.

Beyond that have a look at the `PSWebSocketDriverDelegate` methods and the simple API for interacting with the driver.


### Authors

* Robert Payne

### License

TBD