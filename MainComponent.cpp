#include "MainComponent.h"

MainComponent::MainComponent(){
    addAndMakeVisible(player1);
    addAndMakeVisible(player2);
    setSize(500, 250);
    setAudioChannels(0, 2);
}

MainComponent::~MainComponent(){
    shutdownAudio();
}

void MainComponent::prepareToPlay(int samplesPerBlockExpected, double sampleRate){
    player1.prepareToPlay(samplesPerBlockExpected, sampleRate);
    player2.prepareToPlay(samplesPerBlockExpected, sampleRate);
}

void MainComponent::getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill){
    player1.getNextAudioBlock(bufferToFill);
    if(!player1.isPlaying()){
        player2.getNextAudioBlock(bufferToFill);
    }
}

void MainComponent::releaseResources(){
    player1.releaseResources();
    player2.releaseResources();
}

void MainComponent::resized(){
    auto area = getLocalBounds();
    auto topArea = area.removeFromTop(area.getHeight()/2);
    auto botArea = area;

    player1.setBounds(topArea);
    player2.setBounds(botArea);

}