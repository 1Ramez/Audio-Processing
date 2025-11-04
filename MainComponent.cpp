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
    bufferToFill.clearActiveBufferRegion();

    //buffer for player 2
    juce::AudioBuffer<float> buffer2(bufferToFill.buffer->getNumChannels(),bufferToFill.numSamples);
    juce::AudioSourceChannelInfo buffertoFill2(&buffer2, 0, bufferToFill.numSamples);

    //Fill both players
    player1.getNextAudioBlock(bufferToFill);
    player2.getNextAudioBlock(buffertoFill2);

    //Mix player 2 into main buffer
    for (int ch = 0; ch < bufferToFill.buffer->getNumChannels(); ch++){
        bufferToFill.buffer->addFrom(ch, 0, buffer2, ch, 0, bufferToFill.numSamples);
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