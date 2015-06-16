//
//  Track.h
//  PianoFingerprint
//
//  Created by say nono on 05.06.15.
//  Copyright (c) 2015 __MyCompanyName__. All rights reserved.
//

#pragma once

#include "ofMain.h"
#include "ofxPugiXML.h"

struct Key {
    int id;
//    int value;
    string key_value;
    string keyName;
    string description;
    float begin;
    float end;
    int fadeBegin;
    int fadeEnd;
};


struct Beat {
    int id;
    vector<Key*> keys;
    vector<float> keyIntensities;
};


struct Measure {
    int id;
    int page;
    int row;
    int width;
    int height;
    int x;
    int y;
    int beats;
    int beatsType;
    map<unsigned char,Beat*> beatsMap;
};

struct Page {
    int id;
    int width;
    int height;
    int marginLeft;
    int marginTop;
    int marginRight;
    int marginBottom;
    bool hasNextPage;
    bool hasPrevPage;
    int prevPageId;
    int nextPageId;
    string file;
    vector<Measure*> measures;
    ofBuffer imageBuffer;
};

class Track{

public:
    
	void setup();
    void loadTrack( ofDirectory folder );
    void printTrackInfo();
	void update();
	void draw();
    
    ofVec2f setPageLayout( Page* page, pugi::xml_node layout );
    ofVec2f setSystemLayout( pugi::xml_node layout );
    Page* getPage( int page );
    vector<Measure*> getMeasures();
    vector<Key*> getKeys();
    string getTrackName();
    Measure* addMeasure( pugi::xml_node measureXml, int page, int column, int row );
    
    Measure* getMeasure( int id );
    
    void processTrack();
    void processMusicXml();
    void processAnnotations();
    
    void assignKeysToBeats();
    
    bool hasMusicXML;
    bool hasAnnotations;
    bool hasWav;
    
    int measuresOffset;
    int measuresAmount;
    
    pugi::xml_node musicXml;
    pugi::xml_node annotationsXml;
    pugi::xml_document docMusicXml;
    pugi::xml_document docAnnotationsXml;

    vector<Page*> pages;
    map<int,Page*> pagesId;
    vector<Measure*> measures;
    vector<Key*> keys;
    map<int,Measure*> measuresId;
    map<int,Measure*> beatsMeasureMap;
    map<int,Beat*> beatsMap;
    
    string trackName;
    ofDirectory path;
	
};



