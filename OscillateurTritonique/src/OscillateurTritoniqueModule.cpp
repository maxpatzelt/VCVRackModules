#include "OscillateurTritoniqueModule.hpp"
#include <cmath>

OscillateurTritoniqueModule::OscillateurTritoniqueModule() {
    config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);
    
    configParam(TOPOLOGY_PARAM, 0.f, 1.f, 0.33f, "Topology Warp");
    configParam(TOPOLOGY_ATTEN_PARAM, -1.f, 1.f, 0.f, "Topology CV Attenuator");
    
    configParam(SKEW_PARAM, 0.f, 1.f, 0.f, "Temporal Skew");
    configParam(SKEW_ATTEN_PARAM, -1.f, 1.f, 0.f, "Skew CV Attenuator");
    
    configParam(BLOOM_PARAM, 0.f, 1.f, 0.2f, "Spectral Bloom");
    configParam(BLOOM_ATTEN_PARAM, -1.f, 1.f, 0.f, "Bloom CV Attenuator");
    
    configParam(GLIDE_PARAM, 0.f, 1.f, 0.1f, "Tritone Glide");
    configParam(GLIDE_ATTEN_PARAM, -1.f, 1.f, 0.f, "Glide CV Attenuator");
    
    configInput(VOCT_INPUT, "V/Oct");
    configInput(TOPOLOGY_CV_INPUT, "Topology CV");
    configInput(SKEW_CV_INPUT, "Temporal Skew CV");
    configInput(BLOOM_CV_INPUT, "Spectral Bloom CV");
    configInput(GLIDE_CV_INPUT, "Glide CV");
    
    configOutput(AUDIO_OUTPUT, "Audio");
    
    onReset();
}

void OscillateurTritoniqueModule::onReset() {
    wavetablePhase = 0.f;
    currentFreq = 261.626f;
    targetFreq = 261.626f;
    skewWriteIdx = 0;
    
    for (int i = 0; i < SKEW_BUFFER_SIZE; i++) {
        skewBuffer[i] = 0.f;
    }
    
    for (int i = 0; i < NUM_HARMONICS; i++) {
        harmonicPhases[i] = 0.f;
        harmonicAmps[i] = 0.f;
    }
}

void OscillateurTritoniqueModule::generateWavetable(float topology) {
    // Morph between sine → saw → folded sine
    // topology: 0.0 = pure sine, 0.5 = sawtooth, 1.0 = folded/complex
    
    for (int i = 0; i < WAVETABLE_SIZE; i++) {
        float phase = (float)i / WAVETABLE_SIZE;
        float sine = std::sin(2.f * M_PI * phase);
        float saw = 2.f * phase - 1.f;
        
        // Wavefold for complex topology
        float folded = std::sin(3.f * M_PI * phase) * 0.5f + sine * 0.5f;
        
        if (topology < 0.5f) {
            // Morph sine → saw
            float blend = topology * 2.f;
            wavetable[i] = sine * (1.f - blend) + saw * blend;
        } else {
            // Morph saw → folded
            float blend = (topology - 0.5f) * 2.f;
            wavetable[i] = saw * (1.f - blend) + folded * blend;
        }
    }
}

float OscillateurTritoniqueModule::processTopologyWarp(float topology, float phase) {
    // Sample from wavetable
    float readPos = phase * WAVETABLE_SIZE;
    int idx0 = (int)readPos % WAVETABLE_SIZE;
    int idx1 = (idx0 + 1) % WAVETABLE_SIZE;
    float frac = readPos - (int)readPos;
    
    // Linear interpolation
    return wavetable[idx0] * (1.f - frac) + wavetable[idx1] * frac;
}

float OscillateurTritoniqueModule::processTemporalSkew(float input, float skew, float sampleTime) {
    // Write to circular buffer
    skewBuffer[skewWriteIdx] = input;
    skewWriteIdx = (skewWriteIdx + 1) % SKEW_BUFFER_SIZE;
    
    // Micro-delay: 0-10ms with modulation
    float delayMs = skew * 10.f;
    int delaySamples = (int)(delayMs * 0.001f / sampleTime);
    delaySamples = clamp(delaySamples, 0, SKEW_BUFFER_SIZE - 1);
    
    int readIdx = (skewWriteIdx - delaySamples + SKEW_BUFFER_SIZE) % SKEW_BUFFER_SIZE;
    
    // Add modulated micro-timing shift
    float modulation = std::sin(2.f * M_PI * 1.3f * APP->engine->getFrame() * sampleTime);
    int modulatedOffset = (int)(modulation * skew * 100.f);
    readIdx = (readIdx + modulatedOffset + SKEW_BUFFER_SIZE) % SKEW_BUFFER_SIZE;
    
    return skewBuffer[readIdx];
}

