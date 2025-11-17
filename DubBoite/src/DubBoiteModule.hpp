#pragma once
#include <rack.hpp>

using namespace rack;

struct DubBoiteModule : Module {
    enum ParamIds {
        // Channel faders
        CH1_FADER,
        CH2_FADER,
        CH3_FADER,
        CH4_FADER,
        // Effect knobs
        DIFFUSION_KNOB,
        SCRUB_KNOB,
        LOWDRIFT_KNOB,
        SATURATION_KNOB,
        // Master
        MASTER_FADER,
        NUM_PARAMS
    };
    enum InputIds {
        CH1_INPUT,
        CH2_INPUT,
        CH3_INPUT,
        CH4_INPUT,
        NUM_INPUTS
    };
    enum OutputIds {
        MIX_OUTPUT,
        SEND_OUTPUT,
        NUM_OUTPUTS
    };
    enum LightIds {
        CH1_LIGHT,
        CH2_LIGHT,
        CH3_LIGHT,
        CH4_LIGHT,
        ENUMS(MASTER_LIGHTS, 3), // RGB VU meter
        NUM_LIGHTS
    };

    static constexpr int DELAY_SIZE = 16384;
    static constexpr int NUM_PATHS = 8;
    
    float delayBuffers[4][DELAY_SIZE] = {};
    int delayWritePos[4] = {};
    float diffusionPaths[NUM_PATHS][DELAY_SIZE] = {};
    int diffusionWritePos[NUM_PATHS] = {};
    float lowpassState[4] = {};
    float saturationMemory[4] = {};
    float scrubPhase = 0.f;
    float lowDriftPhase = 0.f;
    
    // VU meter
    dsp::VuMeter2 masterVu;

    DubBoiteModule();
    void process(const ProcessArgs& args) override;
    
    float processSendDiffusion(float input, float diffusion);
    float processTapeScrub(float input, int channel, float scrub, float sampleTime);
    float processLowDrift(float input, int channel, float drift, float sampleTime);
    float processSaturationBloom(float input, int channel, float bloom);
};
