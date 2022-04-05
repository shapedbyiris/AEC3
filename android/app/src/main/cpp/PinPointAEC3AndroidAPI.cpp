//
//  PinPointAECAndroidAPI.cpp
//  PinPointAECAndroid
//
//

#include "PinPointAEC3AndroidAPI.h"
#include "PinPointAEC3MVM.h"

#define WEBRTC_POSIX 1

#include "api/echo_canceller3_factory.h"
#include "api/echo_canceller3_config.h"
#include "audio_processing/audio_buffer.h"
#include "audio_processing/aec3/echo_canceller3.h"
#include "audio_processing/high_pass_filter.h"

class PinPointAEC3 {
public:
    PinPointAEC3(int sampleRate, bool linearOutput) {
        if (sampleRate != 16000 && sampleRate != 32000 && sampleRate != 48000) {
            throw std::invalid_argument("Sample rate not supported. Valid sample rates are 16000, 32000 and 48000.");
        }
        this->sampleRate = sampleRate;
        this->numberOfChannels = 1;
        this->streamConfig = webrtc::StreamConfig(sampleRate, numberOfChannels, false);
        this->linearStreamConfig = webrtc::StreamConfig(kLinearOutputRateHz, 1, false);

        webrtc::EchoCanceller3Config config = webrtc::EchoCanceller3Config();
        if (linearOutput)
            config.filter.export_linear_aec_output = true;

        const size_t numRenderChannels = numberOfChannels;
        const size_t numCaptureChannels = numberOfChannels;

        aecBuffer = std::make_unique<webrtc::AudioBuffer>(sampleRate, numberOfChannels, sampleRate, numberOfChannels, sampleRate, numberOfChannels);
        refBuffer = std::make_unique<webrtc::AudioBuffer>(sampleRate, numberOfChannels, sampleRate, numberOfChannels, sampleRate, numberOfChannels);
        linearBuffer = std::make_unique<webrtc::AudioBuffer>(kLinearOutputRateHz, numberOfChannels, kLinearOutputRateHz, numberOfChannels, kLinearOutputRateHz, numberOfChannels);
        aec = std::make_unique<webrtc::EchoCanceller3>(config, sampleRate, numRenderChannels, numCaptureChannels);
        highPassFilter = std::make_unique<webrtc::HighPassFilter>(sampleRate, numberOfChannels);
    }

    void processCapture(float* audioBuffer, int numberOfFrames) {
        if (numberOfFrames != sampleRate / 100) {
            throw std::invalid_argument("numberOfFrames should be sampleRate / 100");
        }
        float* stackedBuffer[1] = {audioBuffer};
        aecBuffer->CopyFrom(stackedBuffer, streamConfig);
        aec->AnalyzeCapture(aecBuffer.get());
        if (sampleRateSupportsMultiBand()) {
            aecBuffer->SplitIntoFrequencyBands();
        }
        highPassFilter->Process(aecBuffer.get(), sampleRateSupportsMultiBand());
        aec->SetAudioBufferDelay(0);
        aec->ProcessCapture(aecBuffer.get(), false);
        if (sampleRateSupportsMultiBand()) {
            aecBuffer->MergeFrequencyBands();
        }
        aecBuffer->CopyTo(streamConfig, stackedBuffer);
    }

    void processCapture(float* audioBuffer, float* linearAudioBuffer, int numberOfFrames) {
        if (numberOfFrames != sampleRate / 100) {
            throw std::invalid_argument("numberOfFrames should be sampleRate / 100");
        }
        float* stackedBuffer[1] = {audioBuffer};
        float* stackedLinearBuffer[1] = {linearAudioBuffer};
        aecBuffer->CopyFrom(stackedBuffer, streamConfig);
        aec->AnalyzeCapture(aecBuffer.get());
        if (sampleRateSupportsMultiBand()) {
            aecBuffer->SplitIntoFrequencyBands();
        }
        highPassFilter->Process(aecBuffer.get(), sampleRateSupportsMultiBand());
        aec->SetAudioBufferDelay(0);
        aec->ProcessCapture(aecBuffer.get(), linearBuffer.get(), false);
        if (sampleRateSupportsMultiBand()) {
            aecBuffer->MergeFrequencyBands();
        }
        aecBuffer->CopyTo(streamConfig, stackedBuffer);
        linearBuffer->CopyTo(linearStreamConfig, stackedLinearBuffer);
    }

