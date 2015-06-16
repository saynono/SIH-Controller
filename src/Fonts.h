//
//  Fonts.h
//  PianoFingerprint
//
//  Created by say nono on 13.06.15.
//  Copyright (c) 2015 __MyCompanyName__. All rights reserved.
//

#pragma once

#include "ofMain.h"

class Fonts{

public:

    
    static Fonts& one(){
        static Fonts instance; // Instantiated on first use.
        return instance;
    }
    
    Fonts();
    
	void setup();

    ofTrueTypeFont fontRobotoLight8;
    ofTrueTypeFont fontRobotoRegular8;
    ofTrueTypeFont fontRobotoBold8;
    ofTrueTypeFont fontRobotoLight12;
    ofTrueTypeFont fontRobotoRegular12;
    ofTrueTypeFont fontRobotoBold12;
    ofTrueTypeFont fontRobotoLight30;
    ofTrueTypeFont fontRobotoRegular30;
    ofTrueTypeFont fontRobotoBold30;
    
};





#ifndef _H_SINGLETON
#define _H_SINGLETON
#include "assert.h"
#include <cstdlib>

template <class T>
class Singleton {
public:
    static T* Instance() {
        if(!m_pInstance) m_pInstance = new T;
        assert(m_pInstance !=NULL);
        return m_pInstance;
    }
protected:
    Singleton();
    ~Singleton();
private:
    Singleton(Singleton const&);
    Singleton& operator=(Singleton const&);
    static T* m_pInstance;
};
template <class T> T* Singleton<T>::m_pInstance=NULL;
#endif

