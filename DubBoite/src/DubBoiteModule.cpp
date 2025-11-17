#include "DubBoiteModule.hpp"
#include <cmath>

DubBoiteModule::DubBoiteModule() {
    config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);
    
    // Channel faders (0-1 range)
    configParam(CH1_FADER, 0.f, 1.f, 0.8f, "Channel 1 Level", "%", 0, 100);
    configParam(CH2_FADER, 0.f, 1.f, 0.8f, "Channel 2 Level", "%", 0, 100);
    configParam(CH3_FADER, 0.f, 1.f, 0.8f, "Channel 3 Level", "%", 0, 100);
    configParam(CH4_FADER, 0.f, 1.f, 0.8f, "Channel 4 Level", "%", 0, 100);
    
    // Effect knobs
    configParam(DIFFUSION_KNOB, 0.f, 1.f, 0.4f, "Send Diffusion");
    configParam(SCRUB_KNOB, 0.f, 1.f, 0.3f, "Tape Scrub");
    configParam(LOWDRIFT_KNOB, 0.f, 1.f, 0.2f, "Low-End Drift");
    configParam(SATURATION_KNOB, 0.f, 1.f, 0.5f, "Saturation Bloom");
    
    // Master fader
    configParam(MASTER_FADER, 0.f, 1.f, 0.9f, "Master Level", "%", 0, 100);
    
    // Inputs/outputs
    configInput(CH1_INPUT, "Channel 1");
    configInput(CH2_INPUT, "Channel 2");
    configInput(CH3_INPUT, "Channel 3");
    configInput(CH4_INPUT, "Channel 4");
    configOutput(MIX_OUTPUT, "Mix");
    configOutput(SEND_OUTPUT, "Effects Send");
    
    masterVu.lambda = 1 / 0.1f; // 100ms integration time
}

float DubBoiteModule::processSendDiffusion(float input, float diffusion) {
    if (diffusion < 0.01f) return 0.f;
    
    float output = 0.f;
    for (int i = 0; i < NUM_PATHS; i++) {
        int delayMs = 10 + i * 10; // 10ms to 80ms
        int delaySamples = (int)(delayMs * 48.f); // Assume 48kHz
        
        // Write input to path
        diffusionPaths[i][diffusionWritePos[i]] = input;
        
        // Read delayed signal
        int readPos = (diffusionWritePos[i] - delaySamples + DELAY_SIZE) % DELAY_SIZE;
        float delayed = diffusionPaths[i][readPos];
        
        // Apply spectral coloration
        float brightness = (float)i / NUM_PATHS;
        output += delayed * (0.7f + brightness * 0.3f) / NUM_PATHS;
        
        diffusionWritePos[i] = (diffusionWritePos[i] + 1) % DELAY_SIZE;
    }
    
    return output * diffusion;
}

float DubBoiteModule::processTapeScrub(float input, int channel, float scrub, float sampleTime) {
    // LFO with harmonics
    float lfoFreq = 0.3f + scrub * 2.f; // 0.3-2.3Hz
    scrubPhase += lfoFreq * sampleTime;
    if (scrubPhase >= 1.f) scrubPhase -= 1.f;
    
    float lfo = std::sin(2.f * M_PI * scrubPhase);
    lfo += std::sin(2.f * M_PI * scrubPhase * 2.7f) * 0.3f;
    lfo += std::sin(2.f * M_PI * scrubPhase * 5.3f) * 0.2f;
    
    // Modulated delay 5-25ms
    int baseDelay = (int)(15.f * 48.f);
    int modDelay = (int)(10.f * 48.f * lfo * scrub);
    int totalDelay = clamp(baseDelay + modDelay, 1, DELAY_SIZE - 1);
    
    delayBuffers[channel][delayWritePos[channel]] = input;
    int readPos = (delayWritePos[channel] - totalDelay + DELAY_SIZE) % DELAY_SIZE;
    float output = delayBuffers[channel][readPos];
    
    delayWritePos[channel] = (delayWritePos[channel] + 1) % DELAY_SIZE;
    
    return output * scrub + input * (1.f - scrub);
}

float DubBoiteModule::processLowDrift(float input, int channel, float drift, float sampleTime) {
    if (drift < 0.01f) return input;
    
    // 200Hz lowpass
    float cutoff = 200.f;
    float rc = 1.f / (2.f * M_PI * cutoff);
    float alpha = sampleTime / (rc + sampleTime);
    lowpassState[channel] += alpha * (input - lowpassState[channel]);
    
    // Drift LFO at 0.2Hz
    lowDriftPhase += 0.2f * sampleTime;
    if (lowDriftPhase >= 1.f) lowDriftPhase -= 1.f;
    float lfo = std::sin(2.f * M_PI * lowDriftPhase);
    
    // Mix drifted lows back
    float driftedLow = lowpassState[channel] * (1.f + lfo * drift * 0.3f);
    return input * (1.f - drift * 0.5f) + driftedLow * drift * 0.5f;
}

float DubBoiteModule::processSaturationBloom(float input, int channel, float bloom) {
    if (bloom < 0.01f) return input;
    
    float drive = 1.f + bloom * 3.f; // 1-4x
    float driven = input * drive;
    
    // Tanh saturation
    float saturated = std::tanh(driven);
    
    // Harmonic waveshaping
    float shaped = saturated + std::sin(saturated * 3.f * M_PI) * bloom * 0.2f;
    
    // Feedback
    saturationMemory[channel] = shaped * 0.1f;
    shaped += saturationMemory[channel] * bloom;
    
    return shaped;
}

void DubBoiteModule::process(const ProcessArgs& args) {
    float diffusion = params[DIFFUSION_KNOB].getValue();
    float scrub = params[SCRUB_KNOB].getValue();
    float lowdrift = params[LOWDRIFT_KNOB].getValue();
    float saturation = params[SATURATION_KNOB].getValue();
    float master = params[MASTER_FADER].getValue();
    
    float mixSum = 0.f;
    float sendSum = 0.f;
    
    for (int i = 0; i < 4; i++) {
        if (!inputs[CH1_INPUT + i].isConnected()) {
            lights[CH1_LIGHT + i].setBrightness(0.f);
            continue;
        }
        
        float input = inputs[CH1_INPUT + i].getVoltage();
        float fader = params[CH1_FADER + i].getValue();
        
        // Process chain
        float sig = input * fader;
        sig = processTapeScrub(sig, i, scrub, args.sampleTime);
        sig = processLowDrift(sig, i, lowdrift, args.sampleTime);
        sig = processSaturationBloom(sig, i, saturation);
        
        // Send to diffusion
        sendSum += sig * diffusion;
        
        // Mix
        mixSum += sig;
        
        // Channel light
        lights[CH1_LIGHT + i].setBrightness(std::abs(sig) * 0.2f);
    }
    
    // Process send diffusion
    float sendOut = processSendDiffusion(sendSum, diffusion);
    
    // Apply master
    float finalMix = mixSum * master;
    
    // VU meter
    masterVu.process(args.sampleTime, finalMix / 5.f);
    lights[MASTER_LIGHTS + 0].setBrightness(masterVu.getBrightness(0.f, 0.f));
    lights[MASTER_LIGHTS + 1].setBrightness(masterVu.getBrightness(0.f, 1.f));
    lights[MASTER_LIGHTS + 2].setBrightness(masterVu.getBrightness(1.f, 0.f));
    
    // Outputs
    outputs[MIX_OUTPUT].setVoltage(finalMix);
    outputs[SEND_OUTPUT].setVoltage(sendOut * master);
}
