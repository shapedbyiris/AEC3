//
//  PinPointAEC3MVM.h
//  PinPointAEC
//  Additional processing after AEC3 for "Muffled Voice Mitigation" (MVM)
//  Created by Cesar Alonso on 25/3/22.
//

#ifndef PinPointAEC3MVM_h
#define PinPointAEC3MVM_h

class PinPointAEC3MVM
{
private:
    // Linear + non-linear combination parameters
    const float frameEnergyThreshold = 0.0001;
    const float maxGain = 0.4;
    const float alpha = 0.95;
    float gain;
    float lastSample;
    
public:
    PinPointAEC3MVM(void)
    {
        gain = maxGain;
        lastSample = 0;
    }
    
    float calculateEnergy(float *buffer, int numSamples)
    {
        float frameEnergy = 0;
        for (int i = 0; i < numSamples; i++)
        {
            frameEnergy += buffer[i] * buffer[i];
        }
        return frameEnergy;
    }
    
    void combineAecLinearNonlinear (float* captureBufferPtr, float* captureBufferLinearPtr, int samplesPerFrame)
    {
        captureBufferLinearPtr[0] = captureBufferLinearPtr[0] * 0.5 + lastSample * 0.5; // LPF
        for (int i = 1; i < samplesPerFrame; i++)
        {
            captureBufferLinearPtr[i] = captureBufferLinearPtr[i] * 0.5 + captureBufferLinearPtr[i-1] * 0.5; // LPF
        }
        lastSample = captureBufferLinearPtr[samplesPerFrame-1];
        
        float frameEnergy = calculateEnergy(captureBufferPtr, samplesPerFrame);
        
        if (frameEnergy > frameEnergyThreshold)
        {
            gain = maxGain;
        }
        else
        {
            gain = gain * alpha;
        }
        
        for (int i = 0; i < samplesPerFrame; i++)
        {
            captureBufferPtr[i]  = captureBufferPtr[i] + gain * captureBufferLinearPtr[i];
        }
    }
};

#endif /* PinPointAEC3MVM_h */
