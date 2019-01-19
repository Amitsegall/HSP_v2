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
    camImage.allocate(w, h,GL_RGBA);
    camImage.begin();
    ofClear(255, 255, 255,255);
    camImage.end();
    initImage.allocate(w, h,ofImageType::OF_IMAGE_COLOR_ALPHA);

    mainOut.allocate(1280, 800,GL_RGB); // otherwise the CV will fail - kill the alpah !
    mainOut.begin();
    ofClear(255, 255, 255);
    mainOut.end();

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
    gui.add(layThres.set("Layout Threshold",80,0,255));
    gui.add(backDiffThres.set("Back Diff Threshold", 70, 0, 200));
    gui.add(blurVal.set("Blur value", 0, 0, 30));
    gui.add(minBlobSize.set("Blob Size Hit", 5000, 0, 10000));
    gui.add(minBlob.set("Min Blob min", 0, 0, 10000));
    gui.add(maxBlob.set("Max Blob size", 3000, 3000, 18000));
    gui.add(minVel.set("Min Velocity",0,0,127));
    gui.add(maxVel.set("Max Velocity",127,0,127));


    blobview.setup("Display the Blob",true);
    gui.add(&blobview);
    gui.setPosition(130, 0);

    //layout editor
    maker.setup();
    maker.add(shapeNum.set("shape Number", 0, 0, 16));
    maker.add(shapeNote.set("shape Note",0,0,127));
    button.setup("Set the note");
    maker.add(&button);
    maker.setPosition(130, 0);
    isMaker=false;


    // creating or reading the gui settings
    if (!ofFile("Settings.xml"))
        gui.saveToFile("Settings.xml");

    gui.loadFromFile("Settings.xml");

    menu = false; // show/hide gui
    liveCam = false; // show/hide live Cam input


    //layout setup
    layout.setup(1280,800);
    isRandom = false;

    //draw hidden interface to change sounds and layouts
    uLeft = ofRectangle(0,0,100,100);
    uRight = ofRectangle(ofGetWidth()-100,0,100,100);
    dLeft = ofRectangle(0,ofGetHeight()-100,100,100);
    dRight = ofRectangle(ofGetWidth()-100,ofGetHeight()-100,100,100);
    musClicked = false; //check mouse click

    // Midi setup
    mout.setup();
    ccVal = 0;
    velocity = 100;
    mout.sendControlChange(1, 119, ccVal);

    //initialize playing options!
    canPlayNow.resize(4);
    for (int i = 0;i<canPlayNow.size();i++){
        canPlayNow[i]=true;
    }


    instNum = 19; // set to max - 1;
    colorCheck = 1; // any number that is not zero
    
}


//--------------------------------------------------------------
void ofApp::update(){

    updateHomography();

    if(layThres != layout.threshold){
        layout.threshold = layThres;
        layout.findShapesInImage(layout.images[layout.currentImage]);
        layout.makeShapesFromBlobs();
    }

    SelectLayoutInterface();

    musX = ofGetMouseX();
    musY = ofGetMouseY();

}

//--------------------------------------------------------------
void ofApp::draw(){

//    make the colors move later
    float sinTime = sin(ofDegToRad(ofGetFrameNum()));
    float cosTime = cos(ofDegToRad(ofGetFrameNum()));
    float s = ofMap(sinTime,-1,1,0,255);
    float c = ofMap(cosTime,-1,1,0,255);
    
    backImg.draw(0,0); // draw the backimage with layout texts
    
    
    if (homoUp1 && homoUp2){
        
        
        if (layout.currentImage != colorCheck){ //create an array of colors only once;
            colorList.resize(layout.myShapes.size());
            colorCheck = layout.currentImage;
            for (int i = 0; i<layout.myShapes.size();i++){
                layoutColor(i,s,c);
            }
        }
        
        
        if ( backDiff.contourFinder.blobs.size()>0){ // if there are blobls
            
            for (int j = 0 ;  j < backDiff.contourFinder.blobs.size(); j++){ // for every blob detected
                
                int blobCount = backDiff.contourFinder.blobs.size();
                blobLocation = backDiff.contourFinder.blobs[j].centroid;
                blobArea = backDiff.contourFinder.blobs[j].area;
                
                checkShapesInLayout(backDiff.myIds[j],blobLocation.x,blobLocation.y,blobArea,s,c,blobCount);
                
                
                if (blobArea != oldArea){ // check size differenc and make a velocity
                    velocity = abs(blobArea-oldArea);
                }
                
            }//for every blob detected loop
            
            
        }else{
            for (int i = 0; i<layout.myShapes.size();i++){
                layoutColor(i,s,c);
            }
        }// if blob detected
        
        
        playWithMouse();
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
        
        reportStream<< " Blobs detected in IR camera " << backDiff.contourFinder.blobs.size() <<endl<< " Blob size  " << blobArea << " Blobs consider hit at " << minBlobSize<<" Blob Velocity "<<velocity<<endl<<" App fps: " << ofGetFrameRate() << " layout size " << layout.myShapes.size() << endl;
        
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
            newNotes[shapeNum] = shapeNote;
            
        }
    } // enable the layout editor
    
    oldArea = blobArea; //for velocity calculation
    
    
    
}

