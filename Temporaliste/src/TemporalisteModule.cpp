#include "TemporalisteModule.hpp"
#include <cmath>

TemporalisteModule::TemporalisteModule() {
    config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);
    
    configParam(DENSITY_PARAM, 0.f, 1.f, 0.75f, "Polyrhythmic Density");
    configParam(DENSITY_ATTEN_PARAM, -1.f, 1.f, 0.f, "Density CV Attenuator");
    
    configParam(DRIFT_PARAM, 0.f, 1.f, 0.3f, "Stochastic Drift");
    configParam(DRIFT_ATTEN_PARAM, -1.f, 1.f, 0.f, "Drift CV Attenuator");
    
    configParam(ACCENT_PARAM, 0.f, 1.f, 0.5f, "Spectral Accent");
    configParam(ACCENT_ATTEN_PARAM, -1.f, 1.f, 0.f, "Accent CV Attenuator");
    
    configParam(TIMESHIFT_PARAM, 0.f, 1.f, 0.2f, "Time-Space Shift");
    configParam(TIMESHIFT_ATTEN_PARAM, -1.f, 1.f, 0.f, "Shift CV Attenuator");
    
    configInput(CLOCK_INPUT, "Clock");
    configInput(DENSITY_CV_INPUT, "Density CV");
    configInput(DRIFT_CV_INPUT, "Drift CV");
    configInput(ACCENT_CV_INPUT, "Accent CV");
    configInput(TIMESHIFT_CV_INPUT, "Time-Space Shift CV");
    
    configOutput(GATE1_OUTPUT, "Gate 1");
    configOutput(GATE2_OUTPUT, "Gate 2");
    configOutput(GATE3_OUTPUT, "Gate 3");
    configOutput(GATE4_OUTPUT, "Gate 4");
    
    onReset();
}

void TemporalisteModule::onReset() {
    for (int i = 0; i < 4; i++) {
        layers[i].stepCount = 0;
        layers[i].phase = 0.f;
        driftPhases[i] = 0.f;
        timeshiftWriteIdx = 0;
        
        for (int j = 0; j < TIMESHIFT_BUFFER_SIZE; j++) {
            timeshiftBuffers[i][j] = 0.f;
        }
    }
    
    for (int i = 0; i < ACCENT_BUFFER_SIZE; i++) {
        accentBuffer[i] = 0.f;
    }
    accentWriteIdx = 0;
    
    // Initialize rhythm layers with different divisions
    layers[0].division = 1;  // Quarter notes
    layers[0].patternLength = 4;
    
    layers[1].division = 2;  // Eighth notes
    layers[1].patternLength = 8;
    
    layers[2].division = 3;  // Triplets
    layers[2].patternLength = 6;
    
    layers[3].division = 4;  // Sixteenth notes
    layers[3].patternLength = 16;
}

void TemporalisteModule::processPolyrhythmicDensity(float density) {
    // Control how many layers are active (1-4)
    // density 0.0 = 1 layer, 1.0 = 4 layers
    int activeLayers = 1 + (int)(density * 3.f);
    activeLayers = clamp(activeLayers, 1, 4);
    
    for (int i = 0; i < 4; i++) {
        layers[i].active = (i < activeLayers);
        
        // Adjust probabilities based on density
        if (layers[i].active) {
            // Higher layers have lower probabilities
            float layerProb = 1.f - (float)i * 0.15f;
            layers[i].probability = layerProb * (0.5f + density * 0.5f);
        }
    }
}

bool TemporalisteModule::processStochasticDrift(int layer, float drift) {
    // Probabilistic timing variation
    // drift 0.0 = deterministic, 1.0 = highly stochastic
    
    // Generate random value for this layer
    float randomValue = random::uniform();
    
    // Base probability from layer configuration
    float baseProb = layers[layer].probability;
    
    // Apply drift to modulate probability
    float driftedProb = baseProb * (1.f - drift * 0.5f);
    driftedProb += drift * randomValue * 0.5f;
    
    // Temporal drift: phase offset modulation
    driftPhases[layer] += drift * (randomValue - 0.5f) * 0.1f;
    driftPhases[layer] = clamp(driftPhases[layer], -0.5f, 0.5f);
    
    // Gate decision
    return (randomValue < driftedProb);
}

