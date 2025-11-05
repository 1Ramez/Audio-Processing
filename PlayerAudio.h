#pragma once
#include <JuceHeader.h>

class PlayerAudio
{
public:
    PlayerAudio();
    ~PlayerAudio();

    void prepareToPlay(int samplesPerBlockExpected, double sampleRate);
    void getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill);
    void releaseResources();

    bool loadFile(const juce::File& file);
    bool readMeta(const juce::File& file);
    void start();
    void stop();
    void setGain(float gain);
    void setPosition(double pos);
    double getPosition() const;
    double getLength() const;
    void setLooping(bool shouldLoop);
    void handleEnd();
    void setMute(bool shouldMute);
    void loadPlaylist(const juce::Array<juce::File> &files);
    void playNext();
    void playPrevious();
    void delPlaylist();
    void setLoopStartPoint();
    void setLoopEndPoint();
    void setABLooping(bool shouldABLoop);
    bool isPlaying();
    void jumpForward(double seconds);
    void jumpBackward(double seconds);
    void setPlaybackSpeed(double newSpeed);
    double getPlaybackSpeed();
    bool getTrackChanged();
    void setTrackChanged(bool changed);
    juce::File getCurrentFile();

    juce::String getTitle() const;
    juce::String getAuthor() const;
    juce::String getDurationText() const;

private:
    juce::AudioFormatManager formatManager;
    std::unique_ptr<juce::AudioFormatReaderSource> readerSource;
    juce::AudioTransportSource transportSource;

    double loopStartSeconds = 0.0;
    double loopEndSeconds = 0.0;
    double playbackSpeed = 1.0;
    bool isABLoopingActive = false;
    bool isLooping = false;
    bool trackChanged = false;
    float currentGain = 0.5;
    float lastGainBeforeMute = 0.5;
    juce::String title;
    juce::String author;
    juce::String durationText;
    juce::Array<juce::File> playlistFiles;
    int currFileIndex = -1;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PlayerAudio)
};