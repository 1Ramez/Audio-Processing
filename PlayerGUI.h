#pragma once
#include <JuceHeader.h>
#include "PlayerAudio.h"

class PlayerGUI : public juce::Component,
    public juce::Button::Listener,
    public juce::Slider::Listener,
    public juce::Timer,
    public juce::ChangeListener
{
public:
    PlayerGUI();
    ~PlayerGUI() override;

    void resized() override;
    void paint(juce::Graphics& g) override;

    void prepareToPlay(int samplesPerBlockExpected, double sampleRate);
    void getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill);
    void releaseResources();
    void displayMeta();
    void timerCallback() override;
    bool keyPressed (const juce::KeyPress& key) override;
    void changeListenerCallback(juce::ChangeBroadcaster* source) override;
    void updateThumbnail();


private:
    PlayerAudio playerAudio;

    //GUI elements
    //Buttons
    juce::TextButton loadButton{ "Load File" };
    juce::TextButton restartButton{ "Restart" };
    juce::TextButton stopButton{ "Stop" };
    juce::TextButton playButton { "Play" };
    juce::TextButton pauseButton { "Pause" };
    juce::TextButton loopButton { "Loop: OFF" };
    juce::TextButton toStartButton { "To Start" };
    juce::TextButton toEndButton { "To End" };
    juce::TextButton muteButton { "Mute" };
    juce::TextButton loadPlaylistButton { "Load Playlist" };
    juce::TextButton nextButton { "Next" };
    juce::TextButton previousButton { "Previous" };
    juce::TextButton setAButton{ "Set - A" };
    juce::TextButton setBButton{ "Set - B" };
    juce::TextButton loopABButton{ "ABLoop: OFF" };
    juce::TextButton forward10Button { "=>> +10s" };
    juce::TextButton backward10Button { "<<= -10s" };


    //Sliders
    juce::Slider volumeSlider;
    juce::Slider positionSlider;
    juce::Slider speedSlider;

    //Labels
    juce::Label titleLabel;
    juce::Label authorLabel;
    juce::Label durationLabel;
    juce::Label speedLabel;
    juce::Label volumeLabel;

    //Waveform 
    juce::AudioThumbnailCache thumbnailCache{5};
    juce::AudioFormatManager formatManager;
    juce::AudioThumbnail thumbnail{512, formatManager, thumbnailCache};
    double currentPosInTrack = 0.0;


    //Variables
    bool shouldLoop = false; //Keeps track of whether looping is enabled or not
    bool isMuted = false;
    bool isLoopAB = false;

    //File Chooser
    std::unique_ptr<juce::FileChooser> fileChooser;

    //Event handlers
    void buttonClicked(juce::Button* button) override;
    void sliderValueChanged(juce::Slider* slider) override;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PlayerGUI)
};