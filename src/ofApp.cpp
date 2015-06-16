/* This is an example of how to integrate maximilain into openFrameworks,
 including using audio received for input and audio requested for output.
 
 
 You can copy and paste this and use it as a starting example.
 
 */

#include "ofApp.h"

#include "include/opencv2/opencv.hpp"
#include "include/opencv/cv.h"

#include "ofxOpenCv.h"

#include "Fonts.h"

//--------------------------------------------------------------
void ofApp::setup(){
    
    sampleRate 	= 44100; /* Sampling Rate */
	bufferSize	= 512; /* Buffer Size. you have to fill this buffer with sound using the for loop in the audioOut method */
    fftSize = 1024;
    bufferDuration = 4.0f; // Duration in seconds

    frameWidth = 1024*2;
    frameHeight = 104*2;
    
    inputMode = INPUT_LIVE;
    
    doFind = false;

//    samplePlayer.load(ofToDataPath("03_Prelude_op67no1_mono.wav")); // ofToDataPath tells the load function to look in the data folder
    
	ofBackground(10,10,10);
    ofSetWindowShape(2400, 1600);
    
    ofSoundStreamSetup(2,2,this, sampleRate, bufferSize, 4); /* this has to happen at the end of setup - it switches on the DAC */
    
    ofxMaxiSettings::setup(sampleRate, 2, bufferSize);

    source.setup( sampleRate, bufferSize, fftSize );
    live.setup( bufferDuration, sampleRate, bufferSize, fftSize );

    finder.setup();
    keys.setup();
    
    tracks.setup( &keys );
    lights.setup( &keys );
    
//    temp();

}

void ofApp::exit(){
    source.exit();
    finder.exit();
    tracks.exit();
}

void ofApp::loadTrack( int id ){
    if( id > tracks.getTrackList().size() ){
        return;
    }
    ofLog() << "Load Track : " << id;
    if( tracks.loadTrack( id ) ){
        string pathMat = tracks.getCurrentTrack()->path.path() + "/fingerprint.png";
        if( !source.loadFingerprint( pathMat ) ){
            ofLog() << "Failed loading MAT file.";
            ofLog() << "Creating MAT from audio.";            
            string pathWave = tracks.getCurrentTrack()->path.path() + "/audio.wav";
            source.createFingerprint( pathWave );
        }else{
            ofLog() << "Successfully loaded MAT from file.";
        }
    }else{
        ofLog() << "Loading track failed! [" << id << "]";
    }
}

//--------------------------------------------------------------
void ofApp::update(){
    if( doFind ){
//        ofLog() << " FIND!";
//        doFind = false;
//        source.saveFingerprint("source_image.bmp");
//        live.saveFingerprint("live_window_image.bmp");
        finder.find( source.getMat(), live.getMat() );
    }
    lights.update();
}

//--------------------------------------------------------------
void ofApp::draw(){
    
    ofPushMatrix();
    ofSetColor(255);
    
    if( !tracks.isLoaded() ){
        vector< ofPtr<Track> > trackList = tracks.getTrackList();
        string trackListStr = "";
        for( int i=0;i<trackList.size();i++ ){
            trackListStr += ofToString(i+1) + " : " + trackList[i]->getTrackName() + "\n";
        }
        Fonts::one().fontRobotoRegular30.drawString( trackListStr, 20, 30 );
    }
    
    int y = 100;
    
    drawSource(0,ofGetHeight()-300, ofGetWidth(), 104 );
//    drawResult(100,y, frameWidth,frameHeight );
//    y += frameHeight + 20;
//    drawFFT(100,y,frameWidth,frameHeight);
//    y += frameHeight + 20;
//    drawOctaves(100,y, frameWidth,frameHeight );
//    y += frameHeight + 20;
//    drawSpectrum(100,y, frameWidth,frameHeight );

    tracks.draw();

    ofPopMatrix();
    

}

void ofApp::drawFFT( int x, int y, int w, int h ){
    ofPushStyle();
    ofNoFill();
    ofRect(x, y, w, h);
    ofFill();
    live.drawFFT(x, y, w, h);
    ofPopStyle();
}


void ofApp::drawSource( int x, int y, int w, int h ){    
    ofPushStyle();
    ofFill();
    ofPushMatrix();
    ofTranslate(x, y);
    ofTranslate(0,h);
    ofRotate( -90 );
    ofSetColor(255);
    source.draw( h, w );
    ofPopMatrix();
    ofNoFill();
    ofRect(x, y, w, h);
    ofFill();
    ofPopStyle();
}

void ofApp::drawOctaves( int x, int y, int w, int h ){
    ofPushStyle();
    ofNoFill();
    ofRect(x, y, w, h);
    ofFill();
    live.drawOctaves(x,y,w,h);
    ofPopStyle();
}

void ofApp::drawSpectrum( int x, int y, int w, int h ){
    ofPushStyle();
    ofFill();
    ofPushMatrix();
    ofTranslate(x, y);
    ofTranslate(0,h);
    ofRotate( -90 );
    //    ofTranslate(0,h);
    live.draw( h, w );
    ofPopMatrix();
    ofNoFill();
    ofRect(x, y, w, h);
    ofFill();
    ofPopStyle();
}

