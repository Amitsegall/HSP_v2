#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){

    ofSetVerticalSync(false); //turned off to gain additioanl fsp

    ofSetBackgroundColor(0);
    int w = 640;
    int h = 480;

    kinect.setRegistration(true);
    kinect.init(true); // IR cam , remove true for the normal cam
    kinect.open();        // opens first available kinect

    // new things to setup
    chess.load("chess640.jpg");

    //init the FBO for homography and tracking
    mainOut.allocate(1280 , 800,GL_RGB); // otherwise the CV will fail - kill the alpah !
    mainOut.begin();
    ofClear(255, 255, 255);
    mainOut.end();
    
    shaderOut.allocate(1280 , 800); // otherwise the CV will fail - kill the alpah !
    shaderOut.begin();
    ofClear(0);
    shaderOut.end();

    //init backdiff
    backDiff.setup();
    blobArea = 0;
    backImg.load("Back.jpg");

    // checking if there is a homography matrix already
    ofFile previous("homography.yml");
    if(previous.exists()) {
        homo.setup("init.png","chess640.jpg");
        homoUp1 = true;
        homoUp2 = true;
    }else {
        homoUp1 = false;
        homoUp2 = false;
    }

//    //Gui
    gui.setup();
    gui.add(backDiffThres.set("Back Diff Threshold", 70, 0, 200));
    gui.add(blobHit.set("Blob Hit Size", 5000, 0, 10000));
    gui.add(minBlob.set("Min Blob min", 0, 0, 10000));
    gui.add(maxBlob.set("Max Blob size", 3000, 3000, 18000));
    gui.add(minVel.set("Min Velocity",0,0,127));
    gui.add(maxVel.set("Max Velocity",127,0,127));
    gui.add(MidiMpeCh.set("MIDI MPE Ch Count",6,1,15));
    gui.add(MinForPitch.set("Min Shape Pitch-x",100,10,500));
    gui.add(MinForCC.set("Min Shape CC-y",100,10,500));
    
    
    

    blobview.setup("Display the Blob",true);
    gui.add(&blobview);
    gui.setPosition(130, 0);
    showAllLayouts.setup("Show All Layouts",false);
    gui.add(&showAllLayouts);
    gui.setPosition(130, 0);
    layoutLimit = 5;

    //layout editor
    maker.setup();
    maker.add(shapeNum.set("shape Number", 0, 0, 100));
    maker.add(shapeNote.set("shape Note",0,0,127));
    button.setup("Set the Note");
    button2.setup("Save Layout");
    maker.add(&button); // change note
    maker.add(&button2); // save layout
    maker.setPosition(130, 0);
    isMaker=false;
    
    //loading layouts to program from .JSON

    bool parsingSuccessful = jsLayouts.open("layouts.json");

    if (parsingSuccessful)
    {
        ofLogNotice("ofApp::setup") << "file is working!";
    }else {
         ofLogError("ofApp::setup")  << "Failed to parse JSON" << endl;
    }

    // creating or reading the gui settings
    if (!ofFile("Settings.xml"))
        gui.saveToFile("Settings.xml");

    gui.loadFromFile("Settings.xml");

    menu = false; // show/hide gui
    liveCam = false; // show/hide live Cam input

    //layout setup
    layout.setup(1280,800);


    //draw hidden interface to change sounds and layouts
    uLeft = ofRectangle(0,0,100,100);
    uRight = ofRectangle(ofGetWidth()-100,0,100,100);
    dLeft = ofRectangle(0,ofGetHeight()-100,100,100);
    dRight = ofRectangle(ofGetWidth()-100,ofGetHeight()-100,100,100);
    musClicked = false; //check mouse click

    // Midi setup
    midi.openPort(0);
    ccVal = 0;
    velocity = 100;
    midi.sendControlChange(1, 119, ccVal);

    instNum = 19; // set to max - 1;
    colorCheck = 1; // any number that is not zero

    bigList.clear();
    listOfNotes.clear();
    cleanNotes = false;
    
    //shaders for colors
    shader1.load("shaders/shader1");
    shader2.load("shaders/shader2");
    shader3.load("shaders/shader3");
    shader4.load("shaders/shader4");
}


