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
    vector<bool> colorList;
    int colorCheck;
//    ofColor coolCol
    
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
    
    //nice colors 
    ofShader shader1,shader2,shader3;
    ofFbo shaderOut;
    
};
