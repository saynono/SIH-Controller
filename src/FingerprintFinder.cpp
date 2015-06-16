//
//  FingerprintFinder.cpp
//  PianoFingerprint
//
//  Created by say nono on 30.05.15.
//  Copyright (c) 2015 __MyCompanyName__. All rights reserved.
//
//
// All from here:
// http://docs.opencv.org/doc/tutorials/imgproc/histograms/template_matching/template_matching.html
//
//

#include "FingerprintFinder.h"

using namespace cv;

void FingerprintFinder::setup(){
    //    enum { TM_SQDIFF=0, TM_SQDIFF_NORMED=1, TM_CCORR=2, TM_CCORR_NORMED=3, TM_CCOEFF=4, TM_CCOEFF_NORMED=5 };
    match_method = TM_CCOEFF_NORMED;
}

void FingerprintFinder::exit(){
//    ofThread::stopThread();
}

void FingerprintFinder::find( Mat original, Mat tofind ){
    /// Do the Matching and Normalize
    
    
    /// Create the result matrix
    int result_cols =  original.cols - tofind.cols + 1;
    int result_rows = original.rows - tofind.rows + 1;
    
    if( result.cols != result_cols || result.rows != result_rows ){
        result = cv::Mat::zeros(result_cols, result_rows, CV_8U);
        resultSmoothend = cv::Mat::zeros(result_cols, result_rows, CV_8U);
    }
    
    matchTemplate( original, tofind, result, match_method );
    normalize( result, result, 0, 1, NORM_MINMAX, -1, Mat() );
  
    for( int i=0;i<result.total();i++ ){
        resultSmoothend.data[i] = (result.data[i]*1 + resultSmoothend.data[i]*9 ) / 10;
    }
    
//    cout << "------------------------------------" << std::endl;
//    cout << " RESULT : " << std::endl << result << std::endl;
//    cout << "------------------------------------" << std::endl;
    
    /// Localizing the best match with minMaxLoc
    double minVal; double maxVal; cv::Point minLoc; cv::Point maxLoc;
//    cv::Point matchLoc;
    
    minMaxLoc( result, &minVal, &maxVal, &minLoc, &maxLoc, Mat() );
    
    matchSize.x = tofind.cols;
    matchSize.y = tofind.rows;
    
    /// For SQDIFF and SQDIFF_NORMED, the best matches are lower values. For all the other methods, the higher the better
    if( match_method  == CV_TM_SQDIFF || match_method == CV_TM_SQDIFF_NORMED )
    { matchLocation = minLoc; }
    else
    { matchLocation = maxLoc; }
    
//    ofLog() << " Confidence : " << (int)result.at<unsigned char>(matchLocation.x,matchLocation.y);
    
    cofidenceLevels.push_back( result.at<unsigned char>(matchLocation.x,matchLocation.y)/255.0f );
    while (cofidenceLevels.size() > 1000) {
        cofidenceLevels.erase( cofidenceLevels.begin());
    }
    
}

void FingerprintFinder::draw(){
//    draw( 0,0, result.cols, result.rows );
}

void FingerprintFinder::draw( int w, int h ){
    draw( 0, 0, w, h );
}

void FingerprintFinder::draw( int x, int y, int w, int h){
//    ofxCv::drawMat( result, x, y);
    ofPushMatrix();
    ofPushStyle();
    if( result.total() > 0){
        int val = (result.at<unsigned char>(matchLocation.x,matchLocation.y) /2.55f);
        ofSetColor(255, 0, 0);
        ofDrawBitmapString(" CONFIDENCE: " + ofToString(val)+"%", 10, 14);
        
        string str = "MODE: ";
        if( match_method == 0){
            str += "TM_SQDIFF";
        }else if( match_method == 1){
            str += "SQDIFF_NORMED";
        }else if( match_method == 2){
            str += "CCORR";
        }else if( match_method == 3){
            str += "CCORR_NORMED";
        }else if( match_method == 4){
            str += "CCOEFF";
        }else if( match_method == 5){
            str += "CCOEFF_NORMED";
        }
        ofDrawBitmapString(str, 150, 14);
    }
    ofNoFill();
    ofSetColor(255, 0, 0);
    ofScale( (float)w/(float)result.rows, 1.f );
    ofRect( matchLocation.y, matchLocation.x, matchSize.y, matchSize.x );
    for( int i=0;i<result.rows;i++ ){
        ofLine(i, h, i, h-resultSmoothend.at<unsigned char>(0,i)/10);
    }
    
    ofPopMatrix();

    ofSetColor(0, 255, 0);
    ofScale( (float)w/(float)result.rows, 1.f );
    ofBeginShape();
    for( int i=0;i<cofidenceLevels.size();i++ ){
        ofVertex( i, h - cofidenceLevels[i] * h );
    }
    ofEndShape();
    
    
    ofPopStyle();
//    /// Show me what you got
//    rectangle( img_display, matchLoc, Point( matchLoc.x + templ.cols , matchLoc.y + templ.rows ), Scalar::all(0), 2, 8, 0 );
//    rectangle( result, matchLoc, Point( matchLoc.x + templ.cols , matchLoc.y + templ.rows ), Scalar::all(0), 2, 8, 0 );

}

void FingerprintFinder::threadedFunction(){
}

void FingerprintFinder::setMode( TEMPLATE_MODES mode ){
    match_method = mode;
}


