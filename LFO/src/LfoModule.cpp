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

    // Sample and hold random values at a certain rate
    sampleTimer += args.sampleTime;
    if (sampleTimer >= 0.1f) { // Update random value every 100ms
        sampleTimer = 0.0f;
        // Generate new random value
        rng = rng * 1103515245u + 12345u;
        float randomValue = (float)(rng & 0x7FFFFFFF) / (float)0x7FFFFFFF;
        chaosValue = randomValue * 2.0f - 1.0f; // -1 to +1
    }
    
    // Apply chaos: modulate frequency based on held random value
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