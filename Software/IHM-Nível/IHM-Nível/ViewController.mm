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
    dispatch_async(dispatch_get_main_queue(), ^{
        NSNumberFormatter* formatter = [[NSNumberFormatter alloc] init];
        formatter.numberStyle = NSNumberFormatterDecimalStyle;
        self.textField.stringValue = [formatter stringFromNumber:number];
        self.levelIndicator.doubleValue = [number doubleValue];
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

@end