//--------------------------------------------------------------
void ofApp::update(){

    updateHomography();

    SelectLayoutInterface();

    musX = ofGetMouseX();
    musY = ofGetMouseY();

}

//--------------------------------------------------------------
void ofApp::draw(){

    float sinTime = sin(ofDegToRad(ofGetFrameNum()));
    float cosTime = cos(ofDegToRad(ofGetFrameNum()));
    float s = ofMap(sinTime,-1,1,0,255);
    float c = ofMap(cosTime,-1,1,0,255);
    
    ofPushStyle();
    ofSetColor(255);
    backImg.draw(0,0); // draw the backimage with layout texts
    ofPopStyle();
    
    if (homoUp1 && homoUp2){
        
        
        if (layout.currentImage != colorCheck){ //create an array of colors only once;
            colorList.resize(layout.myShapes.size());
            for (int x=0 ; x <colorList.size();x++){
                colorList[x]=true;
            }
            colorCheck = layout.currentImage;
        }
        
        
        if ( backDiff.contourFinder2.size()>0){ // if there are blobls
        
            for (int j = 0 ;  j < backDiff.contourFinder2.size(); j++){ // for every blob detected
                blobLocation = backDiff.myLocs[j];
                blobArea = backDiff.myArea[j];
                activeBlobId = backDiff.myIds[j];
                activeMidiCh = (activeBlobId%MidiMpeCh)+2; //ch1 saved for general massages like sustain and others.
                
                checkShapesInLayout(activeMidiCh,blobLocation.x,blobLocation.y,blobArea,s,c,j); /// this is is where it begins
                
                if (blobArea != oldArea){ // check size differenc and make a velocity
                    velocity = abs(blobArea-oldArea);
                }
             
            }//for every blob detected loop
            
            cleanNotes = true;
       
        
        }// if blob detected
        
      
        /// new approach for playing polyphonic:
     
        if (bigList.size()>0) { // if there are notes in list to play
            
            for (int i = 0;  i< bigList.size();i++) {
                
                if (bigList[i].z == 1){
                    
                    layoutToNotes(bigList[i].x,bigList[i].y, 100); // currently fixed velocity
                    colorList[bigList[i].y] = false; //change color to white
                    bigList[i].z = 0; // bool so you can't play more than once.
                  
                } // if not in played
            } // for list of notes
        }
        
        if (backDiff.contourFinder2.size() == 0 && cleanNotes == true){
            for (int x= 1; x<=16;x++){ // select Midi Ch.
                for (int i = 0; i<128;i++){ // select Note
                midi.sendNoteOn(x, i,0);
                }
            }
            bigList.clear();
            listOfNotes.clear();
            cleanNotes = false;

        } // if the list big

        
        drawTheInterface();
        
        
    } else { //if homography not worked
        
        chess.draw(0,0);
        
        if (homoUp1 && !homoUp2){
            
            homo.draw();
        }
    }
    
    if (menu){ //display settings menu
        
        // temp visual rep of ui
        ofDrawRectangle(uLeft);
        ofDrawRectangle(uRight);
        ofDrawRectangle(dLeft);
        ofDrawRectangle(dRight);
        
        gui.draw();
        // draw instructions
        ofSetColor(255, 255, 255);
        stringstream reportStream;
        
        reportStream<< " Blobs detected in IR camera " << backDiff.contourFinder.blobs.size() <<endl<< " Blob size  " << blobArea << " Blobs consider hit at " << blobHit<<" Blob Velocity "<<velocity<<endl<<" App fps: " << ofGetFrameRate() << " layout size " << layout.myShapes.size() << endl;
        
        ofDrawBitmapString(reportStream.str(), 130, ofGetHeight()-40);
        
    }
    
    if (liveCam){ // display the IR camera image on top of everything
        kinect.draw(0,0);
    }
    
    if (blobview){ // turn on / off the visual blob
        
        backDiff.draw();
        
    }
    
    if (isMaker){ // if is maker
        maker.draw();
        
        if (button){
            
            int x = layout.currentImage;
            int y = shapeNum;
            int z = shapeNote;
            jsLayouts["layouts"][x]["notes"][y] = z;
        }
        if (button2){
            // save the xml with new layout
            jsLayouts.save("layouts.json");
        
        }
    } // enable the layout editor
    
    oldArea = blobArea; //for velocity calculation
    

}