float OscillateurTritoniqueModule::processSpectralBloom(float bloom, float baseFreq, float sampleTime) {
    // Generate harmonics with bloom-controlled expansion
    float output = 0.f;
    
    for (int i = 0; i < NUM_HARMONICS; i++) {
        int harmonic = i + 1;
        
        // Amplitude decay with bloom extension
        float naturalDecay = 1.f / (float)harmonic;
        float bloomBoost = std::pow(1.f - bloom, (float)harmonic * 0.5f);
        harmonicAmps[i] = naturalDecay * (0.3f + bloom * 0.7f) * bloomBoost;
        
        // Frequency: base * harmonic with tritone inflection
        float freq = baseFreq * harmonic;
        if (harmonic % 2 == 0 && bloom > 0.5f) {
            // Apply tritone shift to even harmonics at high bloom
            freq *= 1.f + (bloom - 0.5f) * (TRITONE_RATIO - 1.f);
        }
        
        // Advance phase
        harmonicPhases[i] += freq * sampleTime;
        if (harmonicPhases[i] > 1.f) harmonicPhases[i] -= 1.f;
        
        // Generate sine
        output += std::sin(2.f * M_PI * harmonicPhases[i]) * harmonicAmps[i];
    }
    
    return output;
}

float OscillateurTritoniqueModule::processTritoneGlide(float voct, float glide, float sampleTime) {
    // Convert V/Oct to frequency
    float inputFreq = 261.626f * std::pow(2.f, voct);
    
    // Apply tritone modulation with glide amount
    float tritoneModulation = std::sin(2.f * M_PI * 0.7f * APP->engine->getFrame() * sampleTime);
    float tritoneFreq = inputFreq * std::pow(TRITONE_RATIO, glide * tritoneModulation);
    
    targetFreq = tritoneFreq;
    
    // Portamento with glide-controlled time constant
    float portamentoTime = 0.001f + glide * 0.5f; // 1ms to 500ms
    float lambda = sampleTime / portamentoTime;
    lambda = clamp(lambda, 0.f, 1.f);
    
    currentFreq += (targetFreq - currentFreq) * lambda;
    
    return currentFreq;
}

void OscillateurTritoniqueModule::process(const ProcessArgs& args) {
    // Get parameters with CV
    float topology = params[TOPOLOGY_PARAM].getValue();
    if (inputs[TOPOLOGY_CV_INPUT].isConnected()) {
        topology += inputs[TOPOLOGY_CV_INPUT].getVoltage() * params[TOPOLOGY_ATTEN_PARAM].getValue() * 0.1f;
    }
    topology = clamp(topology, 0.f, 1.f);
    
    float skew = params[SKEW_PARAM].getValue();
    if (inputs[SKEW_CV_INPUT].isConnected()) {
        skew += inputs[SKEW_CV_INPUT].getVoltage() * params[SKEW_ATTEN_PARAM].getValue() * 0.1f;
    }
    skew = clamp(skew, 0.f, 1.f);
    
    float bloom = params[BLOOM_PARAM].getValue();
    if (inputs[BLOOM_CV_INPUT].isConnected()) {
        bloom += inputs[BLOOM_CV_INPUT].getVoltage() * params[BLOOM_ATTEN_PARAM].getValue() * 0.1f;
    }
    bloom = clamp(bloom, 0.f, 1.f);
    
    float glide = params[GLIDE_PARAM].getValue();
    if (inputs[GLIDE_CV_INPUT].isConnected()) {
        glide += inputs[GLIDE_CV_INPUT].getVoltage() * params[GLIDE_ATTEN_PARAM].getValue() * 0.1f;
    }
    glide = clamp(glide, 0.f, 1.f);
    
    // Get V/Oct input
    float voct = 0.f;
    if (inputs[VOCT_INPUT].isConnected()) {
        voct = inputs[VOCT_INPUT].getVoltage();
    }
    
    // Process tritone glide to get current frequency
    float freq = processTritoneGlide(voct, glide, args.sampleTime);
    
    // Generate wavetable based on topology
    generateWavetable(topology);
    
    // Advance wavetable phase
    wavetablePhase += freq * args.sampleTime;
    if (wavetablePhase >= 1.f) wavetablePhase -= 1.f;
    
    // Get base oscillator output
    float baseOsc = processTopologyWarp(topology, wavetablePhase);
    
    // Add spectral bloom harmonics
    float bloomOutput = processSpectralBloom(bloom, freq, args.sampleTime);
    
    // Combine base + bloom
    float mixed = baseOsc * 0.6f + bloomOutput * 0.4f;
    
    // Apply temporal skew
    float output = processTemporalSkew(mixed, skew, args.sampleTime);
    
    // Output
    outputs[AUDIO_OUTPUT].setVoltage(output * 5.f);
}
