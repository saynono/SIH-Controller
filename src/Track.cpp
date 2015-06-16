//
//  Track.cpp
//  PianoFingerprint
//
//  Created by say nono on 05.06.15.
//  Copyright (c) 2015 __MyCompanyName__. All rights reserved.
//

#include "Track.h"

	
void Track::setup(){
}

void Track::printTrackInfo(){

    float len = 0;
    for( int i=0;i<pages.size();i++ ){
        len += pages[i]->imageBuffer.size();
    }

    ofLog() << "-------------------------------------------------------";
    ofLog() << "TRACK       | " << trackName;
    ofLog() << "PAGES       | " << pages.size();
    ofLog() << "MEASURES    | " << measures.size();
    ofLog() << "SIZE TOTAL MEMORY: " << (len/1048576) << " MB         ";
    ofLog() << "\n";
}

void Track::loadTrack( ofDirectory folder ){
    
    path = folder;
    measuresOffset = 0;
//    pugi::xml_document doc;

    for( int i=0;i<folder.size();i++ ){
        if( folder[i].isFile() ){
            string ext = folder[i].getExtension();
            string fileName = folder[i].getFileName();
            
            if( fileName == "score.xml"){
                ofLogVerbose("TracksHandler") << " MusicXML present";
                
                std::string buffer = ofBufferFromFile( folder[i].getAbsolutePath() ).getText();
                pugi::xml_parse_result result = docMusicXml.load(buffer.c_str());
                musicXml = docMusicXml.child("score-partwise");
                hasMusicXML = true;
                
            }else if( fileName == "wav"){
                ofLogVerbose("TracksHandler") << " Wave file present";
                
            }else if( fileName == "annotations.xml"){
                ofLog() << "TracksHandler" << " Annotations file present";
                std::string buffer = ofBufferFromFile( folder[i].getAbsolutePath() ).getText();
//                pugi::xml_document doc;
                pugi::xml_parse_result result = docAnnotationsXml.load(buffer.c_str());
                annotationsXml = docAnnotationsXml.child("score-annotations");
                hasAnnotations = true;
            }else{
//                ofLogVerbose("TracksHandler") << "Strange file : " << ext << " : " << folder[i].getFileName();
            }
        }
    }
    
    
    processTrack();
    
}

void Track::processTrack(){
    if( hasAnnotations ){
        processAnnotations();
    }
    if( hasMusicXML){
        processMusicXml();
    }
    
    if( hasMusicXML && hasAnnotations ){
        assignKeysToBeats();
    }
    printTrackInfo();
}

void Track::processMusicXml(){
    
    pugi::xml_node messageMeasures = musicXml.child("part");
    
    int page = -1;
    int row = 1;
    int column = 0;
    int measureCount = 0;
    ofVec2f pageOffset;
    ofVec2f systemOffset;
    int px = 0;
    int py = 0;
    int staffHeight;
    int beamWidth = 8;
    int hyphenWidth = 20;
    int beats = 4;
    int beatsType = 4;
    
    int beatsCounter = 0;
//    int measureOffset = 14;
    pugi::xml_node appearance = musicXml.child("appearance");
    if( appearance ){
//        <distance type="hyphen">120</distance>
//        <distance type="beam">8</distance>
    }
    
    for (pugi::xml_node measure = messageMeasures.first_child(); measure; measure = measure.next_sibling()){
        pugi::xml_node printNode = measure.child("print");
        if( printNode ){
            int p = printNode.attribute("page-number").as_int()-1;
            if( p > -1 && page != p){
                page = p;
                Page* newPage = new Page();
                newPage->id = pages.size();
                newPage->hasPrevPage = false;
                newPage->hasNextPage = false;
                pages.push_back( newPage );
                pagesId[ newPage->id ] = newPage;
                pageOffset = setPageLayout( newPage, printNode.child("page-layout") );
                py = pageOffset.y;
                row = 1;
            }else{
                row++;
                column = 1;
                py += staffHeight;
            }
            
            pugi::xml_node staffLayoutNode = printNode.child("staff-layout");
            if( staffLayoutNode ){
                int amount = staffLayoutNode.attribute("number").as_int();
                int hh = staffLayoutNode.child("staff-distance").text().as_int();
                staffHeight = amount * 36 + hh * (amount-1);
            }
            pugi::xml_node systemLayoutNode = printNode.child("system-layout");
            if( systemLayoutNode ){
                systemOffset = setSystemLayout(systemLayoutNode);
                py += systemOffset.y;
//                if(page==1) ofLog() << page << " => New Row : " << systemOffset << "        y : " << py;
            }
            px = pageOffset.x + systemOffset.x + hyphenWidth;
        }
        
        pugi::xml_node attributesNode = measure.child("attributes");
        if(attributesNode){
            pugi::xml_node timeNode = attributesNode.child("time");
            if(timeNode){
                beats = timeNode.child("beats").text().as_int();
                beatsType = timeNode.child("beat-type").text().as_int();
                ofLog() << " ===========================================================";
                ofLog() << " BEATS: " << beats << "         BEATS TYPE: " << beatsType;
                ofLog() << " ===========================================================";
            }
        }
        
        
        Measure* m = addMeasure(measure, page, column, row);
        m->x = px;
        m->y = py;
        m->height = staffHeight;
        m->id = measureCount - measuresOffset;
        m->beats = beats;
        m->beatsType = beatsType;
//        m->beatsStartId = beatsCounter;
        
        
        if( m->id > -1 && m->id < measuresAmount ){
            pages[m->page]->measures.push_back( m );
            measures.push_back(m);
            measuresId[ m->id ] = m;
            
            
            for( int j=0;j<beats;j++ ){
                Beat* beat = new Beat();
                beat->id = beatsCounter+j;
                m->beatsMap[j] = beat;
                beatsMeasureMap[beat->id] = m;
                beatsMap[beat->id] = beat;
            }
            
            beatsCounter += beats;
            
        }
        measureCount++;
        column ++;
        px += m->width + beamWidth;
    }
//    
//    map<int, Measure* >::iterator it = beatsMeasureMap.begin();
//    for( ; it!=beatsMeasureMap.end(); ++it ){
//        ofLog() << "MAP BEAT " << (*it).first << "    MEASURE => " << (*it).second->id << " " << (*it).second->beatsMap[0]->id;
//    }
    
    
    for( vector<Page*>::iterator it = (pages.begin());it!=(pages.end());++it){
        if( it != --pages.end()){
            (*it)->nextPageId = (*it)->id+1;
            getPage( (*it)->nextPageId )->hasPrevPage = true;
        }
        if( it != pages.begin() ){
            (*it)->prevPageId = (*it)->id-1;
            getPage( (*it)->prevPageId )->hasNextPage = true;
        }
    }    
}

