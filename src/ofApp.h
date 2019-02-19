#pragma once

#include "ofMain.h"
#include "ofxKinect.h"
#include "ofxGui.h"
#include "ofxJson.h"
#include "ofxMidi.h"

//my classes to make nice code:

//layout maker and midi playback
#include "layoutMaker.h"
// homograpy and tracking
#include "homoClass.h"
#include "BackDiffClass.h"

#include <algorithm> // for std::find
#include <iterator> // for std::begin, std::end


class ofApp : public ofBaseApp{

	public:
		void setup();
		void update();
		void draw();

		void keyPressed(int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y );
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void mouseEntered(int x, int y);
		void mouseExited(int x, int y);
		void windowResized(int w, int h);
		void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);
    
    // midi out
    ofxMidiOut midi;
    int ccVal;
    bool cleanNotes;
    int activeBlobId, activeMidiCh;
    vector<int> listOfNotes;
    vector<ofPoint>bigList; // storing x = id , y = shape/note, z = can play 0/1. 
    
    
    int instNum;
    int oldArea; //helps calculate velocity
    int velocity;
    int whiteNotes [7] = {3,5,6,8,10,0}; //excep C which will set root!
    void playWithMouse();
    
    
    //create layouts
    LayoutMaker layout;
    void SelectLayoutInterface();
    void drawTheShape(int ShapeNum);
    void drawTheInterface();
    void checkShapesInLayout(int blobNumber, int x, int y, int area,int s, int c,int placeInLine);
    int layoutLimit;
    
    //layout maker editor
    ofxPanel maker;
    ofParameter <int> shapeNum;
    ofParameter <int> shapeNote;
    ofxButton button;
    ofxButton button2;
    bool isMaker;
    vector<int> newNotes;
    
   
    // interface to change layouts and sounds
    ofRectangle uLeft,uRight,dLeft,dRight;
    bool isRandom;
    bool canClick[4] = {true,true,true,true};
    
    
    // interface layouts
    string noteNames[12] = {"C","C#","D","D#","E","F","F#","G","G#","A","A#","B"};
    ofxJSONElement jsLayouts;
    
    // colors
    vector<ofColor> colorList;
    int colorCheck;
    int pianoCol [31] = {27,8,15,22,20,10,3,25,17,24,6,29,13,30,18,5,12,1,31,4,2,14,11,19,9,21,7,23,26,28,16};
    int mpcCol [16] = {8,6,11,2,7,5,12,10,9,16,14,4,3,1,15,13};
    int kalCol [16] = {12,11,6,1,3,5,16,14,10,8,4,2,7,8,13,15};
    int tempCol [16] = {8,7,6,5,4,3,2,15,14,13,11,10,9,1,16,12};
    int panCol[9] = {4,5,8,3,1,2,7,0,6};
    
    int pushCromCol [64] = {49,14,8,55,39,6,57,63,
        48,50,0,15,13,12,11,10,
        9,51,22,52,53,54,41,16,
        47,30,33,24,27,26,25,28,
        7,38,37,36,35,34,29,32,
        31,46,45,44,43,42,17,40,
        18,19,20,21,23,1,2,3,
        4,62,61,60,59,58,5,56};
    
    int cirOfFifCol [24] = {0,9,3,6,10,1,7,4,11,2,8,5,
        12,21,15,18,22,19,16,13,23,14,20,17};
    
    
    ofColor coolCol;
    
    void layoutToNotes(int ch, int note, int velocity);
    void layoutColor(int val1, int val2,int val3);
    
    //kinect cam
    ofxKinect kinect;
    
    //gui
    ofxPanel gui;
    ofParameter<int> layThres;
    ofParameter<int> backDiffThres;
    ofParameter<int> blurVal;
    ofParameter<int> blobHit;
    ofParameter<int> minBlob;
    ofParameter<int> maxBlob;
    ofParameter<int> minVel;
    ofParameter<int> maxVel;
    ofParameter<int> MidiMpeCh;
    ofParameter<int> MinForPitch;
    ofParameter<int> MinForCC;
    
    ofxToggle showAllLayouts;
    ofxToggle blobview;
    
    bool menu; // show gui
    
    // homography class
    HomoClass homo;
    bool homoUp1,homoUp2;
    void updateHomography();
    void drawHomography();
    
    ofImage chess,initImage,camImg;
    ofFbo mainOut;

    
    ofPixels currentPixels,outPixels;
    void initImg(ofImage input);
    bool liveCam;
    
    
    //backDiff
    BackDiff backDiff;
    int blobArea;
    ofPoint blobLocation;
    ofImage backImg;
    
    //mouse info
    bool musClicked;
    int musX,musY;
    
		
};
