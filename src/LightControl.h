//
//  LightControl.h
//  PianoFingerprint
//
//  Created by say nono on 12.06.15.
//  Copyright (c) 2015 __MyCompanyName__. All rights reserved.
//

#pragma once

#include "ofMain.h"
#include "KeyDefinitions.h"
#include "ofxMidi.h"
#include "ofxDmx.h"
#include "ofxAnimatable.h"
#include "ofxAnimatableOfColor.h"

class LightControl : public ofxMidiListener {

public:
	
    void setup( KeyDefinitions* keys );
    void newMidiMessage(ofxMidiMessage& msg);
	void update();
//	void draw();
    
    
    KeyDefinitions* keys;
    ofxMidiIn midiIn;
    ofxDmx dmx;
    
    ofxAnimatableOfColor primaryColour;
    ofxAnimatableOfColor secondaryColour;

};



