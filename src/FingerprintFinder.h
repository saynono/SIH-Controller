//
//  FingerprintFinder.h
//  PianoFingerprint
//
//  Created by say nono on 30.05.15.
//  Copyright (c) 2015 __MyCompanyName__. All rights reserved.
//

#pragma once

#include "ofMain.h"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "ofxCv.h"

enum TEMPLATE_MODES {
    T_TM_SQDIFF=0,
    T_TM_SQDIFF_NORMED=1,
    T_TM_CCORR=2,
    T_TM_CCORR_NORMED=3,
    T_TM_CCOEFF=4,
    T_TM_CCOEFF_NORMED=5
};

class FingerprintFinder{

public:
	
	void setup();
    void exit();
    void find( cv::Mat original, cv::Mat tofind );
    void draw();
    void draw( int w, int h );
    void draw( int x, int y, int w, int h );
    
    void setMode( TEMPLATE_MODES mode );
    
private:
    
    void threadedFunction();
    
    /// Global Variables
//    cv::Mat img; cv::Mat templ;
    cv::Mat result;
    cv::Mat resultSmoothend;
    int match_method;
    int max_Trackbar = 5;
    cv::Point matchLocation;
    cv::Point matchSize;
    
    vector<float> cofidenceLevels;
    
};



