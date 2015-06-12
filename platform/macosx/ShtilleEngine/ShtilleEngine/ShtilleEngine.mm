//
//  ShtilleEngine.m
//  ShtilleEngine
//
//  Created by Владимир Свиридов on 03.05.15.
//  Copyright (c) 2015 Владимир Свиридов. All rights reserved.
//

#import "ShtilleEngine.h"
#include "../../../../sht/include/sht.h"

@implementation ShtilleEngine

-(void)Load
{
    sht::OpenGlApplication * app = new sht::OpenGlApplication();
    app->Run();
    delete app;
    //LoadTest();
}

@end
