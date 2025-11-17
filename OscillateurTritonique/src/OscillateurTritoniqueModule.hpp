#pragma once
#include <rack.hpp>

using namespace rack;

struct OscillateurTritoniqueModule : Module {
    enum ParamIds {
        TOPOLOGY_PARAM,
        TOPOLOGY_ATTEN_PARAM,
        SKEW_PARAM,
        SKEW_ATTEN_PARAM,
        BLOOM_PARAM,
        BLOOM_ATTEN_PARAM,
        GLIDE_PARAM,
        GLIDE_ATTEN_PARAM,
        NUM_PARAMS
    };
    enum InputIds {
        VOCT_INPUT,
        TOPOLOGY_CV_INPUT,
        SKEW_CV_INPUT,
        BLOOM_CV_INPUT,
        GLIDE_CV_INPUT,
        NUM_INPUTS
    };
    enum OutputIds {
        AUDIO_OUTPUT,
        NUM_OUTPUTS
    };
    enum LightIds {
        NUM_LIGHTS
    };

    // Wavetable morphing
    static constexpr int WAVETABLE_SIZE = 2048;
    float wavetable[WAVETABLE_SIZE];
    float wavetablePhase = 0.f;
    
    // Portamento
    float targetFreq = 261.626f; // C4
    float currentFreq = 261.626f;
    
    // Micro delay for temporal skew
    static constexpr int SKEW_BUFFER_SIZE = 4096;
    float skewBuffer[SKEW_BUFFER_SIZE] = {};
    int skewWriteIdx = 0;
    
    // Harmonic expansion for spectral bloom
    static constexpr int NUM_HARMONICS = 12;
    float harmonicPhases[NUM_HARMONICS] = {};
    float harmonicAmps[NUM_HARMONICS] = {};
    
    // Tritone interval (sqrt(2), 600 cents)
    static constexpr float TRITONE_RATIO = 1.41421356237f;

    OscillateurTritoniqueModule();
    void process(const ProcessArgs& args) override;
    void onReset() override;
    
    // DSP processors
    void generateWavetable(float topology);
    float processTopologyWarp(float topology, float phase);
    float processTemporalSkew(float input, float skew, float sampleTime);
    float processSpectralBloom(float bloom, float baseFreq, float sampleTime);
    float processTritoneGlide(float voct, float glide, float sampleTime);
};