//--------------------------------------------------------------

void ofApp::checkShapesInLayout(int blobId, int x, int y, int area, int s, int c,int blobcount){ // this function is the one that sends midi out !
    
    
    int smoothVelocity = ofMap(velocity, 0, 4000, minVel, maxVel,true); // smooth the velocity
    
    if (blobcount > 1){
        for (int i = 0; i<layout.myShapes.size();i++){
            
            if (area <= minBlobSize ){
                
                if (layout.myShapes[i].inside(x,y)){
                    
                    
                    if (canPlayNow[blobId] == true) {
                        colorList[i] = ofColor(255); //change the shape color to white
                        layoutToNotes(i, smoothVelocity); //play the note
                        canPlayNow[blobId] = false; // make sure you can only play one note
                        
                    }
                    
                }else{
                    layoutColor(i,s,c);// leave the color as it was in the original list on all other shape
                }
                //             canPlayNow[i] = true;
            }else{
                canPlayNow[blobId] = true;
            }
            
            
        } // for every shape in the layout
    }else{
        
        for (int i = 0; i<layout.myShapes.size();i++){
            
            if (area <= minBlobSize ){
                
                if (layout.myShapes[i].inside(x,y)){
                    
                    
                    if (canPlayNow[i] == true) {
                        colorList[i] = ofColor(255); //change the shape color to white
                        layoutToNotes(i, smoothVelocity); //play the note
                        canPlayNow[i] = false; // make sure you can only play one note
                        
                    }
                    
                }else{
                    layoutColor(i,s,c);// leave the color as it was in the original list on all other shape
                }
                //             canPlayNow[i] = true;
            }else{
                canPlayNow[i] = true;
            }
            
            
        }
    }
    
}// end of function


//--------------------------------------------------------------
void ofApp::playWithMouse(){
    
    for (int i = 0; i<layout.myShapes.size();i++){
        
        if (layout.myShapes[i].inside(musX, musY) && musClicked){
            colorList[i] = ofColor(255); //change the shape color to white
            layoutToNotes(i, 100); //play the note
            
        }
    }
}


//--------------------------------------------------------------
void ofApp::drawTheInterface(){  // this is what draws the shape and check if they are inside
    
    
    for (int i = 0; i<layout.myShapes.size();i++){
        
        ofPushStyle();
        
        drawTheShape(i);
        
        ofPopStyle();
        
    }
    
}


//--------------------------------------------------------------

void ofApp::drawTheShape(int shapeNum){
    
    // draw the shapes of the layouts!
    
    ofSetColor(colorList[shapeNum]);
    ofBeginShape();
    auto vertices = layout.myShapes[shapeNum].getVertices();
    for(int j = 0; j < vertices.size(); j++) {
        ofVertex(vertices[j]);
    }
    ofEndShape();
    
    // write the number of shapes
    if (menu){
        ofSetColor(255);
        ofDrawBitmapString(shapeNum+1, layout.myShapes[shapeNum].getCentroid2D());
        
    }
    
    if (isMaker){
        ofSetColor(255);
        ofDrawBitmapString(shapeNum, layout.myShapes[shapeNum].getCentroid2D());
        auto s = std::to_string(newNotes[shapeNum]/12);
        ofDrawBitmapString((noteNames[newNotes[shapeNum]%11]+s), layout.myShapes[shapeNum].getCentroid2D().x,layout.myShapes[shapeNum].getCentroid2D().y+15);
        
    }
}


