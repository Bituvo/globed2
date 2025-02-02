#pragma once
#include <defs.hpp>

#if GLOBED_VOICE_SUPPORT

#include <opus.h>
#include <util/data.hpp>

#include "encoder.hpp"

struct DecodedOpusData {
    float* ptr;
    size_t length;

    void freeData() {
        GLOBED_REQUIRE(ptr != nullptr, "attempting to double free an instance of DecodedOpusData")

        delete[] ptr;
#ifdef GLOBED_DEBUG
        // to try and prevent misuse
        ptr = nullptr;
        length = -1; // wraps around to int max
#endif // GLOBED_DEBUG
    }
};

class AudioDecoder {
public:
    AudioDecoder(int sampleRate = 0, int frameSize = 0, int channels = 1);
    ~AudioDecoder();

    // disable copying and enable moving
    AudioDecoder(const AudioDecoder&) = delete;
    AudioDecoder& operator=(const AudioDecoder&) = delete;

    AudioDecoder(AudioDecoder&& other) noexcept {
        decoder = other.decoder;
        other.decoder = nullptr;

        channels = other.channels;
        sampleRate = other.sampleRate;
        frameSize = other.frameSize;
    }

    AudioDecoder& operator=(AudioDecoder&& other) noexcept {
        if (this != &other) {
            if (this->decoder) {
                opus_decoder_destroy(this->decoder);
            }

            this->decoder = other.decoder;
            other.decoder = nullptr;

            channels = other.channels;
            sampleRate = other.sampleRate;
            frameSize = other.frameSize;
        }

        return *this;
    }

    // Decodes the given Opus data into PCM float samples. `length` must be the size of the input data in bytes.
    // After you no longer need the decoded data, you must call `data.freeData()`, or (preferrably, for explicitness) `AudioDecoder::freeData(data)`
    [[nodiscard]] DecodedOpusData decode(const util::data::byte* data, size_t length);

    // Decodes the given Opus data into PCM float samples. `length` must be the size of the input data in bytes.
    // After you no longer need the decoded data, you must call `data.freeData()`, or (preferrably, for explicitness) `AudioDecoder::freeData(data)`
    [[nodiscard]] DecodedOpusData decode(const EncodedOpusData& data);

    static void freeData(DecodedOpusData& data) {
        data.freeData();
    }

    // sets the sample rate that will be used and recreates the decoder
    void setSampleRate(int sampleRate);
    // sets the frame size of the data that will be used
    void setFrameSize(int frameSize);
    // sets the amount of channels that will be used and recreates the decoder
    void setChannels(int channels);

private:
    // EXPERIMENTAL ZONE
    // See the note in `encoder.hpp`

    // reset the internal decoder state
    void resetState();

protected:
    OpusDecoder* decoder = nullptr;

    int _res;
    int sampleRate, frameSize, channels;

    void remakeDecoder();
    void errcheck(const char* where);
};

#endif // GLOBED_VOICE_SUPPORT