//
//  LightControl.cpp
//  PianoFingerprint
//
//  Created by say nono on 12.06.15.
//  Copyright (c) 2015 __MyCompanyName__. All rights reserved.
//

#include "LightControl.h"

	
void LightControl::setup( KeyDefinitions* k ){
    dmx.connect("tty.usbserial-EN099266");
    keys = k;

//    midiIn.listPorts(); // via instance
    midiIn.openPort("Kenton Killamix Mini");
    midiIn.ignoreTypes(false, false, false);
    midiIn.addListener(this);
    
    primaryColour.setDuration(1.5);
    secondaryColour.setDuration(2);

//    midiIn.setVerbose(true);
}
	
void LightControl::newMidiMessage(ofxMidiMessage& msg) {
    
    ofLog() << "VAL :  " << msg.value;
    
    int l = keys->defs.size();
    int idx = (msg.value % 12) - 5;
//    int idx = round(ofRandom(-5.9, 6.9));
    
    if( keys->defsID[idx] ){
        KeyDefinition* keyDef = keys->defsID[idx];
        primaryColour.animateTo( keyDef->primaryColour );
        secondaryColour.animateTo( keyDef->secondaryColour );

        ofLog() << "Selecting Key : " << keyDef->name << "  => " << keyDef->description;
        ofLog() << " keyDef->primaryColour  : " << ofToHex(keyDef->primaryColour.getHex());
        ofLog() << " keyDef->secondaryColour : " << ofToHex(keyDef->secondaryColour.getHex());
    }
}

void LightControl::update(){
    primaryColour.update( 1.0f/60.0f );
    secondaryColour.update( 1.0f/60.0f );
    dmx.setLevel(1, (int)primaryColour.getCurrentColor().getHue() );
    dmx.setLevel(2, (int)primaryColour.getCurrentColor().getSaturation());
    dmx.setLevel(3, (int)primaryColour.getCurrentColor().getBrightness());
    dmx.setLevel(4, (int)secondaryColour.getCurrentColor().getHue() );
    dmx.setLevel(5, (int)secondaryColour.getCurrentColor().getSaturation());
    dmx.setLevel(6, (int)secondaryColour.getCurrentColor().getBrightness());
    dmx.update();
    
}
