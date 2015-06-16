//
//  FingerprintSource.cpp
//  PianoFingerprint
//
//  Created by say nono on 30.05.15.
//  Copyright (c) 2015 __MyCompanyName__. All rights reserved.
//

#include "FingerprintSource.h"

	
void FingerprintSource::setup( int s, int b, int f ){
    sampleRate 	= s;//44100; /* Sampling Rate */
    bufferSize	= b;//512; /* Buffer Size. you have to fill this buffer with sound using the for loop in the audioOut method */
    fftSize = f;//1024;
    doUpdateImage = false;
    isLoaded = false;
    mat = cv::Mat::ones( 20, 20, CV_32FC1 );

}

void FingerprintSource::exit(){
    ofThread::stopThread();
}

bool FingerprintSource::createFingerprint( string path ){
    
    if( !ofFile(dataPathSound).exists() ) return false;
    
    dataPathSound = path;
    string pathDir = ofFile(dataPathSound).getEnclosingDirectory();
    dataPathMat = pathDir + "/fingerprint.png";
    

    fftAnalyzer.setup(fftSize, bufferSize, bufferSize/2);
    octAnalyzer.setup(sampleRate, fftSize, 12);
    
    samplePlayer.load( ofToDataPath(path) );
    
    int samplesLength = samplePlayer.length/bufferSize;
    image.allocate( octAnalyzer.nAverages, samplesLength, OF_IMAGE_GRAYSCALE);
    memset(image.getPixels(), 0, (int) (image.getWidth() * image.getHeight()) );
    image.update();
    
    imageResized.allocate(104, 1024, OF_IMAGE_GRAYSCALE);
    memset(imageResized.getPixels(), 0, (int) (imageResized.getWidth() * imageResized.getHeight()) );
    imageResized.update();
    
//    mat.create( (int)image.getHeight(), (int)image.getWidth(), CV_32F );
    mat = cv::Mat::zeros( (int)image.getHeight(), (int)image.getWidth(), CV_8U );
    
    // start the process here!
    ofThread::startThread(true);
    return true;
}



void FingerprintSource::saveFingerprint( string path ){
    if( image.isAllocated() ){
        image.saveImage( path );
    }

    cv::Mat matResized;
    cv::resize(mat, matResized, cv::Size(104,1024));
//    imageResized.allocate(104, 1024, OF_IMAGE_GRAYSCALE);
    ofxCv::toOf(matResized, imageResized.getPixelsRef());
//    imageResized.update();
    string pathDir = ofFile(path).getEnclosingDirectory();
    imageResized.saveImage( pathDir + "/fingerprint_preview.png" );

    
//    ofImage img;
//    img.allocate( mat.cols, mat.rows, OF_IMAGE_GRAYSCALE );
//    ofxCv::toOf( mat, img.getPixelsRef() );
//    img.saveImage( path );
//    ofxCv::saveMat( mat, ofToDataPath( path ));
}


bool FingerprintSource::loadFingerprint( string path ){
    
    if( !ofFile(path).exists() ) return false;
    dataPathMat = path;
    image.loadImage( path );

    mat = ofxCv::toCv( image.getPixelsRef() );
    
    string pathDir = ofFile(path).getEnclosingDirectory();
    ofFile preview(pathDir + "/fingerprint_preview.png");
    if( preview.exists() ){
        imageResized.loadImage( preview );
    }else{
        cv::Mat matResized;
        cv::resize(mat, matResized, cv::Size(104,1024));
        imageResized.allocate(104, 1024, OF_IMAGE_GRAYSCALE);
        ofxCv::toOf(matResized, imageResized.getPixelsRef());
        imageResized.update();
        imageResized.saveImage( preview );
    }
    isLoaded = true;    
    return true;
}


void FingerprintSource::threadedFunction() {
    bool isFinished = false;
    int frame = 0;
    double sample;
    
//    int len = samplePlayer.getLength();
    unsigned char* pixels = image.getPixels();
    int spectrogramWidth = (int) image.getHeight();
    int n = (int) image.getWidth();

    while( isThreadRunning() & !isFinished) {
        
        for( int i=0;i<bufferSize;i++ ){
            // Get audio samples
            sample = samplePlayer.playOnce();		// Get left channel sample
            
            // Process left channel
            if (fftAnalyzer.process(sample)) {
                fftAnalyzer.magsToDB();
                octAnalyzer.calculate(fftAnalyzer.magnitudes);
            }
        }
        
        int row = min(mat.rows,frame);
        for(int i = 0; i < n; i++) {
            int j = i + frame * n;
            ////        int logi = ofMap(powFreq(i), powFreq(0), powFreq(n), 0, n);
            int val = octAnalyzer.averages[i] / 20.0 * 255;
            val += 10;
            pixels[j] = (unsigned char)(min(255,val));
//            mat.at<double>(i, row) = octAnalyzer.averages[i] * 200.0f;
            mat.at<unsigned char>(row,i) = (unsigned char)(min(255,val));
            
        }

        if( samplePlayer.position >= samplePlayer.length ) isFinished = true;
        frame++;
        
    }
    saveFingerprint( dataPathMat );
    doUpdateImage = true;
    isLoaded = true;
}

void FingerprintSource::draw(){
    draw( 0,0,imageResized.getWidth(),imageResized.getHeight());
}

void FingerprintSource::draw( int w, int h ){
    draw( 0, 0, w, h );
}

void FingerprintSource::draw( int x, int y, int w, int h ){

    if( isLoaded && !ofThread::isThreadRunning() ){
        if( doUpdateImage ){
            doUpdateImage = false;
            imageResized.update();
//            mat = ofxCv::toCv( image );
//            mat.convertTo( mat, CV_32F, 0.0f, 1.0f );
        }
//        ofxCv::drawMat(mat, x, y, w, h);
//        ofxCv::drawMat(mat, x, y);
        imageResized.draw(x,y,w,h);
        ofLog() << "Draw!";
    }
}

const cv::Mat& FingerprintSource::getMat(){
    return mat;
}