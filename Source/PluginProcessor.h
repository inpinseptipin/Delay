/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

//==============================================================================
/**
*/
#define MaxDelay 2

struct LinearInterpolation
{
    float left;
    float right;
    static LinearInterpolation doPolation(float* circularBufferLeft, float* circularBufferRight, float delayHead, unsigned int bufferSize);
};


class DelayAudioProcessor  : public juce::AudioProcessor
{
public:
    //==============================================================================
    DelayAudioProcessor();
    ~DelayAudioProcessor() override;

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif

    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    //==============================================================================
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;

    //==============================================================================
    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;
    //==============================================================================
    
    

private:
    //==============================================================================
    float m_dryWet;
    float* m_circularbufferLeft;
    float* m_circularbufferRight;
    float m_feedbackLeft;
    float m_feedbackRight;
    float m_delayTime;
    float m_delayHead;
    float m_delaySmooth;

    unsigned int m_bufferHead;
    unsigned int m_bufferSize;
    
    LinearInterpolation m_linearOP;

    juce::AudioParameterFloat* mp_DryWet;
    juce::AudioParameterFloat* mp_Feedback;
    juce::AudioParameterFloat* mp_DelayTime;

    

            
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (DelayAudioProcessor)
};

