//
//  TracksHandler.cpp
//  PianoFingerprint
//
//  Created by say nono on 31.05.15.
//  Copyright (c) 2015 __MyCompanyName__. All rights reserved.
//

#include "TracksHandler.h"


TracksHandler::~TracksHandler(){
    ofLog() << " ============== END OF TRACKS HANDLER ===================";
}

void TracksHandler::setup( KeyDefinitions* kd ){
    
    currentTrackId = -1;
    keyDefinitions = kd;
    pagePreviewMargin = 80;
    pagePreviewWidth = 1;
    pagePreviewHeight = 1;
    pageScale = 1;
    
    overviewMargin = 10;
    overviewHeight = 200;
    overviewWidth = ofGetWidth();
    
    ofSetCurveResolution(40);
    isTrackLoaded = false;
    
    
    ofDirectory dir("./tracks");
    dir.listDir();
    vector<ofFile> folder = dir.getFiles();
    for( int i=0;i<folder.size();i++ ){
        if( folder[i].isDirectory() ){
            addTrack( ofDirectory(folder[i].getAbsolutePath()) );
        }
    }
//    loadTrack(0);
}

void TracksHandler::exit(){
    ofLog() << " ============== EXIT OF TRACKS HANDLER ===================";
}

bool TracksHandler::isLoaded(){
    return isTrackLoaded;
}

void TracksHandler::addTrack( ofDirectory folder ){
    ofLogVerbose("TracksHandler") << "--------------------------------------";
    ofLogVerbose("TracksHandler") << " ADDING TRACK : " << folder.getAbsolutePath();
    folder.listDir();
    ofLogVerbose("TracksHandler") << "----------------------";
    
    ofPtr<Track> track( new Track() );
    track->setup();
    track->loadTrack( folder );
    tracks.push_back(track);
}

bool TracksHandler::loadTrack( int trackId ){
    
    if( trackId >= tracks.size() || trackId == currentTrackId ) return false;
    overviewWidth = ofGetWidth();
    isTrackLoaded = false;
    currentTrackId = trackId;
    currentMeasureId = 0;
    currentTrack = tracks[currentTrackId];
    currentTrack->printTrackInfo();
    loadPage(0);
    isTrackLoaded = true;
    return true;
}

void TracksHandler::loadPage( int pageID ){
    
    if(!currentTrack->getPage(pageID)) return;
    currentPageId = pageID;
    currentPage = currentTrack->getPage(pageID);
    imageCenter.loadImage( currentPage->imageBuffer );
    pagePreviewWidth = currentPage->width;
//    pagePreviewCount = 1;
    if( currentPage->hasPrevPage ){
        imageLeft.loadImage( currentTrack->getPage(currentPage->prevPageId)->imageBuffer );
    }
    if( currentPage->hasNextPage ){
        imageRight.loadImage( currentTrack->getPage(currentPage->nextPageId)->imageBuffer );
    }

    overviewMeasureWidth = overviewWidth/(float)currentTrack->getMeasures().size();
    overviewMeasureWidth = (overviewWidth-(overviewMargin*2))/(float)currentTrack->getMeasures().size();
    pagePreviewHeight = (ofGetHeight()-overviewHeight) - 2 * pagePreviewMargin;
    pageScale = pagePreviewHeight/currentPage->height;
    pagePreviewWidth *= pageScale;
    
}

void TracksHandler::draw(){
    
    if( !isTrackLoaded ) return;
    
    float h = overviewHeight;
    float w = ofGetWidth();
    
    int wTitel = Fonts::one().fontRobotoLight30.stringWidth( currentTrack->getTrackName() );
    Fonts::one().fontRobotoLight30.drawString( currentTrack->getTrackName(), ofGetWidth()/2 - wTitel/2, 50);
    
    ofPushMatrix();
    drawPages( ofGetWidth(), ofGetHeight() - h );
    ofPopMatrix();
    ofPushMatrix();
    drawOverview( w, h );
    ofPopMatrix();
    
    ofPushMatrix();

    Page* p = currentPage;
    ofPoint pA1( overviewMargin + p->measures[0]->id * overviewMeasureWidth, overviewMargin + ofGetHeight()-h);
    ofPoint pA2( overviewMargin + (p->measures[p->measures.size()-1]->id + 1) * overviewMeasureWidth, overviewMargin + ofGetHeight()-h);
    ofPoint p1( (ofGetWidth()/2.0f), pagePreviewHeight+pagePreviewMargin );
    ofPoint p2( ofGetWidth()/2.0f, pagePreviewHeight+pagePreviewMargin );
    drawOverviewPreviewConnection( pA1, pA2, p1 , p2);
    ofPopMatrix();

}

