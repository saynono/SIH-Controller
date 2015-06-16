//
//  KeyDefinitions.h
//  PianoFingerprint
//
//  Created by say nono on 12.06.15.
//  Copyright (c) 2015 __MyCompanyName__. All rights reserved.
//

#pragma once

#include "ofMain.h"
#include "ofxPugiXML.h"

template<typename T2, typename T1>
inline T2 lexical_cast(const T1 &in) {
    T2 out;
    std::stringstream ss;
    ss << in;
    ss >> out;
    return out;
}

struct KeyDefinition {
    int id;
    string value;
    string name;
    string description;
    ofColor primaryColour;
    ofColor secondaryColour;
};

class KeyDefinitions{

public:
	
	void setup();
    
    std::map<int,KeyDefinition*> defsID;
    std::map<string,KeyDefinition*> defs;
//    std::map<int,string> defs;
    
};



