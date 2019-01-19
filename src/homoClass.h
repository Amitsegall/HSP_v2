//
//  homoClass.hpp
//  homographyClass
//
//  Created by Amit Segall on 08/04/2018.
//  This is a homography class I wrote based on Kyle McDonald's ofxCV addon and example
//  I made this work with video dynamically and smooth and I'm very happy with this. 

#ifndef homoClass_hpp
#define homoClass_hpp

#include <stdio.h>
#include "ofMain.h"
#include "ofxCv.h"

class HomoClass {
    
public:
    
   
    void setup(string leftimg, string rightimg);
    void update(ofFbo vid);
    void draw();
    
    bool movePoint(vector<ofVec2f>& points, ofVec2f point);
    void mousePressed(int x, int y, int button);
    void mouseDragged(int x, int y, int button);
    void mouseReleased(int x, int y, int button);
    void saveHomo();


    // variables
    
    ofImage left, right, warpedColor;
    vector<ofVec2f> leftPoints, rightPoints;
    bool movingPoint;
    ofVec2f* curPoint;
    bool saveMatrix;
    bool homographyReady;
    
    
    
    cv::Mat homography;
    
private:
}; 

#endif /* homoClass_hpp */