//--------------------------------------------------------------

void ofApp::checkShapesInLayout(int blobId, int x, int y, int area, int s, int c,int placeInLine){

    int smoothVelocity = ofMap(velocity, 0, 4000, minVel, maxVel,true); // smooth the velocity  int
    
    for (int i = 0; i<layout.myShapes.size();i++){
        
        if (area <= blobHit && layout.myShapes[i].inside(x,y)){
            
            //expriment with expression!
            
//            Hit -  only Y modulation / Mod Wheel
//            Strike - only X modulation / PitchBand
//            Play - X,Y,Z modulation all working
            
            int pbVal = ofMap(x, layout.myShapes[i].getBoundingBox().getMinX(), layout.myShapes[i].getBoundingBox().getMaxX(), 0, 16383); //x
            int modVal = ofMap(y, layout.myShapes[i].getBoundingBox().getMinY(), layout.myShapes[i].getBoundingBox().getMaxY(), 127,0); // y
            int atVal = ofMap(area, minBlob,blobHit, 127,0); // z
            
            midi.sendAftertouch(blobId, atVal); // just sending AF to differnet channels all the time on the Z axis
            
             if ((layout.myShapes[i].getBoundingBox().getMaxY() - layout.myShapes[i].getBoundingBox().getMinY())  > MinForCC){ // threshold for cc modulation on the Y axis (SWIPE)
                     midi.sendControlChange(blobId,1, modVal);  // always send modVal on Y axis the opposite of at value.
             }
            
            if ((layout.myShapes[i].getBoundingBox().getMaxX() - layout.myShapes[i].getBoundingBox().getMinX())  > MinForPitch){ // threshold for pitchband on the X axis (SLIDE)
            midi.sendPitchBend(blobId, pbVal);
            }
            
           
            if (bigList.size() == 0) {
//                cout<<"first item in list!"<<endl;
                ofPoint merge;
                merge.x = blobId; // id
                merge.y = i; // shape num
                merge.z = 1; // canPlayBool
                bigList.push_back(merge);
                listOfNotes.push_back(i);
                
            }else{ // if list is 1 or more // polyphony
                
                if (std::find(std::begin(listOfNotes),std::end(listOfNotes), i) != std::end(listOfNotes)) {
                    // if item is inside don't do anything
                    
                }else{

                    ofPoint merge;
                    merge.x = blobId; // id
                    merge.y = i; // shape num
                    merge.z = 1; // canPlayBool
                    bigList.push_back(merge);
                    listOfNotes.push_back(i);

                }
            }
            
        }else{ //// if the condition is failed
            
            for (int x = 0; x < bigList.size();x++){
                
                if (bigList[x].x == blobId){
                    
                    if(bigList[x].z == 0 && bigList[x].y == i){// if the item can't play and shape is different
//                    cout<<"removing item form list "<<endl;
                    int noteOff = bigList[x].y;
                    int locId = bigList[x].x;
                    midi.sendPitchBend(locId, 8192); // make sure that no pitchband is stuck
                    midi.sendControlChange(locId, 1, 0); // clean the modwheel
                    midi.sendNoteOn(locId, jsLayouts["layouts"][layout.currentImage]["notes"][noteOff].asInt(),0);
                    bigList.erase(bigList.begin()+x);
                    listOfNotes.erase(listOfNotes.begin()+x);
                    
                    }
                    
                }
            }
            if (std::find(std::begin(listOfNotes),std::end(listOfNotes), i) != std::end(listOfNotes)) {// if item is inside don't do anything)
            }else{
                colorList[i]=true; // change color to all other shapes
            }
        }// faild condition
        
    }// for every shape
    
}// end of function

