/*
  ==============================================================================

  This is an automatically generated file created by the Jucer!

  Creation date:  4 Jan 2013 3:44:07pm

  Be careful when adding custom code to these files, as only the code within
  the "//[xyz]" and "//[/xyz]" sections will be retained when the file is loaded
  and re-saved.

  Jucer version: 1.12

  ------------------------------------------------------------------------------

  The Jucer is part of the JUCE library - "Jules' Utility Class Extensions"
  Copyright 2004-6 by Raw Material Software ltd.

  ==============================================================================
*/

#ifndef __JUCER_HEADER_MAINPANEL_MAINPANEL_38A7007A__
#define __JUCER_HEADER_MAINPANEL_MAINPANEL_38A7007A__

//[Headers]     -- You can add your own extra header files here --
#include "../JuceLibraryCode/JuceHeader.h"
#include "PluginProcessor.h"
//[/Headers]



//==============================================================================
/**
                                                                    //[Comments]
    An auto-generated component, created by the Jucer.

    Describe your class and how it works here!
                                                                    //[/Comments]
*/
class MainPanel  : public Component,
                   public Timer,
                   public SliderListener
{
public:
    //==============================================================================
    MainPanel (ConvolutionAudioProcessor* ptr);
    ~MainPanel();

    //==============================================================================
    //[UserMethods]     -- You can add your own custom methods in this section.
	virtual void timerCallback();
    //[/UserMethods]

    void paint (Graphics& g);
    void resized();
    void sliderValueChanged (Slider* sliderThatWasMoved);

    // Binary resources:
    static const char* lionfishlogo_png;
    static const int lionfishlogo_pngSize;
    static const char* laclogo_gif;
    static const int laclogo_gifSize;


    //==============================================================================
    juce_UseDebuggingNewOperator

private:
    //[UserVariables]   -- You can add your own custom variables in this section.
	ConvolutionAudioProcessor* processor; // cf. MainPanel.cpp contructor
    //[/UserVariables]

    //==============================================================================
    Label* label;
    Slider* sliderWet;
    Label* label4;


    //==============================================================================
    // (prevent copy constructor and operator= being generated..)
    MainPanel (const MainPanel&);
    const MainPanel& operator= (const MainPanel&);
};


#endif   // __JUCER_HEADER_MAINPANEL_MAINPANEL_38A7007A__
