#include "PlayerGUI.h"

PlayerGUI::PlayerGUI(){
    // Add buttons
    for (auto* btn : {&loadButton, &restartButton, &stopButton, &playButton, &pauseButton, &loopButton} ){
        btn->addListener(this);
        addAndMakeVisible(btn);
    }

    // Volume slider
    volumeSlider.setRange(0.0, 1.0, 0.01);
    volumeSlider.setValue(0.5);
    addAndMakeVisible(volumeSlider);
    volumeSlider.addListener(this);
}

PlayerGUI::~PlayerGUI(){
}

void PlayerGUI::resized(){
    int y = 20;
    loadButton.setBounds(20, y, 100, 40);
    restartButton.setBounds(140, y, 80, 40);
    stopButton.setBounds(240, y, 80, 40);
    playButton.setBounds(340,y,80,40);
    pauseButton.setBounds(440,y,80,40);
    loopButton.setBounds(540, y, 80, 40);

    volumeSlider.setBounds(20, 100, getWidth() - 40, 30);
}

void PlayerGUI::paint(juce::Graphics& g){
    g.fillAll(juce::Colours::blueviolet);
}

void PlayerGUI::prepareToPlay(int samplesPerBlockExpected, double sampleRate){
    playerAudio.prepareToPlay(samplesPerBlockExpected, sampleRate);
}

void PlayerGUI::getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill){
    playerAudio.getNextAudioBlock(bufferToFill);

}

void PlayerGUI::releaseResources(){
    playerAudio.releaseResources();
}

void PlayerGUI::buttonClicked(juce::Button* button){
    if (button == &loadButton){
        juce::FileChooser chooser("Select audio files...",
            juce::File{},
            "*.wav;*.mp3");

        fileChooser = std::make_unique<juce::FileChooser>(
            "Select an audio file...",
            juce::File{},
            "*.wav;*.mp3");

        fileChooser->launchAsync(
            juce::FileBrowserComponent::openMode | juce::FileBrowserComponent::canSelectFiles,
            [this](const juce::FileChooser& fc){
                auto file = fc.getResult();
                if (file.existsAsFile()){
                    playerAudio.loadFile(file);
                }
            });

    }

    if (button == &restartButton){
        playerAudio.start();
    }

    if (button == &stopButton){
        playerAudio.stop();
        playerAudio.setPosition(0.0);
    }

    if (button == &playButton){
        playerAudio.start();
    }

    if (button == &pauseButton){
        playerAudio.stop();
    }

    if (button == &loopButton){
        shouldLoop = !shouldLoop; // loop status (on/off)
        playerAudio.setLooping(shouldLoop); //pass the value

        if (shouldLoop)
            loopButton.setButtonText("Loop: ON");  //to show it's active
        else
            loopButton.setButtonText("Loop: OFF"); // to show it's off
    }
    
}

void PlayerGUI::sliderValueChanged(juce::Slider* slider){
    if (slider == &volumeSlider)
        playerAudio.setGain((float)slider->getValue());
}