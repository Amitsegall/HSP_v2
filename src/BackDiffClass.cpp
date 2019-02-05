//
//  BackDiffClass.cpp
//  classCVtools
//
//  Created by Amit Segall on 23/03/2018.
//
//

#include "BackDiffClass.h"


using namespace ofxCv;
using namespace cv;


void BackDiff::setup(){
    
    //openCV
    colorImg.allocate(1280,800);
    grayImage.allocate(1280,800);
    grayDiff.allocate(1280,800);
    grayBg.allocate(1280,800);
    blurred.allocate(1280,800);
    
    // setup for ofxCV
    contourFinder2.setMinAreaRadius(35);
    contourFinder2.setMaxAreaRadius(65);
    contourFinder2.setThreshold(165);
    
    // wait for half a frame before forgetting something
    contourFinder2.getTracker().setPersistence(40);
    // an object can move up to 50 pixels per frame
    contourFinder2.getTracker().setMaximumDistance(400);
    
}

//-----------------------------------------------------

void BackDiff::update(ofFbo myImage, int threshold, int minArea, int maxArea){
    
    RectTracker& tracker = contourFinder2.getTracker();
    
    ofPixels locPix;
    myImage.readToPixels(locPix);
  
    colorImg.setFromPixels(locPix);
    grayImage = colorImg;
    
    //Smoothing image
    blurred = grayImage;
    blurred.blurGaussian(1);
    
    //Store first frame to background image
    if ( !grayBg.bAllocated ) {
        grayBg = blurred;
    }
    
    // take the abs value of the difference between background and incoming and then threshold:
    grayDiff.absDiff(grayBg, blurred);
    grayDiff.threshold(threshold);
    
    contourFinder.findContours(grayDiff, minArea, maxArea, 20, false);// find holes
    
    //for id tracking
    contourFinder2.findContours(grayDiff);

    // for allocating location and id each time.
    myIds.resize(contourFinder2.size());
    myLocs.resize(contourFinder2.size());
    myArea.resize(contourFinder2.size());
    for(int i = 0; i < contourFinder2.size(); i++) {
        
        ofPoint center = toOf(contourFinder2.getCenter(i));
        int label = contourFinder2.getLabel(i);
        int area = contourFinder2.getContourArea(i);
        myIds[i] = label;
        myLocs[i] = center;
        myArea[i] = area;
    }
    
}

//-----------------------------------------------------

void BackDiff::draw(){
    
    contourFinder.draw();
    
    // drawing the ID
//    for (int i = 0; i < contourFinder2.size();i++){
//        ofPushStyle();
//        ofSetColor(255,0,0);
//
//        ofDrawBitmapString(myIds[i],myLocs[i].x,myLocs[i].y);
//        ofPopStyle();
//    }

}

//-----------------------------------------------------

void BackDiff::setBack(){
    grayBg = blurred;
}
