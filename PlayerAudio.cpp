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
    handleEnd();

    if (isABLoopingActive && transportSource.isPlaying()){
        if (transportSource.getCurrentPosition() >= loopEndSeconds){
            transportSource.setPosition(loopStartSeconds);
        }
    }

    bufferToFill.buffer->applyGain(bufferToFill.startSample, bufferToFill.numSamples, currentGain);
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

//Reading MetaData and saving it in private strings
bool PlayerAudio::readMeta(const juce::File& file){
    if (file.existsAsFile()){

        if (auto* reader = formatManager.createReaderFor(file)){
            juce::StringPairArray meta = reader->metadataValues;

            title  = meta["title"];
            author = meta["artist"];

            if (title.isEmpty()){
                title  = meta["INAM"];
            }
            if (author.isEmpty()){
                author = meta["IART"];
            }

            //Calculate duration
            double lengthSeconds = transportSource.getLengthInSeconds();
            int seconds = (int) lengthSeconds;
            int minutes = seconds/60;
            seconds = seconds % 60;
            durationText = juce::String::formatted("%d:%02d", minutes, seconds);

            //if data not found
            if (title.isEmpty()){
                title = file.getFileNameWithoutExtension();
            }

            if (author.isEmpty()){
                author = "Unknown Author";
            }
            delete reader;
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

bool PlayerAudio::isPlaying(){
    return transportSource.isPlaying();
}

void PlayerAudio::setGain(float gain){
    //save current audio
    currentGain = gain;
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

juce::String PlayerAudio::getTitle() const{
    return title;
}

juce::String PlayerAudio::getAuthor() const{
    return author;
}

juce::String PlayerAudio::getDurationText() const{
    return durationText;
}

void PlayerAudio::setLooping(bool shouldLoop){
    isLooping = shouldLoop;
}

//Handle when the file finish
void PlayerAudio::handleEnd(){
    if (transportSource.getCurrentPosition() >= transportSource.getLengthInSeconds()){
        if (isLooping){
            transportSource.setPosition(0.0);
            transportSource.start();
        }else if(!isLooping && currFileIndex >= 0){
            playNext();
        }
    }
}

//Mute effect
void PlayerAudio::setMute(bool shouldMute){
    if (shouldMute){
        lastGainBeforeMute = currentGain;
        transportSource.setGain(0.0);
    }else{
        transportSource.setGain(lastGainBeforeMute);
    }
}

//Loading Playlist
void PlayerAudio::loadPlaylist(const juce::Array<juce::File> &files){
    playlistFiles.clear();
    playlistFiles.addArray(files);

    if (playlistFiles.isEmpty()){
        return;
    }

    currFileIndex = 0;
    loadFile(playlistFiles[currFileIndex]);
    readMeta(playlistFiles[currFileIndex]);
}

//Playing next file in the playlist
void PlayerAudio::playNext(){
    if (playlistFiles.isEmpty()){
        return;
    }
    currFileIndex++;
    if (currFileIndex >= playlistFiles.size()){
        currFileIndex = 0;
    }

    loadFile(playlistFiles[currFileIndex]);
    readMeta(playlistFiles[currFileIndex]);
    trackChanged = true;
}

//playing previous file in the playlist
void PlayerAudio::playPrevious(){
    if (playlistFiles.isEmpty()){
        return;
    }
    currFileIndex--;
    if (currFileIndex < 0){
        currFileIndex = playlistFiles.size() - 1;
    }

    loadFile(playlistFiles[currFileIndex]);
    readMeta(playlistFiles[currFileIndex]);
    trackChanged = true;
}

//reset the playlist (used to cancel current playlist when playing a signle file)
void PlayerAudio::delPlaylist(){
    currFileIndex = -1;
    playlistFiles.clear();
}

//Postion Looping
void PlayerAudio::setLoopStartPoint(){
    loopStartSeconds = getPosition();

    if (loopStartSeconds >= loopEndSeconds){
        loopEndSeconds = getLength();
    }
}

void PlayerAudio::setLoopEndPoint(){
    loopEndSeconds = getPosition();

    if (loopEndSeconds <= loopStartSeconds){
        loopStartSeconds = 0.0;
    }
}

void PlayerAudio::setABLooping(bool shouldABLoop){
    isABLoopingActive = shouldABLoop;
}

//Move the audio 10sec
void PlayerAudio::jumpForward(double seconds){
    if (transportSource.isPlaying() || transportSource.getCurrentPosition() > 0){
        double newPos = transportSource.getCurrentPosition() + seconds;
        double trackLength = transportSource.getLengthInSeconds();

        if (newPos > trackLength)
            newPos = trackLength;

        transportSource.setPosition(newPos);
    }
}

void PlayerAudio::jumpBackward(double seconds){
    if (transportSource.isPlaying() || transportSource.getCurrentPosition() > 0)
    {
        double newPos = transportSource.getCurrentPosition() - seconds;
        if (newPos < 0)
            newPos = 0;

        transportSource.setPosition(newPos);
    }
}

void PlayerAudio::setPlaybackSpeed(double newSpeed){
    // Save old speed and playback state
    double oldSpeed = playbackSpeed;
    double currPosition = getPosition();
    double fileLength = getLength();
    bool wasPlaying = transportSource.isPlaying();

    playbackSpeed = juce::jlimit(0.5, 2.0, newSpeed);

    // Adjust transport sample rate
    if (readerSource != nullptr && readerSource->getAudioFormatReader() != nullptr){
        double originalSampleRate = readerSource->getAudioFormatReader()->sampleRate;
        transportSource.setSource(readerSource.get(), 0, nullptr, originalSampleRate * playbackSpeed);
    }

    // Correct position based on change in speed
    double speedRatio = oldSpeed / playbackSpeed;
    double scaledPosition = juce::jlimit(0.0, fileLength, currPosition * speedRatio);
    setPosition(scaledPosition);

    if (wasPlaying)
        transportSource.start();
}





double PlayerAudio::getPlaybackSpeed(){
    return playbackSpeed;
}

juce::File PlayerAudio::getCurrentFile(){
    return playlistFiles[currFileIndex];
}
bool PlayerAudio::getTrackChanged(){
    return trackChanged;
}

void PlayerAudio::setTrackChanged(bool changed){
    trackChanged = changed;
}