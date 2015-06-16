//
//  TracksHandler.h
//  PianoFingerprint
//
//  Created by say nono on 31.05.15.
//  Copyright (c) 2015 __MyCompanyName__. All rights reserved.
//

#pragma once

#include "ofMain.h"
#include "Track.h"
#include "KeyDefinitions.h"
#include "Fonts.h"


class TracksHandler{

public:
    
    std::vector< ofPtr<Track> > tracks;
    
    ~TracksHandler();
	void setup( KeyDefinitions* keys );
    void exit();
    bool isLoaded();
    void addTrack( ofDirectory folder );
    bool loadTrack( int trackId );
    void loadPage( int pageID );
	void draw();
    void drawOverview( float w, float h );
    void drawPages( float w, float h );
    void drawPage( int pageId );
    void drawOverviewPreviewConnection( ofPoint pA1, ofPoint pA2, ofPoint pB1, ofPoint pB2 );

    ofPtr<Track> getCurrentTrack();
    vector< ofPtr<Track> > getTrackList();
    
    void gotoPrevMeasure();
    void gotoNextMeasure();
    void gotoPrevPage();
    void gotoNextPage();
    
    ofImage imageLeft;
    ofImage imageCenter;
    ofImage imageRight;
    ofPtr<Track> currentTrack;
    KeyDefinitions* keyDefinitions;

    int currentTrackId;
    int currentPageId;
    int currentMeasureId;
    Page* currentPage;
    
    float pageScale;
    float pagePreviewMargin;
    float pagePreviewHeight;
    float pagePreviewWidth;
    int pagePreviewCount;
    
    float overviewMargin;
    float overviewHeight;
    float overviewWidth;
    float overviewMeasureWidth;
    
    bool isTrackLoaded;

};



