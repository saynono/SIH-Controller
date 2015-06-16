//
//  Fonts.cpp
//  PianoFingerprint
//
//  Created by say nono on 13.06.15.
//  Copyright (c) 2015 __MyCompanyName__. All rights reserved.
//

#include "Fonts.h"

Fonts::Fonts(){
    setup();
}

void Fonts::setup(){
    

    fontRobotoLight8.loadFont("fonts/RobotoCondensed-Light.ttf", 8);
    fontRobotoRegular8.loadFont("fonts/RobotoCondensed-Regular.ttf", 8);
    fontRobotoBold8.loadFont("fonts/RobotoCondensed-Bold.ttf", 8);

    fontRobotoLight12.loadFont("fonts/RobotoCondensed-Light.ttf", 12);
    fontRobotoRegular12.loadFont("fonts/RobotoCondensed-Regular.ttf", 12);
    fontRobotoBold12.loadFont("fonts/RobotoCondensed-Bold.ttf", 12);

    fontRobotoLight30.loadFont("fonts/RobotoCondensed-Light.ttf", 30);
    fontRobotoRegular30.loadFont("fonts/RobotoCondensed-Regular.ttf", 30);
    fontRobotoBold30.loadFont("fonts/RobotoCondensed-Bold.ttf", 30);
}