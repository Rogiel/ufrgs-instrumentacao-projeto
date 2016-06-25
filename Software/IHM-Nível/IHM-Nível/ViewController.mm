//
//  ViewController.m
//  IHM-Nível
//
//  Created by Rogiel Sulzbach on 6/21/16.
//  Copyright © 2016 Rogiel Sulzbach. All rights reserved.
//

#import "ViewController.h"
#import "AppDelegate.h"

#include <iostream>

@implementation ViewController

- (void)viewDidLoad {
    [super viewDidLoad];
    
    [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(handleNotification:) name:@"SensorValue" object:nil];

    // Do any additional setup after loading the view.
}

-(void)handleNotification:(NSNotification*)notification {
    NSNumber* number = [notification.userInfo objectForKey:@"Value"];
    SerialPayload* payloadPtr = (SerialPayload*) number.longValue;
    SerialPayload payload(*payloadPtr);
    
    dispatch_async(dispatch_get_main_queue(), ^{
        NSNumberFormatter* formatter = [[NSNumberFormatter alloc] init];
        formatter.numberStyle = NSNumberFormatterDecimalStyle;
        
        self.textField.stringValue = [formatter stringFromNumber:[NSNumber numberWithDouble:payload.level]];
        self.levelIndicator.doubleValue = payload.level / 120.0 * 50.0;
        
        self.levelField.stringValue = [NSString stringWithFormat:@"%@ Hz - %@ ciclos",
                                       [formatter stringFromNumber:[NSNumber numberWithDouble:payload.frequency]],
                                       [formatter stringFromNumber:[NSNumber numberWithDouble:16000000.0/payload.frequency]], nil];
        self.referenceField.stringValue = [NSString stringWithFormat:@"%@ Hz - %@ ciclos",
                                       [formatter stringFromNumber:[NSNumber numberWithDouble:payload.referenceFrequency]],
                                       [formatter stringFromNumber:[NSNumber numberWithDouble:16000000.0/payload.referenceFrequency]], nil];
        self.environmentField.stringValue = [NSString stringWithFormat:@"%@ Hz - %@ ciclos",
                                       [formatter stringFromNumber:[NSNumber numberWithDouble:payload.environmentFrequency]],
                                       [formatter stringFromNumber:[NSNumber numberWithDouble:16000000.0/payload.environmentFrequency]], nil];
        
    });
}

- (void)setRepresentedObject:(id)representedObject {
    [super setRepresentedObject:representedObject];

    // Update the view, if already loaded.
}
- (IBAction)changedReference:(id)sender {
    AppDelegate* delegate = [NSApplication sharedApplication].delegate;
    
    NSNumberFormatter* formatter = [[NSNumberFormatter alloc] init];
    formatter.numberStyle = NSNumberFormatterDecimalStyle;

    double v = [[formatter numberFromString:self.reference.stringValue] doubleValue];
    
    if(delegate.queue != nil) {
        delegate.queue->setReference(v);
    }
}

- (IBAction)calibrateAction:(id)sender {
    AppDelegate* delegate = [NSApplication sharedApplication].delegate;
    if(delegate.queue != nil) {
        delegate.queue->calibrate();
    }
}
@end