void ofApp::drawResult( int x, int y, int w, int h ){
    ofPushStyle();
    ofFill();
    ofPushMatrix();
    ofTranslate(x, y);
//    ofTranslate(0,h);
//    ofRotate( -90 );
    //    ofTranslate(0,h);
    finder.draw( w, h );
    ofPopMatrix();
    ofNoFill();
    ofRect(x, y, w, h);
    ofFill();
    ofPopStyle();
    
}

//--------------------------------------------------------------
void ofApp::audioOut(float * output, int bufferSize, int nChannels) {
    
    
    if( inputMode == INPUT_SAMPLE ){
        double sample;
        for (int i = 0; i < bufferSize; i++){
            sample = samplePlayer.playOnce();		// Get left channel sample
            output[i*nChannels    ] = sample;	// Put left channel sample in output
            output[i*nChannels + 1] = sample;	// Put right channel sample in output
        }
        live.addSample(output,bufferSize,nChannels);
    }
    
}

//--------------------------------------------------------------
void ofApp::audioIn(float * input, int bufferSize, int nChannels){
    if( inputMode == INPUT_LIVE && ofGetElapsedTimef() > 1 && tracks.isLoaded() ){
        live.addSample(input, bufferSize, nChannels);
    }
}


//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){
    if( key == ' '){
        doFind = !doFind;
    }
    if( key == 'p'){
        peng();
    }
    if( key == 'f'){
        ofToggleFullscreen();
    }
//    if( key == '1'){
//        finder.setMode( T_TM_SQDIFF );
//    }else if( key == '2'){
//        finder.setMode( T_TM_SQDIFF_NORMED );
//    }else if( key == '3'){
//        finder.setMode( T_TM_CCORR );
//    }else if( key == '4'){
//        finder.setMode( T_TM_CCORR_NORMED );
//    }else if( key == '5'){
//        finder.setMode( T_TM_CCOEFF );
//    }else if( key == '6'){
//        finder.setMode( T_TM_CCOEFF_NORMED );
//        

    if( key == '1'){
        loadTrack( 0 );
    }else if( key == '2'){
        loadTrack( 1 );
    }else if( key == '3'){
        loadTrack( 2 );
    }else if( key == '4'){
        loadTrack( 3 );
    }else if( key == '5'){
        loadTrack( 4 );
    }else if( key == '6'){
        loadTrack( 5 );
    }else if( key == '7'){
        loadTrack( 6 );
    }else if( key == OF_KEY_RIGHT ){
        tracks.gotoNextMeasure();
    }else if( key == OF_KEY_LEFT ){
        tracks.gotoPrevMeasure();
    }else if( key == OF_KEY_DOWN ){
        tracks.gotoNextPage();
    }else if( key == OF_KEY_UP ){
        tracks.gotoPrevPage();
    }

    ofLog() << " KEY : " << key;
}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y){
    
}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){
    
}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){
    
}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){
    
}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){
    
}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){
    
}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){
    
}

//--------------------------------------------------------------
void ofApp::peng(){
    
    
    cv::Mat tempImgMat = live.getMat();
    
    ofxCvGrayscaleImage tempImgCv;
    //        resultImgCv.allocate(tempImgMat.cols, tempImgMat.rows);
    //        IplImage ipl_img = tempImgMat;
    
    tempImgCv.setFromPixels((unsigned char*)tempImgMat.data, tempImgMat.cols, tempImgMat.rows);
    
    //    memcpy(resultImgCv.getCvImage()->imageData, ipl_img.imageData, ipl_img.imageSize);
    
    //    resultImgCv.setFromPixels( ipl_img.imageData, resultImgCv.getWidth(), resultImgCv.getHeight());
    
    ofImage imgResult;
    imgResult.setFromPixels( tempImgCv.getPixelsRef() );
    imgResult.saveImage("live-window-image_v2.bmp");

}

//
//
//
//void ofApp::temp(){
//    ofDirectory dir("./tracks");
//    dir.listDir();
//    vector<ofFile> folder = dir.getFiles();
//    for( int i=0;i<folder.size();i++ ){
//        ofLog() << "ITEMS : " << folder[i].getAbsolutePath() << "    " << folder[i].isDirectory();
//        if( folder[i].isDirectory() ){
//            Track* track = new Track ();
//            //    tracks.push_back( &track );
//            
//            //    tracks.push_back( Track() );
//            //    int id = tracks.size()-1;
//            for( int i=0;i<folder.size();i++ ){
//                if( folder[i].isFile() ){
//                    string ext = folder[i].getExtension();
//                    if( ext == "xml"){
//                        ofLogVerbose("TracksHandler") << " MusicXML present";
//                        track->musicXml.load( folder[i].getAbsolutePath() );
//                        track->hasMusicXML = true;
//                    }else if( ext == "wav"){
//                        ofLogVerbose("TracksHandler") << " Wave file present";
//                    }else if( ext == ".dat"){
//                        ofLogVerbose("TracksHandler") << " Annotations file present";
////                        hasColourAnnotations = true;
//                    }else{
//                        ofLogVerbose("TracksHandler") << "Strange file : " << ext << " : " << folder[i].getFileName();
//                    }
//                }
//            }
//            ofLogVerbose("TracksHandler") << "--------------------------------------";
//            
//            tracks.push_back(track);
//            
////            if( track->hasMusicXML ){
////                processTrack( track );
////            }
//        }
//        
////        ofLog() << " TRACKS : " << tracks.size();
//        
//        
//    }
//
//}