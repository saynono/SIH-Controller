//
//  FingerprintSource.h
//  PianoFingerprint
//
//  Created by say nono on 30.05.15.
//  Copyright (c) 2015 __MyCompanyName__. All rights reserved.
//

#pragma once

#include "ofMain.h"
#include "ofxMaxim.h"
#include "ofxCv.h"

class FingerprintSource : public ofThread{

public:
	
	void setup( int sampleRate, int bufferSize, int fftSize );
    void exit();
    bool createFingerprint( string path );
    void saveFingerprint( string path );
    bool loadFingerprint( string path );
    void draw();
    void draw( int w, int h );
	void draw( int x, int y, int w, int h );
    
    const cv::Mat& getMat();

private:
    
    void threadedFunction();
    
    string dataPathSound;
    string dataPathMat;
    
    int sampleRate;
    int bufferSize;
    int fftSize;
    
    bool doUpdateImage;
    bool isLoaded;

    ofImage                         image;
    ofImage                         imageResized;
    cv::Mat                         mat;

    ofxMaxiSample                   samplePlayer;

    ofxMaxiFFT                      fftAnalyzer;
    ofxMaxiFFTOctaveAnalyzer        octAnalyzer;
	
};



