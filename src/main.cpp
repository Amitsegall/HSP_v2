#include "ofMain.h"
#include "ofApp.h"

//========================================================================
int main( ){
//    ofSetupOpenGL(640*2,480,OF_WINDOW);            // <-------- setup the GL context

	// this kicks off the running of my app
	// can be OF_WINDOW or OF_FULLSCREEN
	// pass in width and height too:
    
    
    ofGLFWWindowSettings settings;
    settings.setGLVersion(2, 1); //we define the OpenGL version we want to use
    settings.setSize(640*2, 480);
    ofCreateWindow(settings);
    
    
	ofRunApp(new ofApp());

}

/* Hit - Strike - Play, a project by Amit Segall 2018
 
 
 Hit - Strike - Play is an installation exploring layout design and interaction with digital musical instruments using computer vision.
 The installation balances the cognitive and visual feedback of a digital interface with a physical musical instrument.
 While providing users with a familiar tactile design of a percussion instrument, any user is able to play music.
 The installation can beused as  a solo act , a collaboration tool, a studio instrument and for a performative purposes.
 Hit - Strike - Play highlights the interface as the key factor in the user - interface - musical output relationship,
 by allowing users to experiment with different design aesthetics using an intuitive approach and modern technology.
 The different layouts challenge traditional instrument design idioms by allowing total creative freedom while making music in the digital age.
 
 
 www.Amitsegall.com
 
 
 Work in Progress V2
 
 */




