#ifndef AUDIOMANAGER_H
#define AUDIOMANAGER_H

#include <SFML/Audio.hpp>

#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

#include "constants/AudioId.h"

class AudioManager
{
private:
    std::unordered_map<AudioId, sf::SoundBuffer> soundBuffers;
    std::unordered_map<AudioId, sf::Sound> sounds;
    std::vector<std::unique_ptr<sf::Sound>> soundPool;

    std::unordered_map<AudioId, float> volumeMap;

    static constexpr size_t minimumPoolSize = 25;

private:
    void initializeVolumeDefaults();
    void initialiseSoundPool();

public:
    AudioManager();
    ~AudioManager();

    void loadSound(AudioId uniqueId, const std::string &filename);

    sf::Sound &getSound(AudioId uniqueId);
    sf::Sound::Status getSoundStatus(AudioId uniqueId) const;

    void playSound(AudioId uniqueId, bool loop = false);
    void playPooledSound(AudioId uniqueId);
    void stopSound(AudioId uniqueId);

    void cleanupSounds();
};

#endif // AUDIOMANAGER_H
