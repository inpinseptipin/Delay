/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
DelayEditor::DelayEditor (DelayAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
    
    auto& params = processor.getParameters();
    juce::AudioParameterFloat* p_dryWet = (juce::AudioParameterFloat*)params.getUnchecked(0);
    juce::AudioParameterFloat* p_feedback = (juce::AudioParameterFloat*)params.getUnchecked(1);
    juce::AudioParameterFloat* p_delayTime = (juce::AudioParameterFloat*)params.getUnchecked(2);

    
    setSize (400, 300);

    addAndMakeVisible(m_header);
    m_header.setText("Plugin:Delay", juce::dontSendNotification);
    m_header.setColour(juce::Label::ColourIds::backgroundColourId, juce::Colours::black);
    m_header.setColour(juce::Label::ColourIds::outlineColourId, juce::Colours::blue);
    m_header.setColour(juce::Label::ColourIds::textColourId, juce::Colours::white);
    m_header.setJustificationType(juce::Justification::horizontallyCentred);
    m_header.setFont(juce::Font(32.0f, 1));

    addAndMakeVisible(m_documentation);
    m_documentation.setButtonText("Click Here to View the Documentation");
    m_documentation.setColour(juce::TextButton::ColourIds::buttonColourId, juce::Colours::grey);
    m_documentation.setColour(juce::TextButton::ColourIds::textColourOffId, juce::Colours::blue);

 //=================================================Dry Wet Slider==================================================
    addAndMakeVisible(m_DryWet);
    m_DryWet.setSliderStyle(juce::Slider::SliderStyle::LinearBarVertical);
    m_DryWet.setTextBoxStyle(juce::Slider::TextEntryBoxPosition::TextBoxBelow, true, getRight()/4, 30);
    m_DryWet.setColour(juce::Slider::ColourIds::trackColourId, juce::Colours::blue);
    m_DryWet.setColour(juce::Slider::ColourIds::backgroundColourId, juce::Colours::black);
    m_DryWet.setRange(p_dryWet->range.start, p_dryWet->range.end);
    m_DryWet.setValue(*p_dryWet);

    addAndMakeVisible(ml_DryWet);
    ml_DryWet.setText("Dry Wet", juce::dontSendNotification);
    ml_DryWet.setColour(juce::Label::ColourIds::backgroundColourId, juce::Colours::black);
    ml_DryWet.setColour(juce::Label::ColourIds::outlineColourId, juce::Colours::blue);
    ml_DryWet.setColour(juce::Label::ColourIds::textColourId, juce::Colours::white);
    ml_DryWet.setJustificationType(juce::Justification::horizontallyCentred);


    m_DryWet.onValueChange = [this, p_dryWet] {*p_dryWet = m_DryWet.getValue(); };
    m_DryWet.onDragStart = [p_dryWet] {p_dryWet->beginChangeGesture(); };
    m_DryWet.onDragEnd = [p_dryWet] {p_dryWet->endChangeGesture(); };


//==================================================================================================================

//=================================================Delay Slider=====================================================

    addAndMakeVisible(m_Delay);
    m_Delay.setSliderStyle(juce::Slider::SliderStyle::LinearBarVertical);
    m_Delay.setTextBoxStyle(juce::Slider::TextEntryBoxPosition::TextBoxBelow, true, getRight() / 4, 30);
    m_Delay.setColour(juce::Slider::ColourIds::trackColourId, juce::Colours::blue);
    m_Delay.setColour(juce::Slider::ColourIds::backgroundColourId, juce::Colours::black);
    m_Delay.setRange(p_delayTime->range.start, p_delayTime->range.end);
    m_Delay.setValue(*p_delayTime);

    m_Delay.onValueChange = [this, p_delayTime] {*p_delayTime = m_Delay.getValue(); };
    m_Delay.onDragStart = [p_delayTime] {p_delayTime->beginChangeGesture(); };
    m_Delay.onDragEnd = [p_delayTime] {p_delayTime->endChangeGesture(); };

    addAndMakeVisible(ml_Delay);
    ml_Delay.setText("Delay", juce::dontSendNotification);
    ml_Delay.setColour(juce::Label::ColourIds::backgroundColourId, juce::Colours::black);
    ml_Delay.setColour(juce::Label::ColourIds::outlineColourId, juce::Colours::blue);
    ml_Delay.setColour(juce::Label::ColourIds::textColourId, juce::Colours::white);
    ml_Delay.setJustificationType(juce::Justification::horizontallyCentred);

//==================================================================================================================

//=================================================Feedback Slider==================================================

    addAndMakeVisible(m_Feedback);
    m_Feedback.setSliderStyle(juce::Slider::SliderStyle::LinearBarVertical);
    m_Feedback.setTextBoxStyle(juce::Slider::TextEntryBoxPosition::TextBoxBelow, true, getRight() / 4, 30);
    m_Feedback.setColour(juce::Slider::ColourIds::trackColourId, juce::Colours::blue);
    m_Feedback.setColour(juce::Slider::ColourIds::backgroundColourId, juce::Colours::black);
    m_Feedback.setRange(p_feedback->range.start, p_feedback->range.end);
    m_Feedback.setValue(*p_feedback);

    m_Feedback.onValueChange = [this, p_feedback] {*p_feedback = m_Delay.getValue(); };
    m_Feedback.onDragStart = [p_feedback] {p_feedback->beginChangeGesture(); };
    m_Feedback.onDragEnd = [p_feedback] {p_feedback->endChangeGesture(); };

    addAndMakeVisible(ml_Feedback);
    ml_Feedback.setText("Feedback", juce::dontSendNotification);
    ml_Feedback.setColour(juce::Label::ColourIds::backgroundColourId, juce::Colours::black);
    ml_Feedback.setColour(juce::Label::ColourIds::outlineColourId, juce::Colours::blue);
    ml_Feedback.setColour(juce::Label::ColourIds::textColourId, juce::Colours::white);
    ml_Feedback.setJustificationType(juce::Justification::horizontallyCentred);

    //==================================================================================================================


}

DelayEditor::~DelayEditor()
{
}

//==============================================================================
void DelayEditor::paint (juce::Graphics& g)
{
    g.setColour(juce::Colours::black);
    g.fillAll();
}

void DelayEditor::resized()
{
    m_header.setBounds(0, 0, getRight(), 2 * getBottom() / 10);

    m_DryWet.setBounds(0, m_header.getBottom(), getRight() / 3, 6*getBottom() / 10);
    ml_DryWet.setBounds(0, m_DryWet.getBottom(), getRight() / 3, getBottom() / 10);

    m_Delay.setBounds(m_DryWet.getRight(), m_header.getBottom(), getRight() / 3, 6 * getBottom() / 10);
    ml_Delay.setBounds(ml_DryWet.getRight(), m_DryWet.getBottom(), getRight() / 3, getBottom() / 10);

    m_Feedback.setBounds(m_Delay.getRight(), m_header.getBottom(), getRight() / 3, 6 * getBottom() / 10);
    ml_Feedback.setBounds(ml_Delay.getRight(), m_Delay.getBottom(), getRight() / 3, getBottom() / 10);


    m_documentation.setBounds(0, 9*getBottom()/10, getRight(), getBottom() / 10);
}


