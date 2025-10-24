#include "PlayerAudio.h"

PlayerAudio::PlayerAudio(){
    formatManager.registerBasicFormats();
}

PlayerAudio::~PlayerAudio(){

}

void PlayerAudio::prepareToPlay(int samplesPerBlockExpected, double sampleRate){
    transportSource.prepareToPlay(samplesPerBlockExpected, sampleRate);
}

void PlayerAudio::getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill){
    transportSource.getNextAudioBlock(bufferToFill);
    checkAndHandleLooping();
}

void PlayerAudio::releaseResources(){
    transportSource.releaseResources();
}

bool PlayerAudio::loadFile(const juce::File& file){
    if (file.existsAsFile()){
        if (auto* reader = formatManager.createReaderFor(file)){
            // 🔑 Disconnect old source first
            transportSource.stop();
            transportSource.setSource(nullptr);
            readerSource.reset();

            // Create new reader source
            readerSource = std::make_unique<juce::AudioFormatReaderSource>(reader, true);

            // Attach safely
            transportSource.setSource(readerSource.get(),
            0,
            nullptr,
            reader->sampleRate);
            transportSource.start();
        }
    }
    return true;
}

void PlayerAudio::start(){
    transportSource.start();
}

void PlayerAudio::stop(){
    transportSource.stop();
}

void PlayerAudio::setGain(float gain){
    //save current audio
    currentGain = gain;    
    if (isMuted){
        isMuted = false;
    }

    transportSource.setGain(gain);
}

void PlayerAudio::setPosition(double pos){
    transportSource.setPosition(pos);
}

double PlayerAudio::getPosition() const{
    return transportSource.getCurrentPosition();
}

double PlayerAudio::getLength() const{
    return transportSource.getLengthInSeconds();
}

void PlayerAudio::setLooping(bool shouldLoop){
    isLooping = shouldLoop;
}

//loop effect
void PlayerAudio::checkAndHandleLooping(){
    if (isLooping){
        if (transportSource.getCurrentPosition() >= transportSource.getLengthInSeconds()){
            transportSource.setPosition(0.0);
            transportSource.start();
        }
    }
}

//Mute effect
void PlayerAudio::setMute(bool shouldMute){
    if (shouldMute && !isMuted){
        lastGainBeforeMute = currentGain;  
        transportSource.setGain(0.0);    
        isMuted = true;
    }

    else if(!shouldMute && isMuted){
        transportSource.setGain(lastGainBeforeMute); 
        currentGain = lastGainBeforeMute;
        isMuted = false;
    }
}
