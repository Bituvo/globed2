#pragma once
#include <defs.hpp>

#include <data/types/gd.hpp>
#include <ui/general/audio_visualizer.hpp>

class VoiceOverlayCell : public cocos2d::CCNode {
public:
    static VoiceOverlayCell* create(const PlayerAccountData& data);
    void updateVolume(float vol);

    int accountId;

private:
    GlobedAudioVisualizer* visualizer;

    bool init(const PlayerAccountData& data);
};