//
//  midiOutClass.hpp
//  MidiPolyNew
//
//  Created by Amit Segall on 23/07/2018.
//
//

#ifndef midiOutClass_hpp
#define midiOutClass_hpp

#include <stdio.h>
#include "ofMain.h"
#include "ofxMidi.h"

class MidiOutClass {
    
public:
    
    ofxMidiOut mout;
    
    
    void setup();
    void sendNoteOn(int midiCh, int noteNum, int velocity, int duration);
    void sendNoteOff(int midiCh, int noteNum, int duration);
    void sendControlChange(int midiCh, int ccNum, int val);
    
};



#endif /* midiOutClass_hpp */
