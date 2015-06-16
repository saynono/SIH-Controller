//
//  KeyDefinitions.cpp
//  PianoFingerprint
//
//  Created by say nono on 12.06.15.
//  Copyright (c) 2015 __MyCompanyName__. All rights reserved.
//

#include "KeyDefinitions.h"

void KeyDefinitions::setup(){
    unsigned int x = lexical_cast<unsigned int>("ee1c25");
    ofLog() << "lexical_cast : " << ofToString(x,16);
    ofColor clr;
    std::string buffer = ofBufferFromFile("key_definitions.xml").getText();
    pugi::xml_document doc;
    pugi::xml_parse_result result = doc.load(buffer.c_str());
    pugi::xml_node node = doc.child("key-definitions");

//    std::map<int,KeyDefinition*> defs;
    
    pugi::xml_node keysNode = node.child("keys");
    if( keysNode ){
        for( pugi::xml_node keyNode = keysNode.first_child(); keyNode; keyNode = keyNode.next_sibling() ){
            
            string clrSecStr = keyNode.child("secondary_colour").text().as_string();
            int clrSec = ofHexToInt( clrSecStr.substr(2) );
            
            string clrPrimStr = keyNode.child("primary_colour").text().as_string();
            int clrPrim = ofHexToInt( clrPrimStr.substr(2) );
            
            KeyDefinition* keyDef = new KeyDefinition();
            keyDef->description = keyNode.child("colour_description").text().as_string();
            keyDef->primaryColour.setHex( clrPrim );
            keyDef->secondaryColour.setHex( clrSec );
            keyDef->name = keyNode.child("key_name").text().as_string();
            keyDef->description = keyNode.child("colour_description").text().as_string();
            keyDef->id = keyNode.child("value").text().as_int();
            for( pugi::xml_node keyValueNode = keyNode.child("key_values").first_child(); keyValueNode; keyValueNode = keyValueNode.next_sibling() ){
                defs[ keyValueNode.text().as_string() ] = keyDef;
            }
            defsID[ keyDef->id ] = keyDef;
            
        }
    }

}