    void processCapture(float* audioBuffer, float* linearAudioBuffer, int numberOfFrames, bool enableMVM) {
        if (numberOfFrames != sampleRate / 100) {
            throw std::invalid_argument("numberOfFrames should be sampleRate / 100");
        }
        float* stackedBuffer[1] = {audioBuffer};
        float* stackedLinearBuffer[1] = {linearAudioBuffer};
        aecBuffer->CopyFrom(stackedBuffer, streamConfig);
        aec->AnalyzeCapture(aecBuffer.get());
        if (sampleRateSupportsMultiBand()) {
            aecBuffer->SplitIntoFrequencyBands();
        }
        highPassFilter->Process(aecBuffer.get(), sampleRateSupportsMultiBand());
        aec->SetAudioBufferDelay(0);
        aec->ProcessCapture(aecBuffer.get(), linearBuffer.get(), false);
        if (sampleRateSupportsMultiBand()) {
            aecBuffer->MergeFrequencyBands();
        }
        aecBuffer->CopyTo(streamConfig, stackedBuffer);
        linearBuffer->CopyTo(linearStreamConfig, stackedLinearBuffer);
        if (enableMVM)
        {
            mvm->combineAecLinearNonlinear (stackedBuffer[0], stackedLinearBuffer[0], numberOfFrames);
            aecBuffer->CopyFrom(stackedBuffer, streamConfig);
        }
    }

    void analyzeRender(float* audioBuffer, int numberOfFrames) {
        if (numberOfFrames != sampleRate / 100) {
            throw std::invalid_argument("numberOfFrames should be sampleRate / 100");
        }
        refBuffer->CopyFrom(&audioBuffer, streamConfig);
        if (sampleRateSupportsMultiBand()) {
            refBuffer->SplitIntoFrequencyBands();
        }
        aec->AnalyzeRender(refBuffer.get());
        if (sampleRateSupportsMultiBand()) {
            refBuffer->MergeFrequencyBands();
        }
    }

private:
    int sampleRate;
    int numberOfChannels;
    webrtc::StreamConfig streamConfig;
    webrtc::StreamConfig linearStreamConfig;
    std::unique_ptr<webrtc::EchoCanceller3> aec;
    std::unique_ptr<webrtc::AudioBuffer> aecBuffer;
    std::unique_ptr<webrtc::AudioBuffer> refBuffer;
    const int kLinearOutputRateHz = 16000;
    std::unique_ptr<webrtc::AudioBuffer> linearBuffer;
    std::unique_ptr<webrtc::HighPassFilter> highPassFilter;
    std::unique_ptr<PinPointAEC3MVM> mvm;

    bool sampleRateSupportsMultiBand() {
        return sampleRate == 32000 || sampleRate == 48000;
    }
};

void* PinPointAEC3Create(int sampleRate, bool linearOutput) {
    PinPointAEC3* aec = new PinPointAEC3(sampleRate, linearOutput);
    return aec;
}

void PinPointAEC3Destroy(void *object) {
    PinPointAEC3 *aec = (PinPointAEC3*)object;
    delete aec;
}

void PinPointAEC3ProcessCapture(void* object, float* audioBuffer, int numberOfFrames) {
    PinPointAEC3 *aec = (PinPointAEC3*)object;
    aec->processCapture(audioBuffer, numberOfFrames);
}

void PinPointAEC3ProcessCaptureLinear(void* object, float* audioBuffer, float* linearBuffer, int numberOfFrames) {
    PinPointAEC3 *aec = (PinPointAEC3*)object;
    aec->processCapture(audioBuffer, linearBuffer, numberOfFrames);
}

void PinPointAEC3ProcessCaptureMVM(void* object, float* audioBuffer, float* linearBuffer, int numberOfFrames) {
    PinPointAEC3 *aec = (PinPointAEC3*)object;
    aec->processCapture(audioBuffer, linearBuffer, numberOfFrames, true);
}

void PinPointAEC3AnalyzeRender(void* object, float* audioBuffer, int numberOfFrames) {
    PinPointAEC3 *aec = (PinPointAEC3*)object;
    aec->analyzeRender(audioBuffer, numberOfFrames);
}
