//
//  FingerprintLive.cpp
//  PianoFingerprint
//
//  Created by say nono on 30.05.15.
//  Copyright (c) 2015 __MyCompanyName__. All rights reserved.
//

#include "FingerprintLive.h"

#include "shift.hpp"


void FingerprintLive::setup( float bf, int s, int b, int f ){
    bufferDuration = bf;
    sampleRate 	= s;//44100; /* Sampling Rate */
    bufferSize	= b;//512; /* Buffer Size. you have to fill this buffer with sound using the for loop in the audioOut method */
    fftSize = f;//1024;
    currentFrame = 0;
    doUpdateImage = false;
    setupFingerprint();
}

void FingerprintLive::exit(){
    ofThread::stopThread();
}

void FingerprintLive::setupFingerprint(){
    
    samplesLength = (sampleRate * bufferDuration)/bufferSize;
    fftAnalyzer.setup(fftSize, bufferSize, bufferSize/2);
    octAnalyzer.setup(sampleRate, fftSize, 12);
    image.allocate( octAnalyzer.nAverages, samplesLength, OF_IMAGE_GRAYSCALE);
    memset(image.getPixels(), 0, (int) (image.getWidth() * image.getHeight()) );
    image.update();
//    mat.create( image.getWidth(), image.getHeight(), CV_32F );
    mat = cv::Mat::zeros( (int)image.getHeight(), (int)image.getWidth(), CV_8U );
    
//    spectrogramMat = cv::Mat::zeros( 1, (int)image.getWidth(), CV_8U );
    spectrogramMat = cv::Mat((int)image.getWidth(), 1, cv::DataType<float>::type );
//    ofThread::startThread(true);
}


void FingerprintLive::saveFingerprint( string path ){
    if( image.isAllocated() ){
        image.saveImage( path );
    }
}


void FingerprintLive::loadFingerprint( string path ){
    
}


void FingerprintLive::addSample( float * output, int bufferSize, int nChannels) {

    double sample;

    unsigned char* pixels = image.getPixels();
    int spectrogramWidth = (int) image.getHeight();
    int n = (int) image.getWidth();

    for( int i=0;i<bufferSize;i++ ){
        sample = output[nChannels*i];		// Get one channel sample
        if (fftAnalyzer.process(sample)) {
            fftAnalyzer.magsToDB();
            octAnalyzer.calculate(fftAnalyzer.magnitudes);
        }
    }
    
    if(currentFrame >= spectrogramWidth){
        memmove(pixels,pixels+n,(spectrogramWidth-1)*n);
        shift(mat,mat,cv::Point2f(0,-1));
    }
    
    int row = min(spectrogramWidth-1,currentFrame);
//    spectrogramMat.resize(0);
    for(int i = 0; i < n; i++) {
        int j = i + row * n; // TODO change this to push lines
        int val = octAnalyzer.averages[i] / 20.0 * 255;
        val += 10;
        spectrogramMat.at<float>(i, 0) = octAnalyzer.averages[i];
        pixels[j] = (unsigned char)(min(255,val));
        mat.at<unsigned char>(row,i) = (unsigned char)(min(255,val));
    }
    currentFrame++;
    doUpdateImage = true;

}

void FingerprintLive::draw(){
    draw( 0,0,image.getWidth(),image.getHeight());
}

void FingerprintLive::draw( int w, int h ){
    draw( 0, 0, w, h );
}

void FingerprintLive::draw( int x, int y, int w, int h ){
    if( !ofThread::isThreadRunning() ){
        if( doUpdateImage ){
            doUpdateImage = false;
            image.update();
            
//            ofLog() << "Peaks:  " << peaks.size();
            findPeaks(spectrogramMat, peaks,.5);
//            mat = ofxCv::toCv( image );
//            mat.convertTo( mat, CV_32F, 0.0f, 1.0f );
        }
        image.draw(x,y,w,h);
//        ofxCv::drawMat( mat, x, y );
    }
}

void FingerprintLive::drawOctaves( int x, int y, int w, int h ){
    ofPushStyle();
    ofPushMatrix();
    ofTranslate(x, y);
    //octave analyser
    float xinc = (float)w / octAnalyzer.nAverages;
    for(int i=0; i < octAnalyzer.nAverages; i++) {
        float height = octAnalyzer.averages[i] / 50.0 * h;
        ofRect((i*xinc),h - height,2, height);
    }
    
    ofSetColor(255, 0, 0);
    ofPushStyle();
    ofNoFill();
    for( int i=0;i<peaks.size();i++ ){
        float height = octAnalyzer.averages[ peaks[i].x ] / 50.0 * h;
        ofCircle( peaks[i].x*xinc, h - height, 4);
        ofRect((i*xinc),- height,2, height);
    }
    ofPopStyle();
    
    
    ofPopMatrix();
    ofPopStyle();
}