//--------------------------------------------------------------
void ofApp::layoutToNotes(int note, int velocity){
    
    if (!isMaker){
        
        switch (layout.currentImage){
                
            case 0 : mout.sendNoteOn(1, piano[note]+47, velocity, 1000); break; // piano
            case 1 : mout.sendNoteOn(1, kalimba[note], velocity, 1000); break; // kalimba
            case 2 : mout.sendNoteOn(1, mpc[note]+36, velocity, 1000) ; break; // mpc
            case 3 : mout.sendNoteOn(1, tempest[note]+48, velocity, 1000); break; // tempest
            case 4 : mout.sendNoteOn(1, pushCrom[note]+36, velocity, 1000); break; // push
            case 5 : mout.sendNoteOn(1, pandrum[note], velocity, 1000) ; break; // pandrum
            case 6 : mout.sendNoteOn(1, 48+cirOfFif[note], velocity, 1000); break; // CircleOfFifths
            case 7 : mout.sendNoteOn(1, 36+bigCirc[note], velocity, 1000); break; // CircleV2
            case 8 : mout.sendNoteOn(1, 36+triangles[note], velocity, 1000); break; // Triangle
            case 9 : mout.sendNoteOn(1, isoGerhard[note]+48, velocity, 1000); break; // isoGerhard
            case 10 : mout.sendNoteOn(1, 47+thunder[note], velocity, 1000); break; // thunder
            case 11 : mout.sendNoteOn(1, 60+birdX[note], velocity, 1000); break; // BirdX
            case 12 : mout.sendNoteOn(1, janko[note]+48, velocity, 1000); break; // Janko
            case 13 : mout.sendNoteOn(1, 60+able[note], velocity, 1000); break; // Ableton
            case 14 : mout.sendNoteOn(1, 36+avicii[note], velocity, 1000); break; // Avicii
            case 15 : mout.sendNoteOn(1, 36+skrillex[note], velocity, 1000); break; // Skrillex
            case 16 : mout.sendNoteOn(1, 36+abc[note], velocity, 1000); break; // Charts
            case 17 : mout.sendNoteOn(1, 36+note, velocity, 1000); break; //
            case 18 : mout.sendNoteOn(1, 36+note, velocity, 1000); break; //
        }
        
    }else{
        mout.sendNoteOn(1, newNotes[note], velocity, 1000);
    }
    
    
    
}

