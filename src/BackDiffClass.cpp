//
//  BackDiffClass.cpp
//  classCVtools
//
//  Created by Amit Segall on 23/03/2018.
//
//

#include "BackDiffClass.h"

//----------------------------------------------------- // a object to find the ID of a blob over time

using namespace ofxCv;
using namespace cv;

const float dyingTime = 0.25;

void Glow::setup(const cv::Rect& track) {
    color.setHsb(ofRandom(0, 255), 255, 255);
    cur = toOf(track).getCenter();
    smooth = cur;
}

void Glow::update(const cv::Rect& track) {
    cur = toOf(track).getCenter();
    smooth.interpolate(cur, .5);
    all.addVertex(smooth.x,smooth.y);
}

void Glow::kill() {
    float curTime = ofGetElapsedTimef();
    if(startedDying == 0) {
        startedDying = curTime;
    } else if(curTime - startedDying > dyingTime) {
        dead = true;
    }
}

int Glow::retLabel(){
    return label;
}

ofPoint Glow::retLoc(){
    return cur;
}

//-----------------------------------------------------

void BackDiff::setup(){
    
    //openCV
    colorImg.allocate(1280, 800);
    grayImage.allocate(1280, 800);
    grayDiff.allocate(1280, 800);
    grayBg.allocate(1280, 800);
    
    mask.allocate(1280, 800);
    blurred.allocate(1280, 800);
    
    // wait for half a frame before forgetting something
    tracker.setPersistence(40);
    // an object can move up to 50 pixels per frame
    tracker.setMaximumDistance(400);

    
}

//-----------------------------------------------------

void BackDiff::update(ofFbo myImage, int threshold, int minArea, int maxArea){
    
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
    
    //Thresholding for obtaining binary image
    mask = grayDiff;
    mask.threshold( threshold) ; //set the  Threshold - very important

//
    contourFinder.findContours(mask, minArea, maxArea, 20, false);// find holes
    
    
    //ofxCV
    
    contourFinder2.findContours(mask);
    tracker.track(contourFinder2.getBoundingRects());

    
    vector<Glow>& followers = tracker.getFollowers();
    myIds.resize(followers.size());
    myLocs.resize(followers.size());
    for(int i = 0; i < followers.size(); i++) {
        myIds[i] = i;
        myLocs[i] = followers[i].retLoc();
    }

    
}

//-----------------------------------------------------

void BackDiff::draw(){
    
    contourFinder.draw();
    

}

//-----------------------------------------------------

void BackDiff::setBack(){
    grayBg = blurred;
}
