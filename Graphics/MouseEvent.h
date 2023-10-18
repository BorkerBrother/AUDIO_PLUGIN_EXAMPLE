//
// Created by Kai on 18.10.23.
//

#ifndef AUDIO_PLUGIN_EXAMPLE_MOUSEEVENT_H
#define AUDIO_PLUGIN_EXAMPLE_MOUSEEVENT_H

#include <juce_audio_processors/juce_audio_processors.h>

class MouseEvent  {


public:
    MouseEvent ();

    void updateParam();

    int getMouseX() const {
        return mouseX;
    }

    int getMouseY() const {
        return mouseY;
    }


    //AudioPluginAudioProcessor &processorRef;

private:
    juce::Point<int> mousePos = juce::Desktop::getInstance().getMousePosition();




    int mouseX = mousePos.getX();
    int mouseY = mousePos.getY();

};

#endif //AUDIO_PLUGIN_EXAMPLE_MOUSEEVENT_H
