#pragma once
#include <JuceHeader.h>
#include "PlayerAudio.h"

class PlayerGUI : public juce::Component,
    public juce::Button::Listener,
    public juce::Slider::Listener
{
public:
    PlayerGUI();
    ~PlayerGUI() override;

    void resized() override;
    void paint(juce::Graphics& g) override;

    void prepareToPlay(int samplesPerBlockExpected, double sampleRate);
    void getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill);
    void releaseResources();

private:
    PlayerAudio playerAudio;

//GUI elements
    //Buttons
    juce::TextButton loadButton{ "Load File" };
    juce::TextButton restartButton{ "Restart" };
    juce::TextButton stopButton{ "Stop" };
    juce::TextButton playButton { "Play" };
    juce::TextButton pauseButton { "Pause" };
    juce::TextButton loopButton { "Loop" };
    juce::TextButton toStartButton { "To Start" };
    juce::TextButton toEndButton { "To End" };
    juce::TextButton muteButton { "Mute" };

    //Sliders
    juce::Slider volumeSlider;

    //Labels
    juce::Label titleLabel;
    juce::Label authorLabel;
    juce::Label durationLabel;

//Variables
    bool shouldLoop = false; //Keeps track of whether looping is enabled or not
    bool isMuted = false;

//File Chooser
    std::unique_ptr<juce::FileChooser> fileChooser;

//Event handlers
    void buttonClicked(juce::Button* button) override;
    void sliderValueChanged(juce::Slider* slider) override;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PlayerGUI)
};