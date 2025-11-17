#pragma once
#include <rack.hpp>

using namespace rack;

struct TemporalisteModule : Module {
    enum ParamIds {
        DENSITY_PARAM,
        DENSITY_ATTEN_PARAM,
        DRIFT_PARAM,
        DRIFT_ATTEN_PARAM,
        ACCENT_PARAM,
        ACCENT_ATTEN_PARAM,
        TIMESHIFT_PARAM,
        TIMESHIFT_ATTEN_PARAM,
        NUM_PARAMS
    };
    enum InputIds {
        CLOCK_INPUT,
        DENSITY_CV_INPUT,
        DRIFT_CV_INPUT,
        ACCENT_CV_INPUT,
        TIMESHIFT_CV_INPUT,
        NUM_INPUTS
    };
    enum OutputIds {
        GATE1_OUTPUT,
        GATE2_OUTPUT,
        GATE3_OUTPUT,
        GATE4_OUTPUT,
        NUM_OUTPUTS
    };
    enum LightIds {
        NUM_LIGHTS
    };

    // Clock and rhythm state
    dsp::SchmittTrigger clockTrigger;
    dsp::PulseGenerator gateGenerators[4];
    
    // Polyrhythm layers (1-4 active)
    struct RhythmLayer {
        int division = 1;      // Clock division ratio
        int stepCount = 0;     // Current step in pattern
        int patternLength = 4; // Steps per pattern
        float probability = 1.f; // Gate probability
        float phase = 0.f;     // Timing phase offset
        bool active = true;
    };
    
    RhythmLayer layers[4];
    
    // Stochastic drift
    float driftPhases[4] = {};
    
    // Spectral accent shaping
    static constexpr int ACCENT_BUFFER_SIZE = 4096;
    float accentBuffer[ACCENT_BUFFER_SIZE] = {};
    int accentWriteIdx = 0;
    
    // Time-space shift buffers for micro-timing
    static constexpr int TIMESHIFT_BUFFER_SIZE = 8192;
    float timeshiftBuffers[4][TIMESHIFT_BUFFER_SIZE] = {};
    int timeshiftWriteIdx = 0;
    
    // Random state
    dsp::SampleRateConverter<1> driftRandom;

    TemporalisteModule();
    void process(const ProcessArgs& args) override;
    void onReset() override;
    
    // DSP processors
    void processPolyrhythmicDensity(float density);
    bool processStochasticDrift(int layer, float drift);
    float processSpectralAccent(int layer, float accent, bool gateActive);
    void processTimeSpaceShift(int layer, float shift, float sampleTime);
};
