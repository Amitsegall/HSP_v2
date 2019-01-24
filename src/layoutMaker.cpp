//
//  fromImgToLayoutClass.cpp
//  randomInterfaceTest
//
//  Created by Amit Segall on 29/07/2018.
//
//

#include "layoutMaker.h"

void LayoutMaker::setup(int width,int height){
    

    //use computer vision to make interface

    myImage.allocate(width, height, OF_IMAGE_GRAYSCALE);
    colorImg.allocate(width,height);
    grayImage.allocate(width,height);
    grayBg.allocate(width,height);
    grayDiff.allocate(width,height);
    threshold = 80;
    
    currentImage = 0;
    readImgDir();
    findShapesInImage(myImage);
    makeShapesFromBlobs();

}

//--------------------------------------------------------------


void LayoutMaker::readImgDir(){
    
    // file managment
    dir.listDir("layouts/");
    dir.sort(); // in linux the file system doesn't return file lists ordered in alphabetical order
    
    //allocate the vector to have as many ofImages as files
    if( dir.size() ){
        images.assign(dir.size(), ofImage());
    }
    
    // you can now iterate through the files and load them into the ofImage vector
    for(int i = 0; i < (int)dir.size(); i++){
        images[i].load(dir.getPath(i));
    }
    
    
    if (dir.size() > 0){ // load first image
        myImage = images[currentImage];
        
    }

    
}

//--------------------------------------------------------------


void LayoutMaker::randLayout(int shapes){
    // clean shapes and make new ones
    cleanShapes();
    
    //generate random shapes:
    for (int i=0; i<shapes;i++){
        int randRes = int(ofRandom(5));
        randRes +=3;
        int x = ofRandom(ofGetWidth());
        int y = ofRandom(ofGetHeight());
        int size = ofRandom(50,200);
        float endAngle = 360.0 - 360.0/randRes;
        ofPolyline myPoly;
        myPoly.arc(x,y,size,size,0,endAngle,randRes);
        myPoly.setClosed(true);
        myShapes.push_back(myPoly);
    }
    
    
}


//--------------------------------------------------------------

void LayoutMaker::makeShapesFromBlobs(){
    // clean shapes and make new ones
    cleanShapes();
    
    for (int i = 0; i < contourFinder.blobs.size(); i++){
        ofPolyline blobShape;
        blobShape =  contourFinder.blobs[i].pts;
        myShapes.push_back(blobShape);
    }
    
}



//--------------------------------------------------------------

void LayoutMaker::cleanShapes(){
    while (myShapes.size()>0){
        myShapes.pop_back();
    }
}


//--------------------------------------------------------------

void LayoutMaker::findShapesInImage(ofImage myImage){
    
    colorImg.setFromPixels(myImage.getPixels());
    grayImage = colorImg;
    
//    grayImage.setFromPixels(myImage.getPixels());
    
    // take the abs value of the difference between background and incoming and then threshold:
    grayDiff.absDiff(grayBg, grayImage);
    grayDiff.threshold(threshold);
    
    // find contours which are between the size of 20 pixels and 1/3 the w*h pixels.
    // also, find holes is set to true so we will get interior contours as well....
    contourFinder.findContours(grayDiff, 20, (500*240)/3, 100, false);// find holes
    
    
}