float TemporalisteModule::processSpectralAccent(int layer, float accent, bool gateActive) {
    // Apply spectral shaping to gate events
    // accent 0.0 = clean gates, 1.0 = complex harmonic content
    
    if (!gateActive) return 0.f;
    
    float output = 5.f; // Base gate voltage
    
    // Add harmonic complexity based on accent
    if (accent > 0.1f) {
        // Generate harmonics at different frequencies
        float fundamental = 100.f + (float)layer * 50.f;
        float phase = (float)APP->engine->getFrame() / 48000.f;
        
        float harmonic1 = std::sin(2.f * M_PI * fundamental * phase);
        float harmonic2 = std::sin(2.f * M_PI * fundamental * 2.f * phase) * 0.5f;
        float harmonic3 = std::sin(2.f * M_PI * fundamental * 3.f * phase) * 0.33f;
        
        float spectral = harmonic1 + harmonic2 + harmonic3;
        spectral *= accent * 0.3f; // Mix in based on accent amount
        
        output += spectral;
    }
    
    // Write to accent buffer for feedback/memory
    accentBuffer[accentWriteIdx] = output;
    accentWriteIdx = (accentWriteIdx + 1) % ACCENT_BUFFER_SIZE;
    
    return output;
}

void TemporalisteModule::processTimeSpaceShift(int layer, float shift, float sampleTime) {
    // Micro-timing offsets per layer
    // shift 0.0 = synchronized, 1.0 = maximum offset
    
    // Calculate per-layer delay offset (0-20ms)
    float layerDelay = shift * (float)(layer + 1) * 5.f; // 0-20ms range
    int delaySamples = (int)(layerDelay * 0.001f / sampleTime);
    delaySamples = clamp(delaySamples, 0, TIMESHIFT_BUFFER_SIZE - 1);
    
    // Apply modulated micro-offset
    float modulation = std::sin(2.f * M_PI * 0.37f * (float)layer * APP->engine->getFrame() * sampleTime);
    int modulatedOffset = (int)(modulation * shift * 50.f);
    
    layers[layer].phase = (float)(delaySamples + modulatedOffset) * sampleTime;
}

void TemporalisteModule::process(const ProcessArgs& args) {
    // Get parameters with CV
    float density = params[DENSITY_PARAM].getValue();
    if (inputs[DENSITY_CV_INPUT].isConnected()) {
        density += inputs[DENSITY_CV_INPUT].getVoltage() * params[DENSITY_ATTEN_PARAM].getValue() * 0.1f;
    }
    density = clamp(density, 0.f, 1.f);
    
    float drift = params[DRIFT_PARAM].getValue();
    if (inputs[DRIFT_CV_INPUT].isConnected()) {
        drift += inputs[DRIFT_CV_INPUT].getVoltage() * params[DRIFT_ATTEN_PARAM].getValue() * 0.1f;
    }
    drift = clamp(drift, 0.f, 1.f);
    
    float accent = params[ACCENT_PARAM].getValue();
    if (inputs[ACCENT_CV_INPUT].isConnected()) {
        accent += inputs[ACCENT_CV_INPUT].getVoltage() * params[ACCENT_ATTEN_PARAM].getValue() * 0.1f;
    }
    accent = clamp(accent, 0.f, 1.f);
    
    float timeshift = params[TIMESHIFT_PARAM].getValue();
    if (inputs[TIMESHIFT_CV_INPUT].isConnected()) {
        timeshift += inputs[TIMESHIFT_CV_INPUT].getVoltage() * params[TIMESHIFT_ATTEN_PARAM].getValue() * 0.1f;
    }
    timeshift = clamp(timeshift, 0.f, 1.f);
    
    // Process density to determine active layers
    processPolyrhythmicDensity(density);
    
    // Process time-space shift for all layers
    for (int i = 0; i < 4; i++) {
        processTimeSpaceShift(i, timeshift, args.sampleTime);
    }
    
    // Clock input processing
    if (inputs[CLOCK_INPUT].isConnected()) {
        if (clockTrigger.process(inputs[CLOCK_INPUT].getVoltage())) {
            // Clock pulse received
            
            // Process each active layer
            for (int i = 0; i < 4; i++) {
                if (!layers[i].active) continue;
                
                // Check if this layer triggers on this clock pulse
                bool shouldTrigger = (layers[i].stepCount % layers[i].division == 0);
                
                if (shouldTrigger) {
                    // Apply stochastic drift
                    bool gateFires = processStochasticDrift(i, drift);
                    
                    if (gateFires) {
                        // Trigger gate with 10ms pulse
                        gateGenerators[i].trigger(0.01f);
                    }
                }
                
                // Advance step counter
                layers[i].stepCount = (layers[i].stepCount + 1) % layers[i].patternLength;
            }
        }
    }
    
    // Process gate outputs
    for (int i = 0; i < 4; i++) {
        bool gateActive = gateGenerators[i].process(args.sampleTime);
        
        // Apply spectral accent shaping
        float gateVoltage = processSpectralAccent(i, accent, gateActive);
        
        outputs[GATE1_OUTPUT + i].setVoltage(gateVoltage);
    }
}
