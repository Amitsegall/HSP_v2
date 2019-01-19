#pragma once

#include "ofMain.h"
#include "ofxKinect.h"
#include "ofxGui.h"


//my classes to make nice code:

//layout maker and midi playback
#include "midiOutClass.h"
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
    MidiOutClass mout;
    int ccVal;
    vector<bool>canPlayNow;
    
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
    void checkShapesInLayout(int blobNumber, int x, int y, int area,int s, int c,int blobCount);
    
    //layout maker editor
    ofxPanel maker;
    ofParameter <int> shapeNum;
    ofParameter <int> shapeNote;
    ofxButton button;
    bool isMaker;
    vector<int> newNotes;
    
    
    // interface to change layouts and sounds
    ofRectangle uLeft,uRight,dLeft,dRight;
    bool isRandom;
    bool canClick[4] = {true,true,true,true};
    
    
    // interface layouts
    string noteNames[12] = {"C","C#","D","D#","E","F","F#","G","G#","A","A#","B"};
    int kalimba [16] = {77,79,67,62,65,69,84,81,74,71,64,60,72,76,83,86};
    int pandrum [9] = {78,71,72,67,79,64,83,76,66};
    int piano [31] = {27,8,15,22,20,10,3,25,17,24,6,29,13,30,18,5,12,1,31,4,2,14,11,19,9,21,7,23,26,28,16};
    int mpc [16] = {8,6,11,2,7,5,12,10,9,16,14,4,3,1,15,13};
    int tempest [16] = {8,7,6,5,4,3,2,15,14,13,11,10,9,1,16,12};
    
    int isoGerhard [96] = {44,25,23,12,6,20,11,27,30,23,
        33,26,29,18,16,9,13,42,17,24,
        5,12,44,15,43,33,36,22,32,41,
        14,28,34,31,40,47,8,10,38,35,
        7,47,39,26,19,21,37,45,4,26,
        22,8,37,19,22,3,28,33,15,41,
        5,11,29,38,32,9,36,17,42,39,
        35,6,20,27,10,25,43,1,31,8,
        40,19,34,16,40,13,12,18,24,29,
        14,21,2,30,7,23};
    
    int janko [44] = {17,9,14,24,18,1,9,16,10,8,
        15,17,13,6,19,15,11,8,14,12,
        6,11,13,21,19,12,23,2,4,18,
        7,5,3,20,22,20,22,16,3,10,
        7,21,5,4};
    
    int pushCrom [64] = {49,14,8,55,39,6,57,63,
        48,50,0,15,13,12,11,10,
        9,51,22,52,53,54,41,16,
        47,30,33,24,27,26,25,28,
        7,38,37,36,35,34,29,32,
        31,46,45,44,43,42,17,40,
        18,19,20,21,23,1,2,3,
        4,62,61,60,59,58,5,56};
    
    int able [8] = {7,4,5,6,3,1,0,2};
    int cirOfFif [24] = {0,15,9,6,10,7,13,4,17,2,8,11
        ,-3,12,6,3,7,10,1,4,14,-1,5,8};
    int avicii [8] = {1,0,5,3,2,7,6,4};
    int skrillex [8] = {2,1,0,7,6,4,5,3};
    
    int abc [36] = {17,15,4,36,34,2,23,13,18,14,
        32,35,7,1,16,8,11,21,30,5,
        24,19,26,31,28,29,3,22,6,25,
        33,20,12,10,25,9};
    
    int bigCirc [48] = { 15,9,13,7,4,10,8,2,11,17,6,0,
        13,19,14,17,8,11,15,21,6,12,10,4,
        14,7,24,17,22,16,9,11,15,18,20,13,
        25,27,20,21,23,19,16,10,17,14,18,12};
    
    int triangles [32] = { 21,4,15,7,18,11,8,14,11,15,
        10,18,7,9,16,13,12,10,9,16,
        20,17,14,5,25,0,27,-2,1,3,
        24,21};
    
    int thunder [27] = {4,3,6,4,9,7,12,11,10,8,16,15,13,14,27,25,26,24,23,17,22,18,21,19,20,1,2};
    
    int birdX [35] = {2,2,1,1,0,0,3,3,13,13,
        11,6,4,5,6,12,5,4,10,11,
        10,12,16,14,14,16,7,9,7,9,
        15,15,17,8,8,};
    
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
    
    void layoutToNotes(int note, int velocity);
    void layoutColor(int val1, int val2,int val3);
    
    //kinect cam
    ofxKinect kinect;
    
    //gui
    ofxPanel gui;
    ofParameter<int> layThres;
    ofParameter<int> backDiffThres;
    ofParameter<int> blurVal;
    ofParameter<int> minBlobSize;
    ofParameter<int> minBlob;
    ofParameter<int> maxBlob;
    ofParameter<int> minVel;
    ofParameter<int> maxVel;
    
    
    
    ofxToggle blobview;
    
    bool menu; // show gui
    
    // homography class
    HomoClass homo;
    bool homoUp1,homoUp2;
    void updateHomography();
    void drawHomography();
    
    ofImage chess,initImage,camImg;
    ofFbo camImage;
    ofFbo mainOut;
    
    
    ofPixels currentPixels,outPixels;
    void initImg(ofFbo input);
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