void TracksHandler::drawOverview( float w, float h){
    ofTranslate(0, ofGetHeight()-h);
    h = 140;
    ofPushMatrix();
    ofPushStyle();
    int margin = 10;
    ofSetColor(20,20,20);
    ofRect(0, 0, w, h);
    w -= margin*2;
    h -= margin*2;
    ofSetColor(70, 70, 70);
    ofTranslate( margin , margin );
    ofRect(0, 0, w, h);
    
    int wMeasure = overviewMeasureWidth;
    
    ofPushMatrix();
    int lPages = currentTrack->pages.size();
    int wCurrentMeassure;
    for( int i=0;i<lPages;i++ ){
        int amountMeasures = currentTrack->getPage(i)->measures.size();
        wCurrentMeassure = wMeasure * amountMeasures;
        ofSetColor(200);
        Fonts::one().fontRobotoLight12.drawString( "PAGE "+ofToString(i+1), wCurrentMeassure/2 - 20, -3);
        if( i%2 == 0) ofSetColor(100);
        else ofSetColor(80);
        ofRect( 0,0,wCurrentMeassure,h );
        ofTranslate( wCurrentMeassure, 0);
    }
    ofPopMatrix();
    
    float marginMeasure = 3;
    int beats = -1;
    ofRectangle measureRect(marginMeasure, marginMeasure*4, wMeasure-(2*marginMeasure), h-(4*2*marginMeasure));
    
//    ofRectangle pulseRect(marginMeasure, marginMeasure*4, wMeasure-(2*marginMeasure), h-(4*2*marginMeasure));

    ofNoFill();
    ofPushMatrix();
    int id;
    int l = currentTrack->getMeasures().size();
    float beatsOffset;
    float px,py;
    float intensity = 1;
    Measure* measure;
    for( int i=0;i<l;i++ ){
        ofSetColor(200);
        measure = currentTrack->getMeasures()[i];
        id = measure->id;
        if( id == currentMeasureId ) ofSetColor(200, 0, 0);
        Fonts::one().fontRobotoLight12.drawString( ofToString(id+1), 10, 35);
        ofRect( measureRect );
        beatsOffset = measureRect.width / (measure->beats);
        px = 0;
        for( int j=0;j<measure->beats;j++ ){
            py = 120;
            px = marginMeasure+(j+1)*beatsOffset;
            for( int k=0;k<measure->beatsMap[j]->keys.size();k++ ){
                intensity = measure->beatsMap[j]->keyIntensities[k];
                Fonts::one().fontRobotoLight8.drawString( measure->beatsMap[j]->keys[k]->key_value, 3+px-10, py - 50 * intensity );
//                py += 12;
            }
            if( j<measure->beats - 1 ) ofLine( px, 40, marginMeasure+(j+1)*beatsOffset, 80 );
        }

        ofTranslate( wMeasure, 0);
    }
    ofPopMatrix();
    ofPopStyle();
    ofPopMatrix();
}

void TracksHandler::drawPages( float w, float h ){
    
    
    h -= pagePreviewMargin * 2;
    
//    int wTot = currentTrack->pages[currentPageId].width;
    int hTot = currentPage->height;
//    if( !currentTrack->getPage( currentPageId )->isLastPage ){
//        wTot += currentTrack->pages[currentPageId+1].width;
//    }
    
    ofPushMatrix();
    ofTranslate( ofGetWidth()/2.0f, pagePreviewMargin);
    ofScale(pageScale, pageScale);
    ofTranslate(-currentPage->width/2, 0);

    ofSetColor(100);
    float pageWidth = currentTrack->pages[currentPageId]->width;
    if( currentPage->hasPrevPage ){
        float pageWidthPrev = currentTrack->getPage(currentPage->prevPageId)->width;
        imageLeft.draw(-pageWidthPrev - 5, 0,pageWidthPrev,currentTrack->getPage(currentPage->prevPageId)->height);
    }
    if( currentPage->hasNextPage ){
        float pageWidthNext = currentTrack->getPage(currentPage->nextPageId)->width;
        imageRight.draw( pageWidth+5, 0, pageWidthNext,currentTrack->getPage(currentPage->nextPageId)->height);
//        imageRight.draw( pageWidth+5, 0, 200,200);
    }
    
//    ofLog() << " ===== ==== " << currentPage->id << " ===== =====";
    
    ofSetColor(255);
    imageCenter.draw(0,0,pageWidth,currentPage->height);
    
    ofSetColor(100);
    Fonts::one().fontRobotoRegular30.drawString( "Page " + ofToString(currentPageId+1), currentTrack->pages[currentPageId]->width/2-50, 120);
    drawPage( currentPageId );
    
    ofPopMatrix();
}

