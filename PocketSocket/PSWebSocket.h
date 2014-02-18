//
//  PSWebSocket.h
//  PocketSocket
//
//  Created by Robert Payne on 18/02/14.
//  Copyright (c) 2014 Zwopple Limited. All rights reserved.
//

#import <Foundation/Foundation.h>
#import "PSWebSocketErrorCodes.h"
#import "PSWebSocketStatusCodes.h"

typedef NS_ENUM(NSInteger, PSWebSocketReadyState) {
    PSWebSocketReadyStateConnecting = 0,
    PSWebSocketReadyStateOpen,
    PSWebSocketReadyStateClosing,
    PSWebSocketReadyStateClosed
};

@class PSWebSocket;

/**
 *  PSWebSocketDelegate
 */
@protocol PSWebSocketDelegate <NSObject>

@required
- (void)webSocketDidOpen:(PSWebSocket *)webSocket;
- (void)webSocket:(PSWebSocket *)webSocket didFailWithError:(NSError *)error;
- (void)webSocket:(PSWebSocket *)webSocket didReceiveMessage:(id)message;
- (void)webSocket:(PSWebSocket *)webSocket didCloseWithCode:(NSInteger)code reason:(NSString *)reason wasClean:(BOOL)wasClean;

@end

/**
 *  PSWebSocket
 */
@interface PSWebSocket : NSObject

#pragma mark - Properties

@property (nonatomic, assign, readonly) PSWebSocketReadyState readyState;
@property (nonatomic, weak) id <PSWebSocketDelegate> delegate;
@property (nonatomic, strong) dispatch_queue_t delegateQueue;

#pragma mark - Initialization

/**
 *  Initialize a PSWebSocket instance in client mode.
 *
 *  @param request that is to be used to initiate the handshake
 *
 *  @return an initialized instance of PSWebSocket in client mode
 */
+ (instancetype)clientSocketWithRequest:(NSURLRequest *)request;

#pragma mark - Actions

/**
 *  Opens the websocket connection and initiates the handshake. Once
 *  opened an instance of PSWebSocket can never be opened again. The
 *  connection obeys any timeout interval set on the NSURLRequest used
 *  to initialize the websocket.
 */
- (void)open;

/**
 *  Send a message over the websocket
 *
 *  @param message an instance of NSData or NSString to send
 */
- (void)send:(id)message;

/**
 *  Send a ping over the websocket
 *
 *  @param pingData data to include with the ping
 *  @param handler  optional callback handler when the corrosponding pong is received
 */
- (void)ping:(NSData *)pingData handler:(void (^)(NSData *pongData))handler;


/**
 *  Close the websocket will default to code 1000 and nil reason
 */
- (void)close;

/**
 *  Close the websocket with a specific code and/or reason
 *
 *  @param code   close code reason
 *  @param reason short textual reason why the connection was closed
 */
- (void)closeWithCode:(NSInteger)code reason:(NSString *)reason;

@end