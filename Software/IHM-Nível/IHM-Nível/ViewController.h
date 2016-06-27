//
//  ViewController.h
//  IHM-Nível
//
//  Created by Rogiel Sulzbach on 6/21/16.
//  Copyright © 2016 Rogiel Sulzbach. All rights reserved.
//

#import <Cocoa/Cocoa.h>

@interface ViewController : NSViewController

@property (weak) IBOutlet NSTextField *textField;
@property (weak) IBOutlet NSTextField *reference;
@property (weak) IBOutlet NSLevelIndicator *levelIndicator;

@property (weak) IBOutlet NSTextField *levelField;
@property (weak) IBOutlet NSTextField *referenceField;
@property (weak) IBOutlet NSTextField *environmentField;
@property (weak) IBOutlet NSTextField *uncertaintyField;

- (IBAction)calibrateAction:(id)sender;
@end