void TracksHandler::drawPage( int pageId ){

    ofPushMatrix();
    ofPushStyle();
//    ofScale(.4, .4);
//    if( tracks.size() > 0 ){
        Page * page = currentTrack->getPage( pageId );
        Measure* measure;
        int px = page->marginLeft;
        int py = page->marginTop;

//        ofNoFill();
        int row = 0;
        Measure* firstMeasure = (*page->measures.begin());
        Measure* lastMeasure = (*(page->measures.end()-1));
        for( int i=0;i<page->measures.size();i++ ){
            measure = page->measures[i];
            if(measure->row != row){
                row = measure->row;
                px = page->marginLeft;
                py = row * 250;
            }
//            ofRect(px, py, measure->width, 200);
            
            if( measure->id == currentMeasureId ) ofSetColor(255, 0, 0);
            else ofSetColor(100);
            ofEnableBlendMode( OF_BLENDMODE_ADD );
            ofRect(measure->x, measure->y-50, measure->width, measure->height+100);
            ofDisableBlendMode();
            ofRect(measure->x, measure->y, 8, measure->height);
            ofRect(measure->x+measure->width-8, measure->y, 8, measure->height);
            px += measure->width;
        }
        
        vector<Key*> keys = currentTrack->getKeys();
        vector<Measure*> measures = currentTrack->getMeasures();
//        ofEnableBlendMode( OF_BLENDMODE_ALPHA );
        for( int i=0;i<keys.size();i++ ){
            Key* key = keys[i];
            if( key->end >=firstMeasure->id && key->begin <= lastMeasure->id){
                
//                if( keyDefinitions->defs[key->key_value] ){
                    ofSetColor( keyDefinitions->defs[key->key_value]->primaryColour );
                    for( int j=key->begin;j<=key->end;j++){
                        measure = measures[j];
                        if( measure->page == page->id ){
                            ofRect(measure->x, measure->y-50+ (key->id%2)*10, measure->width, 10);
                            Fonts::one().fontRobotoBold30.drawString( ofToString(measure->id+1), measure->x, measure->y-50);
                        }
                        if( measure == lastMeasure ) break;
                    }
//                }
            }
        }
//        ofDisableBlendMode();

//        currentTrack->draw();
//    }
    ofPopStyle();
    ofPopMatrix();
}

void TracksHandler::drawOverviewPreviewConnection( ofPoint pA1, ofPoint pA2, ofPoint pB1, ofPoint pB2 ){
    ofPushMatrix();
    ofPushStyle();
    
    ofSetColor(200);
    ofNoFill();
    ofSetLineWidth(1);
    
    float midHeight = (pB1.y - pA1.y)/1.2f + pA1.y;
    
    ofBezier( pA1.x, pA1.y, pA1.x, midHeight, pB1.x, midHeight, pB1.x, pB1.y);
    ofBezier( pA2.x, pA2.y, pA2.x, midHeight, pB1.x, midHeight, pB1.x, pB1.y);

    ofPopStyle();
    ofPopMatrix();
    
}

void TracksHandler::gotoPrevMeasure(){
    if( !isLoaded() ) return;
    currentMeasureId --;
    if( currentMeasureId < 0 ){
        currentMeasureId = 0;
        return;
    }
    if( currentTrack->getMeasure( currentMeasureId )->page != currentPageId ){
        loadPage( currentTrack->getMeasure( currentMeasureId )->page );
    }
}
void TracksHandler::gotoNextMeasure(){
    if( !isLoaded() ) return;
    currentMeasureId ++;
    if( currentMeasureId > currentTrack->getMeasures().size()-1 ){
        currentMeasureId = currentTrack->getMeasures().size()-1;
//        return;
    }
    if( currentTrack->getMeasure( currentMeasureId )->page != currentPageId ){
        loadPage( currentTrack->getMeasure( currentMeasureId )->page );
    }
}
void TracksHandler::gotoPrevPage(){
    if( !isLoaded() ) return;
    if(currentPageId<=0) return;
    loadPage( currentPageId - 1 );
    
}
void TracksHandler::gotoNextPage(){
    if( !isLoaded() ) return;
    if(currentPageId > currentTrack->pages.size()-1) return;
    loadPage( currentPageId + 1 );
}

ofPtr<Track> TracksHandler::getCurrentTrack(){
    return currentTrack;
}

vector< ofPtr<Track> > TracksHandler::getTrackList(){
    return tracks;
}