void FingerprintLive::drawFFT( int x, int y, int w, int h ){
    ofPushStyle();
    ofPushMatrix();
    ofTranslate(x, y);

    int l = fftSize / 16;
    float xinc = (float)w / l;

//    ofLog() << " PIANO_KEY_FREQUENCY : " << sizeof(PIANO_KEY_FREQUENCY)/sizeof(Float32);
    int lFreq = sizeof(PIANO_KEY_FREQUENCY)/sizeof(Float32);
    float px;
    ofSetColor(120);
    for( int i=0;i<lFreq;i++ ){
//        int x = xinc
//        freq = 2 * sampleRate / fftSize;
        px = xinc * PIANO_KEY_FREQUENCY[i] * fftSize / sampleRate;// * sampleRate / fftSize;
        ofLine(px,0,px,h);
    }
    
    for(int i=0; i < l; i++) {
        float height = fftAnalyzer.magnitudesDB[i] / 50.0 * h;
        height = max( 0.0f, height);
        ofRect((i*xinc),h - height,2, height);
    }
    
//    ofSetColor(255, 0, 0);
//    ofPushStyle();
//    ofNoFill();
////    for(vector<cv::Point>::iterator it = peaks.begin(); it != peaks.end();++it){
////    for(vector<cv::Point>::iterator it = peaks.begin(); it != peaks.end();++it){
//    for( int i=0;i<peaks.size();i++ ){
//        ofCircle( peaks[i].x/16, peaks[i].y, 4);
//    }
//    ofPopStyle();
    
    
    ofPopMatrix();
    ofPopStyle();
}

void FingerprintLive::drawPhases( int x, int y, int w, int h ){
        ofPushStyle();
        ofPushMatrix();
        ofTranslate(x, y);
        float xinc = (float)w / fftSize * 2.0;
        for(int i=0; i < fftSize / 2; i++) {
            float height = fftAnalyzer.phases[i] / 50.0 * h;
            ofRect((i*xinc),(h - height)/2,2, height);
        }
        ofPopMatrix();
        ofPopStyle();
}

const cv::Mat& FingerprintLive::getMat(){
    return mat;
}


void FingerprintLive::non_maxima_suppression(const cv::Mat& src, cv::Mat& mask, const bool remove_plateaus)
{
    // find pixels that are equal to the local neighborhood not maximum (including 'plateaus')
    cv::dilate(src, mask, cv::Mat());
    cv::compare(src, mask, mask, cv::CMP_GE);
    
    // optionally filter out pixels that are equal to the local minimum ('plateaus')
    if (remove_plateaus) {
        cv::Mat non_plateau_mask;
        cv::erode(src, non_plateau_mask, cv::Mat());
        cv::compare(src, non_plateau_mask, non_plateau_mask, cv::CMP_GT);
        cv::bitwise_and(mask, non_plateau_mask, mask);
    }
}

// function that finds the peaks of a given hist image
void FingerprintLive::findPeaks(cv::InputArray _src, cv::OutputArray _idx, const float scale, const cv::Size& ksize, const bool remove_plateus){
//    cv::Mat hist = _src.getMat();
    hist = _src.getMat();
    // die if histogram image is not the correct type
//    CV_Assert(hist.type() == CV_32F);
    
    // find the min and max values of the hist image
    double min_val, max_val;
    minMaxLoc(hist, &min_val, &max_val);
    
    maxPeak.x = max_val;
    
    cv::Mat mask;
    cv::GaussianBlur(hist, hist, ksize, 0); // smooth a bit in order to obtain better result
    non_maxima_suppression(hist, mask, remove_plateus); // extract local maxima
    
    vector<cv::Point> maxima;   // output, locations of non-zero pixels
    findNonZero(mask, &maxima);
    
    for(vector<cv::Point>::iterator it = maxima.begin(); it != maxima.end();)
    {
        cv::Point pnt = *it;
        unsigned char pVal = hist.at< unsigned char >(pnt.x-1/*,pnt.x*/);
        unsigned char val = hist.at< unsigned char >(pnt.x/*,pnt.x*/);
        unsigned char nVal = hist.at< unsigned char >(pnt.x+1/*,pnt.x*/);
        // filter peaks
        if((val > max_val * scale))
            ++it;
        else
            it = maxima.erase(it);
    }
    
    cv::Mat(maxima).copyTo(_idx);
    
}

void FingerprintLive::findNonZero( cv::InputArray mask, vector<cv::Point>* maxima ){
    cv::Mat mat = mask.getMat();
    int l = mat.rows*mat.cols;
    for( int i=0;i<l;i++ ){
        if( mat.at< unsigned char >(0,i) > 0 ) maxima->push_back( cv::Point(i,0) );
    }
}