ofVec2f Track::setPageLayout( Page* page, pugi::xml_node layout ){

    page->width = layout.child("page-width").text().as_int();
    page->height = layout.child("page-height").text().as_int();
    page->file = path.getAbsolutePath() +"/"+ "score_image_p"+ofToString(page->id+1)+".png";
    page->imageBuffer = ofBufferFromFile(page->file);
//    ofBuffer buf = ofBuffer( currentTrack->getPage(pageID)->file );
//    image.loadImage( buf );

    
    page->marginLeft = layout.child("page-margins").child("left-margin").text().as_int();
    page->marginTop = layout.child("page-margins").child("top-margin").text().as_int();
    page->marginRight = layout.child("page-margins").child("right-margin").text().as_int();
    page->marginBottom = layout.child("page-margins").child("bottom-margin").text().as_int();
    
    return ofVec2f(page->marginLeft,page->marginTop);
}

ofVec2f Track::setSystemLayout( pugi::xml_node layout ){
    int ox = 0;
    int oy = 0;
    if( layout.child("top-system-distance") ){
        oy = layout.child("top-system-distance").text().as_int();
    }
    if( layout.child("system-distance") ){
        oy = layout.child("system-distance").text().as_int();
    }
    if( layout.child("system-margins") && layout.child("system-margins").child("left-margin")){
        ox = layout.child("system-margins").child("left-margin").text().as_int();
    }
    return ofVec2f(ox,oy);
}

Measure* Track::addMeasure( pugi::xml_node measureXml, int page, int column, int row ){
    Measure* measure = new Measure();
    measure->page = page;
    measure->row = row;
    measure->width = measureXml.attribute("width").as_int();
//    measure->boundingbox = ofRectangle( , , measure->width, );
    return measure;
}

void Track::processAnnotations(){
    trackName = annotationsXml.child("title").text().as_string();
    pugi::xml_node measuresAnno = annotationsXml.child("measures");
    if( measuresAnno ){
        measuresOffset = measuresAnno.child("offset").text().as_int();
        measuresAmount = measuresAnno.child("length").text().as_int();
    }

    pugi::xml_node keysNode = annotationsXml.child("keys");
    if( keysNode ){
//        pugi::xml_node  keyNode = keysNode.first_child();
        for( pugi::xml_node  keyNode = keysNode.first_child(); keyNode; keyNode = keyNode.next_sibling() ){
            Key* key = new Key();
            key->id = keys.size();
            key->begin = keyNode.child("begin").text().as_float()-1;
            key->end = keyNode.child("end").text().as_float()-1;
//            key->value = keyNode.child("value").text().as_int();
            key->key_value = keyNode.child("key_value").text().as_string();
            
            key->keyName = keyNode.child("keyName").text().as_string();
            key->description = keyNode.child("description").text().as_string();
            key->fadeBegin = keyNode.child("fadebegin").text().as_float();
            ofLog() << " ------------ key->fadeBegin : " << key->fadeBegin << "       " << key->fadeEnd << " : " << key->fadeEnd;
            key->fadeEnd = keyNode.child("fadeend").text().as_float();
            keys.push_back( key );
        }
    }
//    std::basic_ostream<char, std::char_traits<char> > stream;
//    std::ostringstream stream;
//    annotationsXml.print(stream);
//    ofLog() << "====> " << stream.str();
}

