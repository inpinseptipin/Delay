/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
DelayAudioProcessor::DelayAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       )
#endif
{
    addParameter(mp_DryWet = new juce::AudioParameterFloat("drywet", "Dry Wet", 0.0, 1.0, 0.5));
    addParameter(mp_Feedback = new juce::AudioParameterFloat("feedback", "Feedback", 0, 0.98, 0.5));
    addParameter(mp_DelayTime = new juce::AudioParameterFloat("delay", "Delay", 0.01, MaxDelay, 0.5));

    m_circularbufferLeft = nullptr;
    m_circularbufferRight = nullptr;
    m_bufferHead = 0;
    m_feedbackLeft = 0;
    m_feedbackRight = 0;
    m_delayHead = 0;
    m_delaySmooth = 0;
    //m_dryWet = 0.5;
}

DelayAudioProcessor::~DelayAudioProcessor()
{
    if (m_circularbufferLeft != nullptr)
    {
        delete[] m_circularbufferLeft;
        m_circularbufferLeft = nullptr;
    }

    if (m_circularbufferRight != nullptr)
    {
        delete[] m_circularbufferRight;
        m_circularbufferRight = nullptr;
    }
}

//==============================================================================
const juce::String DelayAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool DelayAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool DelayAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool DelayAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double DelayAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int DelayAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int DelayAudioProcessor::getCurrentProgram()
{
    return 0;
}

void DelayAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String DelayAudioProcessor::getProgramName (int index)
{
    return {};
}

void DelayAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void DelayAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    m_bufferSize = (int)sampleRate * MaxDelay;
    m_delayTime = sampleRate * *mp_DelayTime;

    if (m_circularbufferLeft == nullptr)
    {
        m_circularbufferLeft = new float[m_bufferSize];
        std::fill_n(m_circularbufferLeft, m_bufferSize,0);
    }
    std::fill_n(m_circularbufferLeft, m_bufferSize, 0);

    if (m_circularbufferRight == nullptr)
    {
        m_circularbufferRight = new float[m_bufferSize];
        std::fill_n(m_circularbufferRight, m_bufferSize, 0);
    }
    std::fill_n(m_circularbufferRight, m_bufferSize, 0);

    m_bufferHead = 0;

    m_delaySmooth = *mp_DelayTime;

}

void DelayAudioProcessor::releaseResources()
{
    if (m_circularbufferLeft != nullptr)
    {
        delete[] m_circularbufferLeft;
        m_circularbufferLeft = nullptr;
    }

    if (m_circularbufferRight != nullptr)
    {
        delete[] m_circularbufferRight;
        m_circularbufferRight = nullptr;
    }
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool DelayAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}
#endif

void DelayAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());

    

    float* leftChannel = buffer.getWritePointer(0);
    float* RightChannel = buffer.getWritePointer(1);

    for (int i = 0; i < buffer.getNumSamples(); i++)
    {
        m_delayTime = getSampleRate() * m_delaySmooth;

        m_delaySmooth = m_delaySmooth - 0.001 * (m_delaySmooth - *mp_DelayTime);
        
        m_circularbufferLeft[m_bufferHead] = leftChannel[i] + m_feedbackLeft;
        m_circularbufferRight[m_bufferHead] = RightChannel[i] + m_feedbackRight;

        m_delayHead = m_bufferHead - m_delayTime;

        if (m_delayHead < 0)
        {
            m_delayHead += m_bufferSize;
        }

        m_linearOP = LinearInterpolation::doPolation(m_circularbufferLeft, m_circularbufferRight, m_delayHead, m_bufferSize);

        //m_feedbackLeft = m_circularbufferLeft[(int)m_delayHead] * *mp_Feedback;
        //m_feedbackRight = m_circularbufferRight[(int)m_delayHead] * *mp_Feedback;

        m_feedbackLeft = m_linearOP.left * *mp_Feedback;
        m_feedbackRight = m_linearOP.right * *mp_Feedback;

        //buffer.addSample(0,i, m_circularbufferLeft[(int)m_delayHead]);
        //buffer.addSample(1, i, m_circularbufferRight[(int)m_delayHead]);

        buffer.setSample(0, i, buffer.getSample(0, i) * (1 - *mp_DryWet) + m_linearOP.left * *mp_DryWet);
        buffer.setSample(1, i, buffer.getSample(1, i) * (1 - *mp_DryWet) + m_linearOP.right * *mp_DryWet);




        m_bufferHead++;

        if (m_bufferHead >= m_bufferSize)
        {
            m_bufferHead = 0;
        }
    }
}

//==============================================================================
bool DelayAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* DelayAudioProcessor::createEditor()
{
    return new DelayEditor (*this);
}

//==============================================================================
void DelayAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void DelayAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new DelayAudioProcessor();
}

LinearInterpolation LinearInterpolation::doPolation(float* circularBufferLeft, float* circularBufferRight, float delayHead, unsigned int bufferSize)
{
    int readHead_x = (int)delayHead;
    int readHead_x1 = readHead_x + 1;

    if (readHead_x1 >= bufferSize)
    {
        readHead_x1 -= bufferSize;
    }

   
    float phase = delayHead - readHead_x;

    LinearInterpolation obj;
    obj.left = (1 - phase) * circularBufferLeft[readHead_x] + phase * circularBufferLeft[readHead_x1];
    obj.right = (1 - phase) * circularBufferRight[readHead_x] + phase * circularBufferRight[readHead_x1];
    return obj;
}