//--------------------------------------------------------------
void ofApp::playWithMouse(){
    
    for (int i = 0; i<layout.myShapes.size();i++){
        
        if (layout.myShapes[i].inside(musX, musY) && musClicked){
            colorList[i] = false; //change the shape color to white
            layoutToNotes(1,i, 100); //play the note
            musClicked = false;
            
        }else{
            colorList[i] = true; // clear too much
        }
            
    }
}


//--------------------------------------------------------------
void ofApp::drawTheInterface(){  // this is what draws the shape and check if they are inside
    shaderOut.begin();
    ofClear(0);
    for (int i = 0; i<layout.myShapes.size();i++){
        

        drawTheShape(i);

    }
    
    shaderOut.end();
    
    
    shaderOut.draw(0,0);
}


//--------------------------------------------------------------

void ofApp::drawTheShape(int shapeNum){
    
    // draw the shapes of the layouts!
    
    
    if (colorList[shapeNum] == true){
    
        bool yBig = false;
        bool xBig = false;
    
        if ((layout.myShapes[shapeNum].getBoundingBox().getMaxX() - layout.myShapes[shapeNum].getBoundingBox().getMinX()) > MinForPitch){ // width
            xBig = true;
        }
        
        if ((layout.myShapes[shapeNum].getBoundingBox().getMaxY() - layout.myShapes[shapeNum].getBoundingBox().getMinY()) > MinForCC){ // height
            yBig = true;
        }

    
        if (yBig == true && xBig == true){ // both x+y

            shader1.begin();
            
            shader1.setUniform1f("u_time",ofGetElapsedTimef());
            shader1.setUniform2f("u_resolution",1280.0,800.0);
            
            ofBeginShape();
            auto vertices = layout.myShapes[shapeNum].getVertices();
            for(int j = 0; j < vertices.size(); j++) {
                ofVertex(vertices[j]);
            }
            ofEndShape();
            shader1.end();

        }else if (yBig == true && xBig == false){ // just y

            shader2.begin();
    
            shader2.setUniform1f("u_time",ofGetElapsedTimef());
            shader2.setUniform2f("u_resolution",1280.0,800.0);
            
            ofBeginShape();
            auto vertices = layout.myShapes[shapeNum].getVertices();
            for(int j = 0; j < vertices.size(); j++) {
                ofVertex(vertices[j]);
            }
            ofEndShape();
            shader2.end();

        }else if (yBig == false && xBig == true){ // just x

            shader3.begin();
            
            shader3.setUniform1f("u_time",ofGetElapsedTimef());
            shader3.setUniform2f("u_resolution",1280.0,800.0);
            
            ofBeginShape();
            auto vertices = layout.myShapes[shapeNum].getVertices();
            for(int j = 0; j < vertices.size(); j++) {
                ofVertex(vertices[j]);
            }
            ofEndShape();
            shader3.end();
            
        }else if (xBig == false && yBig == false){ // if no expression
            
            shader4.begin();
            
            shader4.setUniform1f("u_time",ofGetElapsedTimef());
            shader4.setUniform2f("u_resolution",1280.0,800.0);
            
            ofBeginShape();
            auto vertices = layout.myShapes[shapeNum].getVertices();
            for(int j = 0; j < vertices.size(); j++) {
                ofVertex(vertices[j]);
            }
            ofEndShape();
            shader4.end();
        }

    }else{

        ofSetColor(255);
        ofBeginShape();
        auto vertices = layout.myShapes[shapeNum].getVertices();
        for(int j = 0; j < vertices.size(); j++) {
            ofVertex(vertices[j]);
        }
        ofEndShape();

    }
    
    // write the number of shapes
    if (menu){
        ofSetColor(255);
        ofDrawBitmapString(jsLayouts["layouts"][layout.currentImage]["notes"][shapeNum], layout.myShapes[shapeNum].getCentroid2D().x,layout.myShapes[shapeNum].getCentroid2D().y+30);
        
    }
    
    if (isMaker){
        
        ofSetColor(255);
        ofDrawBitmapString(shapeNum, layout.myShapes[shapeNum].getCentroid2D());
        
        auto s = std::to_string(jsLayouts["layouts"][layout.currentImage]["notes"][shapeNum].asInt()/12);
        
        ofDrawBitmapString((noteNames[jsLayouts["layouts"][layout.currentImage]["notes"][shapeNum].asInt()%12]+s), layout.myShapes[shapeNum].getCentroid2D().x,layout.myShapes[shapeNum].getCentroid2D().y+15);
        
    }
}


