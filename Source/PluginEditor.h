/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

//==============================================================================
/**
*/
class DelayEditor  : public juce::AudioProcessorEditor
{
public:
    DelayEditor (DelayAudioProcessor&);
    ~DelayEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;
    

private:
    DelayAudioProcessor& audioProcessor;
    juce::TextButton m_documentation;
    juce::Label m_header;

    juce::Label ml_DryWet;
    juce::Slider m_DryWet;
    
    juce::Label ml_Delay;
    juce::Slider m_Delay;
    
    juce::Label ml_Feedback;
    juce::Slider m_Feedback;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (DelayEditor)
};
