#pragma once

#include "ofMain.h"
#include "ofxMaxim.h"
#include "FingerprintSource.h"
#include "FingerprintLive.h"
#include "FingerprintFinder.h"
#include "TracksHandler.h"
#include "KeyDefinitions.h"
#include "LightControl.h"

//#include "Track.h"


enum {
    INPUT_LIVE,
    INPUT_SAMPLE
};

class ofApp : public ofBaseApp{
	
    public:
    void setup();
    void exit();
    void loadTrack( int id );
    void update();
    void draw();
    void drawFFT( int x, int y, int w, int h);
    void drawSource( int x, int y, int w, int h);
    void drawOctaves( int x, int y, int w, int h);
    void drawSpectrum( int x, int y, int w, int h);
    void drawResult( int x, int y, int w, int h);
    
    void keyPressed(int key);
    void keyReleased(int key);
    void mouseMoved(int x, int y);
    void mouseDragged(int x, int y, int button);
    void mousePressed(int x, int y, int button);
    void mouseReleased(int x, int y, int button);
    void windowResized(int w, int h);
    void dragEvent(ofDragInfo dragInfo);
    void gotMessage(ofMessage msg);
    
    void audioOut(float * output, int bufferSize, int nChannels);
    void audioIn(float * input, int bufferSize, int nChannels);
    
    void peng();
	
    float	bufferDuration;
	int		bufferSize;
	int		sampleRate;
    int     fftSize;
	
    int frameWidth;
    int frameHeight;
    
    bool doFind;
    bool inputMode;
    
    ofxMaxiSample samplePlayer;
    ofSoundStream soundStream;

    
    FingerprintSource   source;
    FingerprintLive     live;
    FingerprintFinder   finder;
    
    KeyDefinitions      keys;
    TracksHandler       tracks;
    LightControl        lights;
    
};


