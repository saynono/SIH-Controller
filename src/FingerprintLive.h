//
//  FingerprintLive.h
//  PianoFingerprint
//
//  Created by say nono on 30.05.15.
//  Copyright (c) 2015 __MyCompanyName__. All rights reserved.
//

#pragma once

#include "ofMain.h"
#include "ofxMaxim.h"
#include "ofxCv.h"
#include "Frequencies.h"


class FingerprintLive : public ofThread {

    
    public:
        
        void setup( float bufferDuration, int sampleRate, int bufferSize, int fftSize );
        void exit();
        void saveFingerprint( string path );
        void loadFingerprint( string path );
        void addSample( float * output, int bufferSize, int nChannels);
        void draw();
        void draw( int w, int h );
        void draw( int x, int y, int w, int h );
        void drawOctaves( int x, int y, int w, int h );
        void drawFFT( int x, int y, int w, int h );
        void drawPhases( int x, int y, int w, int h );
        const cv::Mat& getMat();
    
    private:
        
        void setupFingerprint();
    
    void non_maxima_suppression(const cv::Mat& src, cv::Mat& mask, const bool remove_plateaus);
    void findPeaks(cv::InputArray _src, cv::OutputArray _idx, const float scale = 0.2, const cv::Size& ksize = cv::Size(9, 9), const bool remove_plateus = true);
    void findNonZero( cv::InputArray mask, vector<cv::Point>* maxima );
    
    cv::Mat hist;
    vector<cv::Point> peaks;
    cv::Mat spectrogramMat;
    cv::Point maxPeak;

        string dataPath;
        int sampleRate;
        int bufferSize;
        int fftSize;
        float bufferDuration;
        int samplesLength;
        int currentFrame;
        bool doUpdateImage;
        
        ofImage                         image;
        cv::Mat                         mat;

//        cv::Mat                         spectrogramMat;
    
        ofxMaxiFFT                      fftAnalyzer;
        ofxMaxiFFTOctaveAnalyzer        octAnalyzer;
        
    };