//--------------------------------------------------------------
void ofApp::layoutColor(int i, int val2,int val3){
    
    switch (layout.currentImage){
            
        case 0 :
            //            coolCol = ofColor::fromHsb(ofMap(pianoCol[i]-1,0,layout.myShapes.size(),val2,val3),255,255); break; // piano
            if (std::find(std::begin(whiteNotes), std::end(whiteNotes), pianoCol[i]%12) != std::end(whiteNotes)){
                coolCol = ofColor(255,0,val2);
            }else if (pianoCol[i]%12 == 1){
                coolCol = ofColor(0,255,0);
            } else {
                coolCol = ofColor(val3,0,255);
            }
            colorList[i] = coolCol;
            break;
            
        case 1 : coolCol = ofColor::fromHsb(ofMap(kalCol[i]-1,0,layout.myShapes.size(),val2,val3),255,255);colorList[i] = coolCol; break; // kalimba
        case 2 : coolCol = ofColor::fromHsb(ofMap(mpcCol[i]-1,0,layout.myShapes.size(),val2,val3),255,255);colorList[i] = coolCol; break; // mpc
        case 3 : coolCol = ofColor::fromHsb(ofMap(tempCol[i]-1,0,layout.myShapes.size(),val2,val3),255,255);colorList[i] = coolCol; break; // tempest
            
        case 4 :
            //             coolCol = ofColor::fromHsb(ofMap(pushCromCol[i]-1,0,layout.myShapes.size(),val2,val3),255,255); break; // push
            if (std::find(std::begin(whiteNotes), std::end(whiteNotes), (pushCromCol[i]+1)%12) != std::end(whiteNotes)){
                coolCol = ofColor(255,0,val2);
            }else if ((pushCromCol[i]+1)%12 == 1){
                coolCol = ofColor(0,255,0);
            } else {
                coolCol = ofColor(val3,0,255);
            }
            colorList[i] = coolCol;
            break;
            
        case 5 : coolCol = ofColor::fromHsb(ofMap(panCol[i],0,layout.myShapes.size(),val2,val3),255,255);colorList[i] = coolCol; break; // pandrum
        case 6 : coolCol = ofColor::fromHsb(ofMap(cirOfFifCol[i],0,layout.myShapes.size(),val2,val3),255,255);colorList[i] = coolCol; break; // CircleOfFifths
        case 7 : coolCol = ofColor::fromHsb(ofMap(i,0,layout.myShapes.size(),val2,val3),255,255);colorList[i] = coolCol; break; // CircleV2
            
            
        case 8 :
            //            coolCol = ofColor::fromHsb(ofMap(i,0,layout.myShapes.size(),val2,val3),255,255);colorList[i] = coolCol; break; // Triangle
            if (std::find(std::begin(whiteNotes), std::end(whiteNotes), (triangles[i]+1)%12) != std::end(whiteNotes)){
                coolCol = ofColor(255,0,val3);
            }else if ((triangles[i]+1)%12 == 1){
                coolCol = ofColor(0,255,0);
            } else {
                coolCol = ofColor(val2,0,255);
            }
            colorList[i] = coolCol;
            break;
            
            
        case 9 :
            //            coolCol = ofColor::fromHsb(ofMap(isoGerhard[i]%12,0,11,val2,val3),255,255); break; // isoGerhard
            if (std::find(std::begin(whiteNotes), std::end(whiteNotes), isoGerhard[i]%12) != std::end(whiteNotes)){
                coolCol = ofColor(255,0,val3);
            }else if (isoGerhard[i]%12 == 1){
                coolCol = ofColor(0,255,0);
            } else {
                coolCol = ofColor(val2,0,255);
            }
            colorList[i] = coolCol;
            break;
            
            
        case 10 : coolCol = ofColor::fromHsb(ofMap(thunder[i],1,layout.myShapes.size(),val2,val3),255,255);colorList[i] = coolCol; break; // thunder
        case 11 : coolCol = ofColor::fromHsb(ofMap(birdX[i],0,layout.myShapes.size(),val2,val3),255,255);colorList[i] = coolCol; break; // birdX
            
        case 12 :
            //            coolCol = ofColor::fromHsb(ofMap(janko[i]%12,0,11,val2,val3),255,255); break; // Janko
            if (std::find(std::begin(whiteNotes), std::end(whiteNotes), janko[i]%12) != std::end(whiteNotes)){
                coolCol = ofColor(255,0,val2);
            }else if (janko[i]%12 == 1){
                coolCol = ofColor(0,255,0);
            } else {
                coolCol = ofColor(val3,0,255);
            }
            colorList[i] = coolCol;
            break;
            
        case 13 : coolCol = ofColor::fromHsb(ofMap(able[i],0,layout.myShapes.size(),val2,val3),255,255);colorList[i] = coolCol; break; // Ableton
        case 14 : coolCol = ofColor::fromHsb(ofMap(avicii[i],0,layout.myShapes.size(),val2,val3),255,255);colorList[i] = coolCol; break; // Avicii
        case 15 : coolCol = ofColor::fromHsb(ofMap(skrillex[i],0,layout.myShapes.size(),val2,val3),255,255);colorList[i] = coolCol; break; // Skrillex
        case 16 : coolCol = ofColor::fromHsb(ofMap(i,0,layout.myShapes.size(),val2,val3),255,255);colorList[i] = coolCol; break; // Charts
        case 17 : coolCol = ofColor::fromHsb(ofMap(i,0,layout.myShapes.size(),val2,val3),255,255);colorList[i] = coolCol; break; //
        case 18 : coolCol = ofColor::fromHsb(ofMap(i,0,layout.myShapes.size(),val2,val3),255,255);colorList[i] = coolCol; break; //
            
    }
    
}

//--------------------------------------------------------------

