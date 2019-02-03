//
//  BackDiffClass.hpp
//  classCVtools
//
//  Created by Amit Segall on 23/03/2018.
//
//

#ifndef BackDiffClass_hpp
#define BackDiffClass_hpp

#include <stdio.h>
#include "ofMain.h"
#include "ofxOpenCv.h"
#include "ofxCv.h"


class BackDiff {
    
    public:
    void setup();
    void update(ofFbo myImage,int threshold, int minArea, int maxArea);
    void draw();
    void setBack();
    
    
    ofxCvColorImage			colorImg;
    ofxCvGrayscaleImage 	grayImage;
    ofxCvGrayscaleImage 	grayBg;
    ofxCvGrayscaleImage 	grayDiff;
    ofxCvGrayscaleImage     blurred;

    
    ofxCvContourFinder 	contourFinder;
    ofImage myImage;
    
    // ofxCV - for ID tracking
    ofxCv::ContourFinder contourFinder2;

    
    //storing ID and location
    vector <int> myIds,myArea;
    vector <ofPoint> myLocs;
    
};




#endif /* BackDiffClass_hpp */