void Track::assignKeysToBeats(){
    
    int startBeat;
    int endBeat;
    int startMeasureId;
    int endMeasureId;
    float steps;
    
    Measure* measure;
    for( vector<Key*>::iterator it = keys.begin(); it != keys.end(); ++it) {
        Key* key = (*it);
        startMeasureId = (int)key->begin;
        endMeasureId = (int)key->end;

        float startPos = startMeasureId + max(0.0f, key->begin - startMeasureId - .1f);
        float endPos = endMeasureId + max(0.0f, key->end - endMeasureId);
        if( endPos == endMeasureId ) endPos += (getMeasure(endMeasureId)->beats-1)/10.0f;
        else endPos -= .1;
        
        measure = getMeasure( startMeasureId );
        float fadeInBeats = 1.0;//1.0f / (measure->beats * key->fadeBegin);
        float fadeOutBeats = 1.0f;//1.0f / (measure->beats * key->fadeEnd);
        if( key->fadeBegin > 0 ){
            fadeInBeats = 1.0f / (float)(measure->beats * key->fadeBegin + 1);
        }
        int lEndFade = 0;
        if( key->fadeEnd > 0 ){
            lEndFade = getMeasure( endMeasureId )->beats * key->fadeEnd + 1;
            fadeOutBeats = 1.0f / (float)lEndFade;
            
        }
        
        int startBeatPos = (int)round(startPos*10.0)%10;
        int endBeatPos = (int)round(endPos*10)%10;
        
        
        Beat* startBeat = getMeasure( startMeasureId )->beatsMap[startBeatPos];
        Beat* endBeat = getMeasure( endMeasureId )->beatsMap[endBeatPos];
        
        ofLog() << key->key_value <<  " ||  STARTBEAT : " <<  startBeat->id << "       ENDBEAT : " << endBeat->id << "          fadein " << key->fadeBegin << "     lEndFade : " << lEndFade;
        
        float intensity = fadeInBeats;
        for( int i=startBeat->id;i<=endBeat->id;i++ ){
            beatsMap[i]->keys.push_back( key );
            beatsMap[i]->keyIntensities.push_back( intensity );
            intensity += fadeInBeats;
            if( intensity > 1 ){
                intensity = 1;
                fadeInBeats = 0;
            }
            if( i > endBeat->id - lEndFade ) intensity -= fadeOutBeats;
        }
        
//        int beatCounter = startBeat;
//        int measureCounter = startMeasureId;
//        float intensity = fadeInBeats;
//        int beatTotCounter = 0;
//        int beatTotCounterPreCalc = (key->end-(key->begin+1))*measure->beats + measure->beats- startBeat + endBeat+1;
//        
//        Beat* start
//        ofLog() << " => " <<
//        
//        
////        ofLog() << " startPos: " << startBeat << "       endBeat: " << endBeat;
////        ofLog() << " key->begin: " << key->begin << " >>      key->end: " << key->end;
//        
//        while( beatCounter < 1000 ){
////            ofLog() << "b" << beatCounter << "  m" << measureCounter;
//            measure = getMeasure( measureCounter );
////            if( beatCounter >= measure->beats ) break;
//            measure->beatsMap[ beatCounter ]->keys.push_back( key );
//            measure->beatsMap[ beatCounter ]->keyIntensities.push_back( intensity );
//
//            intensity = min(1.0f,(intensity + fadeInBeats));
//            beatTotCounter++;
//            if( measureCounter >= endMeasureId && beatCounter >= endBeat ) break;
//            beatCounter ++;
//            if( beatCounter >= measure->beats ){
//                beatCounter = 0;
//                measureCounter++;
//            }
//        }
//        ofLog() << key->key_value <<  " beatTotCounter : " << beatTotCounter << "       beatTotCounterPreCalc : " << beatTotCounterPreCalc;

    }
}


string Track::getTrackName(){
    return trackName;
}

Page* Track::getPage( int i ){
    return pagesId[i];
//    return &pages[ min(i,(int)pages.size()-1)];
}

vector<Measure*> Track::getMeasures(){
    return measures;
}

Measure* Track::getMeasure( int id ){
    return measuresId[ id ];
}

vector<Key*> Track::getKeys(){
    return keys;
}

void Track::update(){
}