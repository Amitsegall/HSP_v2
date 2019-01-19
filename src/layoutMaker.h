//
//  fromImgToLayoutClass.hpp
//
//  Created by Amit Segall on 29/07/2018.
//
//

#ifndef fromImgToLayoutClass_hpp
#define fromImgToLayoutClass_hpp

#include <stdio.h>
#include "ofMain.h"
#include "ofxOpenCv.h"

class LayoutMaker {
    
public:
    
    void setup(int width,int height);
    
    
    // makeShapes
    vector<ofPolyline> myShapes;
    void randLayout(int shapes);
    void cleanShapes();
    
    
    //cv things
    ofxCvColorImage			colorImg;
    ofxCvGrayscaleImage 	grayImage;
    ofxCvGrayscaleImage 	grayBg;
    ofxCvGrayscaleImage 	grayDiff;
    ofxCvContourFinder 	contourFinder;
    ofImage myImage;
    int threshold;
    void findShapesInImage(ofImage myImage);
    void makeShapesFromBlobs();

    //file management
    ofDirectory dir;
    vector<ofImage> images;    
    int currentImage;
    void readImgDir();
    

};




#endif /* fromImgToLayoutClass_hpp */
