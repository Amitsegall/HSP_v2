//
//  BackDiffClass.cpp
//  classCVtools
//
//  Created by Amit Segall on 23/03/2018.
//
//

#include "BackDiffClass.h"


void BackDiff::setup(){
    
    //openCV
    colorImg.allocate(1280,800);
    grayImage.allocate(1280,800);
    grayDiff.allocate(1280,800);
    grayBg.allocate(1280,800);
    blurred.allocate(1280,800);
    
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
    
    contourFinder.findContours(grayDiff, minArea, maxArea, 20, false);// find holes
    
}

//-----------------------------------------------------

void BackDiff::draw(){
    
    contourFinder.draw();
    
    

}

//-----------------------------------------------------

void BackDiff::setBack(){
    grayBg = blurred;
}
