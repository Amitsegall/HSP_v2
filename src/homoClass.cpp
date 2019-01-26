//
//  homoClass.cpp
//  homographyClass
//
//  Created by Amit Segall on 08/04/2018.
//
//

#include "homoClass.h"

using namespace ofxCv;
using namespace cv;



void HomoClass::setup(string leftimg, string rightimg){
    
//    ofSetVerticalSync(true); //turned off in hope to make process faster!
    
    left.load(leftimg);
    right.load(rightimg);

    
    movingPoint = false;
    saveMatrix = false;
    homographyReady = false;
    
    // load the previous homography if it's available
    ofFile previous("homography.yml");
    if(previous.exists()) {
        FileStorage fs(ofToDataPath("homography.yml"), FileStorage::READ);
        fs["homography"] >> homography;
        homographyReady = true;
        
        ofToggleFullscreen(); // if loading an exsiting setup
        
    }
    
}

//_____________________________________________

void HomoClass::update(ofImage camImage){
    
    // get the pixels from the video
    imitate(warpedColor, camImage); // importing the current image / graphics / live camera
    
    if(leftPoints.size() >= 4) {
        vector<Point2f> srcPoints, dstPoints;
        for(int i = 0; i < leftPoints.size(); i++) {
            srcPoints.push_back(Point2f(rightPoints[i].x - left.getWidth(), rightPoints[i].y));
            dstPoints.push_back(Point2f(leftPoints[i].x, leftPoints[i].y));
        }
        
        // generate a homography from the two sets of points
        homography = findHomography(Mat(srcPoints), Mat(dstPoints));
        
        if(saveMatrix) {
            FileStorage fs(ofToDataPath("homography.yml"), FileStorage::WRITE);
            fs << "homography" << homography;
            saveMatrix = false;
        }
    }
    
    
    if(homographyReady) {
        
        // this is how you warp one ofImage into another ofImage given the homography matrix
        
        warpPerspective(camImage, warpedColor, homography, CV_INTER_LINEAR);// CV INTER NN is 113 fps, CV_INTER_LINEAR is 93 fps
        warpedColor.update(); // transform the warped image in realtime.
        
    }

    
}

//_____________________________________________

void drawPoints(vector<ofVec2f>& points) {
    ofNoFill();
    for(int i = 0; i < points.size(); i++) {
        ofDrawCircle(points[i], 10);
        ofDrawCircle(points[i], 1);
    }
}

//_____________________________________________

void HomoClass::draw(){
    

    if(homographyReady) {
        
        warpedColor.draw(0, 0,ofGetWidth(),ofGetHeight()); // drawing and resizing at the same time

    }else {
        // while setting up the homography
        ofPushStyle();
        ofSetColor(255);
        left.draw(0, 0);
        right.draw(left.getWidth(), 0);
        ofSetColor(ofColor::red);
        drawPoints(leftPoints);
        ofSetColor(ofColor::blue);
        drawPoints(rightPoints);
        ofSetColor(128);
        for(int i = 0; i < leftPoints.size(); i++) {
            ofDrawLine(leftPoints[i], rightPoints[i]);
        }
        ofPopStyle();
    }
    
    

}

//_____________________________________________

bool HomoClass::movePoint(vector<ofVec2f>& points, ofVec2f point){
    
    for(int i = 0; i < points.size(); i++) {
        if(points[i].distance(point) < 20) {
            movingPoint = true;
            curPoint = &points[i];
            return true;
        }
    }
    return false;

}

//_____________________________________________

void HomoClass::mousePressed(int x, int y, int button){
 
    ofVec2f cur(x, y);
    ofVec2f rightOffset(left.getWidth(), 0);
    if(!movePoint(leftPoints, cur) && !movePoint(rightPoints, cur) && rightPoints.size()<4 && homographyReady == false) {
        // can't create new homography if there is one / can't create more than 4 points for homography.
      
        rightPoints.push_back(cur);

        
        if (leftPoints.size() == 0){
            ofVec2f point1 (0,0);
            leftPoints.push_back(point1);
        }
        if (leftPoints.size() == 1){
            ofVec2f point1 (640,0);
            leftPoints.push_back(point1);
        }

        if (leftPoints.size() == 2){
            ofVec2f point1 (0,480);
            leftPoints.push_back(point1);
        }

        if (leftPoints.size() == 3){
            ofVec2f point1 (640,480);
            leftPoints.push_back(point1);
        }

        
    }
    
}

//_____________________________________________

void HomoClass::mouseDragged(int x, int y, int button){
    
    if(movingPoint) {
        curPoint->set(x, y);
    }
}

//_____________________________________________


void HomoClass::mouseReleased(int x, int y, int button){
 
    movingPoint = false;

}

//_____________________________________________

void HomoClass::saveHomo(){
   
        saveMatrix = true;
        homographyReady = true;

}

//_____________________________________________