//--------------------------------------------------------------
void ofApp::layoutToNotes(int ch, int note, int velocity){
    
    
    midi.sendNoteOn(ch, jsLayouts["layouts"][layout.currentImage]["notes"][note].asInt(), velocity);
    
}


//--------------------------------------------------------------

void ofApp::SelectLayoutInterface(){
    
    bool change = false;
    
        //  change interface from my interface list
        if (uLeft.inside(blobLocation.x,blobLocation.y) && blobArea <= blobHit){//up left
            if (canClick[0]){
                canClick[0] = false;
                change = true;
                
                if (showAllLayouts){
                    if (layout.currentImage > 0){
                        layout.currentImage --;
                        layout.currentImage %= layout.dir.size();

                    }else{
                        layout.currentImage = layout.dir.size()-1;
                        layout.currentImage %= layout.dir.size();
                    }
                }else {
                    if (layout.currentImage > 0){
                        layout.currentImage --;
                        layout.currentImage %= layout.dir.size();
                    
                    }else {
                        layout.currentImage = layoutLimit;
                        layout.currentImage %= layout.dir.size();
                    }
                }
            }
                
            ccVal = layout.currentImage;
            midi.sendControlChange(1, 119, ccVal);

        }else{
            canClick[0]=true;
        }


        if (uRight.inside(blobLocation.x,blobLocation.y) && blobArea <= blobHit){//up right
            if(canClick[1]){
                canClick[1] = false;
                change = true;
                if (showAllLayouts){
                layout.currentImage ++;
                layout.currentImage %= layout.dir.size();
                }else if (layout.currentImage >= layoutLimit){
                        layout.currentImage = 0;
                } else {
                    layout.currentImage++;
                    layout.currentImage %= layout.dir.size();
                
                }
            }
            ccVal = layout.currentImage;
            midi.sendControlChange(1, 119, ccVal);

        }else{
            canClick[1]=true;
        }

/*** in HSP v2 you can't change sound to a layout, they all apart of the layout itself***/
    
    //  change musical instrument in Ableton using MIDI Control Change msg
//    if (dLeft.inside(blobLocation.x,blobLocation.y) && blobArea <= blobHit){//down left
//        if(canClick[2]){
//            canClick[2] = false;
//            if ( ccVal > 0){
//                ccVal --;
//            }else{
//                ccVal= instNum;
//            }
//            midi.sendControlChange(1, 119, ccVal);
//        }
//    }else{
//        canClick[2]=true;
//    }
//
//    if (dRight.inside(blobLocation.x,blobLocation.y) && blobArea <= blobHit){//down right
//        if(canClick[3]){
//            canClick[3] = false;
//            ofBackground(255);
//            if ( ccVal < instNum){
//                ccVal ++;
//            }else{
//                ccVal= 0;
//            }
//            midi.sendControlChange(1, 119, ccVal);
//        }
//    }else{
//        canClick[3]=true;
//    }

    if (change){ //if layout changed draw new one
    layout.cleanShapes();
    layout.findShapesInImage(layout.images[layout.currentImage]);
    layout.makeShapesFromBlobs();
    }
    
}


