#include "LfoModule.hpp"
#include <cmath>
#include <random>

using namespace rack;

LfoModule::LfoModule() {
    config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS);
    configParam(FREQUENCY_PARAM, 0.1f, 10.0f, 1.0f, "Frequency", " Hz");
    configParam(CHAOS_PARAM, 0.0f, 1.0f, 0.0f, "Chaos", "%", 0.f, 100.f);
    configOutput(SINE_OUTPUT, "Sine LFO");
}

void LfoModule::process(const ProcessArgs &args) {
    float baseFreq = params[FREQUENCY_PARAM].getValue();
    float chaosAmount = params[CHAOS_PARAM].getValue();

    // Generate smooth random modulation (update chaos phase slowly)
    float chaosFreq = 0.5f; // Chaos LFO runs at 0.5 Hz
    sampleTimer += chaosFreq * args.sampleTime;
    while (sampleTimer >= 1.0f) {
        sampleTimer -= 1.0f;
    }
    
    // Use sine wave of random phase for smooth random-like modulation
    float chaosPhase = sampleTimer + ((float)(rng & 0xFFFF) / 65536.0f);
    chaosValue = sinf(2.0f * M_PI * chaosPhase);
    
    // Apply chaos: smoothly modulate frequency
    // At chaos=0: no modulation (multiplier = 1)
    // At chaos=1: ±300% modulation (multiplier = -2 to 4)
    float freqMultiplier = 1.0f + (chaosValue * chaosAmount * 3.0f);
    float modulatedFreq = baseFreq * freqMultiplier;
    
    // Clamp to positive values
    if (modulatedFreq < 0.01f) modulatedFreq = 0.01f;
    
    // Update phase with modulated frequency
    phase += modulatedFreq * args.sampleTime;
    while (phase >= 1.0f) {
        phase -= 1.0f;
    }
    
    // Output sine wave
    float sine = sinf(2.0f * M_PI * phase);
    outputs[SINE_OUTPUT].setVoltage(5.0f * sine);
}