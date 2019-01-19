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


class Glow : public ofxCv::RectFollower { // helps finding blob id over time
protected:
    ofColor color;
    ofVec2f cur, smooth;
    float startedDying;
    ofPolyline all;
public:
    Glow()
    :startedDying(0) {
    }
    void setup(const cv::Rect& track);
    void update(const cv::Rect& track);
    void kill();
    int retLabel();
    ofPoint retLoc();
};


class BackDiff {
    
    public:
    void setup();
    void update(ofFbo myImage,int threshold,int blurVal, int minArea, int maxArea);
    void draw();
    void setBack();
    
    
    ofxCvColorImage			colorImg;
    ofxCvGrayscaleImage 	grayImage;
    ofxCvGrayscaleImage 	grayBg;
    ofxCvGrayscaleImage 	grayDiff;
    ofxCvGrayscaleImage     mask;
    ofxCvGrayscaleImage     blurred;

    
    ofxCvContourFinder 	contourFinder;
    ofImage myImage;

    
    // ofxCV
    ofxCv::ContourFinder contourFinder2;
    ofxCv::RectTrackerFollower<Glow> tracker;
    
    
    vector <int> myIds;
    vector <ofPoint> myLocs;
    
    
};




#endif /* BackDiffClass_hpp */
