//
//  midiOutClass.cpp
//  MidiPolyNew
//
//  Created by Amit Segall on 23/07/2018.
//
//

#include "midiOutClass.h"

void MidiOutClass::setup(){
    
    mout.openPort(0);
}


void MidiOutClass::sendNoteOn(int midiCh, int noteNum, int velocity, int duration){
    mout.sendNoteOn(midiCh, noteNum, velocity);
    std::thread noteOffThread( &MidiOutClass::sendNoteOff, this, midiCh, noteNum, duration);
    noteOffThread.detach();
}

void MidiOutClass::sendNoteOff(int midiCh, int noteNum, int duration){
    if(duration>0)
        std::this_thread::sleep_for( chrono::microseconds(duration*1000) );
    
    vector<unsigned char> noteOff;
    noteOff.push_back(MIDI_NOTE_OFF+(midiCh-1));
    noteOff.push_back(noteNum);
    noteOff.push_back(0);
    ofPtr<ofxBaseMidiOut> ofOut = mout.midiOut;
    ofxRtMidiOut * rtOut = static_cast<ofxRtMidiOut*>(ofOut.get());
    rtOut->midiOut.sendMessage(&noteOff);
}

void MidiOutClass::sendControlChange(int midiCh, int ccNum, int val){
    mout.sendControlChange(midiCh, ccNum, val);
}