void ofApp::SelectLayoutInterface(){
    
    if (!isRandom){// if it's not a randomly generated interface
        // change interface from my interface list
        if (uLeft.inside(blobLocation.x,blobLocation.y) && blobArea <= minBlobSize){//up left
            if (canClick[0]){
                canClick[0] = false;
                if (layout.currentImage > 0){
                    layout.currentImage --;
                    layout.currentImage %= layout.dir.size();
                    
                }else {
                    layout.currentImage = layout.dir.size()-1;
                    layout.currentImage %= layout.dir.size();
                }
            }
            ccVal = layout.currentImage;
            mout.sendControlChange(1, 119, ccVal);
        }else{
            canClick[0]=true;
        }
        
        
        if (uRight.inside(blobLocation.x,blobLocation.y) && blobArea <= minBlobSize){//up right
            if(canClick[1]){
                canClick[1] = false;
                layout.currentImage ++;
                layout.currentImage %= layout.dir.size();
            }
            ccVal = layout.currentImage;
            mout.sendControlChange(1, 119, ccVal);
        }else{
            canClick[1]=true;
        }
        
        layout.cleanShapes();
        layout.findShapesInImage(layout.images[layout.currentImage]);
        layout.makeShapesFromBlobs();
        
    } // if it's not a random interface
    
    
    // change musical instrument in Ableton using MIDI Control Change msg
    if (dLeft.inside(blobLocation.x,blobLocation.y) && blobArea <= minBlobSize){//down left
        if(canClick[2]){
            canClick[2] = false;
            if ( ccVal > 0){
                ccVal --;
            }else{
                ccVal= instNum;
            }
            mout.sendControlChange(1, 119, ccVal);
        }
    }else{
        canClick[2]=true;
    }
    
    if (dRight.inside(blobLocation.x,blobLocation.y) && blobArea <= minBlobSize){//down right
        if(canClick[3]){
            canClick[3] = false;
            
            ofBackground(255);
            if ( ccVal < instNum){
                ccVal ++;
            }else{
                ccVal= 0;
            }
            mout.sendControlChange(1, 119, ccVal);
        }
    }else{
        canClick[3]=true;
    }
    
}


//--------------------------------------------------------------
void ofApp::updateHomography(){
    
    kinect.update();
    
    if (kinect.isFrameNew()){
        
        // creating an image from videos
        
        camImage.begin();
        kinect.draw(0, 0);
        camImage.end();
        
        homo.update(camImage); // processing the original image in the homography class
        
        //homography update
        
        if (homoUp1 && homoUp2){
            
            mainOut.begin();
            homo.draw(); // drawing out the acual aligned image back
            mainOut.end();
            
            // processing only the actual space i'm at (the screen):
            backDiff.update(mainOut,backDiffThres,blurVal,minBlob,maxBlob);
            
        }
        
    }// close new frame
    
}


//--------------------------------------------------------------


void ofApp::initImg(ofFbo input){
    ofPixels locPix;
    input.readToPixels(locPix);
    initImage.setFromPixels(locPix);
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
            initImg(camImage);
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
        case 'F' :
            ofToggleFullscreen();
            break;
            
            
            //additional layout options:
            
        case '0': //open layout maker settings
            isMaker = !isMaker;
            newNotes.resize(layout.myShapes.size());
            for (int i = 0; i<newNotes.size();i++){
                newNotes[i] = i;
            }
            break;
        case ' ': // change layouts one after another
            isRandom = false;
            if (layout.dir.size() > 0){
                layout.currentImage++;
                layout.currentImage %= layout.dir.size();
            }
            ccVal = layout.currentImage;
            mout.sendControlChange(1, 119, ccVal);
            layout.cleanShapes();
            layout.findShapesInImage(layout.images[layout.currentImage]);
            layout.makeShapesFromBlobs();
            break;
        case 'r': //generate random layout with x shapes in it.
            ofBackground(0);
            layout.randLayout(2);
            isRandom = true;
            break;
            
            // computer vision settings and tools:
            
        case 'b': //set background for back differencing
            backDiff.setBack();
            break;
        case 'c': // clear screen
            ofPushStyle();
            ofSetColor(0);
            ofDrawRectangle(0, 0, ofGetWidth(), ofGetHeight());
            ofPopStyle();
            break;
        case 'q': // set blob size
            minBlobSize.set(blobArea);
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
    
    
}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){
    
    homo.mouseReleased(x, y, button);
    musClicked = false;
    
    
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
