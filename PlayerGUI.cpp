#include "PlayerGUI.h"

PlayerGUI::PlayerGUI(){
    // Add buttons
    for (auto* btn : {&loadButton, &restartButton, &stopButton, &playButton,
            &pauseButton, &loopButton, &toStartButton, &toEndButton, &muteButton, &loadPlaylistButton, &nextButton, &previousButton}){

        btn->addListener(this);
        addAndMakeVisible(btn);
    }

    //Add Labels
    for (auto* label : {&titleLabel, &authorLabel, &durationLabel}){
        addAndMakeVisible(label);
        label->setColour(juce::Label::backgroundColourId, juce::Colours::darkslategrey);
        label->setEditable(false,false,false);
        label->setJustificationType(juce::Justification::centred);
    }
    titleLabel.setText("Title: ", juce::dontSendNotification);
    authorLabel.setText("Author: ", juce::dontSendNotification);
    durationLabel.setText("Duration: ", juce::dontSendNotification);

    // Volume slider
    volumeSlider.setRange(0.0, 1.0, 0.01);
    volumeSlider.setValue(0.5);
    addAndMakeVisible(volumeSlider);
    volumeSlider.addListener(this);
}

PlayerGUI::~PlayerGUI(){
}

void PlayerGUI::resized(){

    //Row 1
    int y = 20, w = 80, h = 40;
    loadButton.setBounds(20, y, w+20 , h);
    restartButton.setBounds(140, y, w, h);
    stopButton.setBounds(240, y, w, h);
    playButton.setBounds(340, y, w, h);
    pauseButton.setBounds(440, y, w, h);
    loopButton.setBounds(540, y, w, h);
    muteButton.setBounds(640, y, w, h);
    titleLabel.setBounds(740,y,2*w,h);
    authorLabel.setBounds(920,y,2*w,h);
    durationLabel.setBounds(1100,y,w+20,h);

    //Row 2
    int y2 = y + h + 10;
    toStartButton.setBounds(20, y2, w+20, h);
    toEndButton.setBounds(140, y2, w, h);
    loadPlaylistButton.setBounds(240,y2,w+40,h);
    nextButton.setBounds(380, y2, w, h);
    previousButton.setBounds(480, y2, w, h);

    

    volumeSlider.setBounds(20, y2 + 60, getWidth() - 40, 30);
}

void PlayerGUI::paint(juce::Graphics& g){
    g.fillAll(juce::Colours::mediumslateblue);
}

void PlayerGUI::prepareToPlay(int samplesPerBlockExpected, double sampleRate){
    playerAudio.prepareToPlay(samplesPerBlockExpected, sampleRate);
}

void PlayerGUI::getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill){
    playerAudio.getNextAudioBlock(bufferToFill);
    displayMeta();
}

void PlayerGUI::releaseResources(){
    playerAudio.releaseResources();
}

void PlayerGUI::displayMeta(){
    titleLabel.setText("Title: " + playerAudio.getTitle(), juce::dontSendNotification);
    authorLabel.setText("Author: " + playerAudio.getAuthor(), juce::dontSendNotification);
    durationLabel.setText("Duration: " + playerAudio.getDurationText(), juce::dontSendNotification);
}

void PlayerGUI::buttonClicked(juce::Button* button){
    if (button == &loadButton){
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

                    //Reading Meta data
                    playerAudio.readMeta(file);

                    //Showing the metadata on labels
                    displayMeta();

                    //To cancel previous playlist
                    playerAudio.delPlaylist();
                }
            });

    }

    if (button == &restartButton){
        playerAudio.setPosition(0.0);
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

    if (button == &toStartButton){
        playerAudio.setPosition(0.0);
    }

    if (button == &toEndButton){
        double length = playerAudio.getLength();
        if (length > 0.0){
            playerAudio.setPosition(length - 0.1);
        }
    }
    
    if (button == &muteButton){
        isMuted = !isMuted;
        playerAudio.setMute(isMuted);
        if (isMuted){          
            muteButton.setButtonText("Unmute"); 
        }
        else{
            muteButton.setButtonText("Mute");   
        }
    }

    if (button == &loadPlaylistButton){
        // juce::FileChooser chooser("Select audio files...",
        //     juce::File{},
        //     "*.wav;*.mp3");

        fileChooser = std::make_unique<juce::FileChooser>(
            "Select multiple audio files...",
            juce::File{},
            "*.wav;*.mp3");

        fileChooser->launchAsync(
            juce::FileBrowserComponent::openMode | juce::FileBrowserComponent::canSelectMultipleItems,
            [this](const juce::FileChooser& fc){
                auto files = fc.getResults();

                if (files.isEmpty()){
                    return;
                }

                // Call PlayerAudio to handle playlist logic
                playerAudio.loadPlaylist(files);

                // Update metadata labels for the first file
                displayMeta();
            });
    }

    if (button == &nextButton){
        playerAudio.playNext();
        displayMeta();
    }

    if (button == &previousButton){
        playerAudio.playPrevious();
        displayMeta();
    }

}

void PlayerGUI::sliderValueChanged(juce::Slider* slider){
    if (slider == &volumeSlider){
        playerAudio.setGain((float)slider->getValue());
        muteButton.setButtonText("Mute");
        isMuted = false;
    }

}