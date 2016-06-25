//
//  AppDelegate.m
//  IHM-Nível
//
//  Created by Rogiel Sulzbach on 6/21/16.
//  Copyright © 2016 Rogiel Sulzbach. All rights reserved.
//

#import "AppDelegate.h"

#include <thread>

class SerialProcessingQueueDelegateImpl: public SerialProcessingQueueDelegate {
    virtual void receivedData(SerialPayload payload) const {
        NSNotificationCenter* center = [NSNotificationCenter defaultCenter];
        
        NSDictionary* userInfo = @{
            @"Value": [NSNumber numberWithLong:(long)&payload]
        };
        
        [center postNotificationName:@"SensorValue" object:nil userInfo:userInfo];
    }
};

@interface AppDelegate ()

@end

@implementation AppDelegate

- (void)applicationDidFinishLaunching:(NSNotification *)aNotification {
    // Insert code here to initialize your application
    _queue = new SerialProcessingQueue(new SerialProcessingQueueDelegateImpl());
    
    _queue->setReference(35110);
    
    SerialProcessingQueue* queue = _queue;
    std::thread* t = new std::thread([queue]() {
        queue->run();
    });
}

- (void)applicationWillTerminate:(NSNotification *)aNotification {
    // Insert code here to tear down your application
}

@end
