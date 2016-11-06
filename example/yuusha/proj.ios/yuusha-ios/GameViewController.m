//
//  GameViewController.m
//  yuusha-ios
//
//  Created by jukka on 16/7/30.
//  Copyright © 2016年 jukka. All rights reserved.
//

#import "GameViewController.h"
#import <OpenGLES/ES2/glext.h>

#include "seal.h"


@interface GameViewController () {

}
@property (strong, nonatomic) EAGLContext *context;
@property (strong, nonatomic) GLKBaseEffect *effect;

- (void)setupGL;
- (void)tearDownGL;

@end

@implementation GameViewController

- (void)viewDidLoad
{
    [super viewDidLoad];
    
    self.context = [[EAGLContext alloc] initWithAPI:kEAGLRenderingAPIOpenGLES2];

    if (!self.context) {
        NSLog(@"Failed to create ES context");
        return;
    }
    
    GLKView *view = (GLKView *)self.view;
    view.context = self.context;
    view.drawableDepthFormat = GLKViewDrawableDepthFormat24;

    [self setupGL];
}

- (void)dealloc
{    
    [self tearDownGL];
    
    if ([EAGLContext currentContext] == self.context) {
        [EAGLContext setCurrentContext:nil];
    }
}

- (void)didReceiveMemoryWarning
{
    [super didReceiveMemoryWarning];

    if ([self isViewLoaded] && ([[self view] window] == nil)) {
        self.view = nil;
        
        [self tearDownGL];
        
        if ([EAGLContext currentContext] == self.context) {
            [EAGLContext setCurrentContext:nil];
        }
        self.context = nil;
    }

    // Dispose of any resources that can be recreated.
}

- (BOOL)prefersStatusBarHidden {
    return YES;
}

- (void)setupGL
{
    [EAGLContext setCurrentContext:self.context];

    CGRect rect = [self.view bounds];
    seal_load_game_config();
    seal_init_graphics(rect.size.width, rect.size.height);
    seal_start_game();
}

- (void)tearDownGL
{
    [EAGLContext setCurrentContext:self.context];
    
    seal_destroy();
}

#pragma mark - GLKView and GLKViewController delegate methods

- (void)update
{
    // update
    seal_update();
}


- (void)glkView:(GLKView *)view drawInRect:(CGRect)rect
{
    seal_draw();
}

@end
