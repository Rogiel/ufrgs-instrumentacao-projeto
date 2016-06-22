//
//  AppDelegate.h
//  IHM-Nível
//
//  Created by Rogiel Sulzbach on 6/21/16.
//  Copyright © 2016 Rogiel Sulzbach. All rights reserved.
//

#import <Cocoa/Cocoa.h>

#include "SerialProcessingQueue.h"

@interface AppDelegate : NSObject <NSApplicationDelegate>

@property (assign) SerialProcessingQueue* queue;


@end