//--------------------------------------------------------------
void ofApp::updateHomography(){
    
    kinect.update();
    
    if (kinect.isFrameNew()){

//        // creating an image from videos
        
        camImg.setFromPixels(kinect.getPixels());

        homo.update(camImg); // processing the original image in the homography class

        //homography update

        if (homoUp1 && homoUp2){

            mainOut.begin();
            homo.draw(); // drawing out the acual aligned image back
            mainOut.end();

            // processing only the actual space i'm at (the screen):
            
            backDiff.update(mainOut,backDiffThres,minBlob,maxBlob);
            
        }

    }// close new frame
    
}


//--------------------------------------------------------------


void ofApp::initImg(ofImage input){

    initImage.setFromPixels(input.getPixels());
    ofSaveImage(initImage,"init.png");
}

//--------------------------------------------------------------

void ofApp::keyPressed(int key){
    
    switch (key){
            
            // setting the homography :
            
        case '1' : // set the chess board full screen on the output
            ofToggleFullscreen();
            chess.resize(ofGetWidth(), ofGetHeight());
            break;
        case '2' : // capture a new image from the camera and save cam settings
            initImg(camImg);
            break;
        case '3': // setup both images in the homography class
            homo.setup("chess640.jpg","init.png"); // setting up the chessboard and the image of the space(init)
            homoUp1 = true;
            break;
        case '4': // save homography and start playing !
            homoUp2 = true;
            homo.saveHomo();
            ofClear(255);
            break;
            
            // menu and settings :
            
        case 'l': // open camera
            liveCam = !liveCam;
            break;
        case 'm': // open settings menu
            menu = !menu;
            break;
        case 's':
            gui.saveToFile("Settings.xml"); // save settings
            break;
    
            
            //additional layout options:
            
        case '0':                //open layout maker settings
            isMaker = !isMaker;
            break;
            
            
        case ' ': // change layouts one after another
            
            if (showAllLayouts){
                if (layout.dir.size() > 0){
                    layout.currentImage++;
                    layout.currentImage %= layout.dir.size();
                }
            }else if (layout.dir.size() > 0 && layout.currentImage < layoutLimit){
                    layout.currentImage++;
                    layout.currentImage %= layout.dir.size();
            }else {
                layout.currentImage = 0;
            }
            
            ccVal = layout.currentImage;
            midi.sendControlChange(1, 119, ccVal);
            layout.cleanShapes();
            layout.findShapesInImage(layout.images[layout.currentImage]);
            layout.makeShapesFromBlobs();
            
            break;
       
            // computer vision settings and tools:
            
        case 'b': //set background for back differencing
            backDiff.setBack();
            break;
      
        case 'c': // set blob size
            blobHit.set(blobArea+1000);
            maxBlob.set(blobArea+5000);
            minBlob.set(blobArea-2000);
            break;
    
    }
    
}

//--------------------------------------------------------------


void ofApp::keyReleased(int key){
    
}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){
    
}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){
    
    homo.mouseDragged(x, y, button);
}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){
    
    homo.mousePressed(x, y, button);
    musClicked = true;
    if (homoUp1 && homoUp2){
    playWithMouse(); /// mostly for testing
    }
    
}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){
    
    if (!menu){
    
        homo.mouseReleased(x, y, button);
        musClicked = false;
        for (int i = 0; i<128;i++){ // select Note
            midi.sendNoteOn(1, i,0);
        }
    }
    
    
    
}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y){
    
}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y){
    
}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){
    
}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){
    
}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){
    
}
