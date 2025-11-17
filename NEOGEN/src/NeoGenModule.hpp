#pragma once
#include <rack.hpp>

using namespace rack;

// Forward declaration
struct NeoGenModule;

struct NeoGenModule : Module {
    enum ParamId {
        SHAPE_PARAM,      // 0.0-1.0: morphs between 4 waveform algorithms
        ARTIFACT_PARAM,   // 0.0-1.0: digital imperfections and chaos
        HARMONICS_PARAM,  // 0.0-1.0: spectral tilt and overtone emphasis
        DRIVE_PARAM,      // 0.0-1.0: waveshaper saturation and DC shift
        NUM_PARAMS
    };
    
    enum InputId {
        VOCT_INPUT,       // V/Oct pitch input
        SHAPE_INPUT,      // Shape CV input
        ARTIFACT_INPUT,   // Artifact CV input
        HARMONICS_INPUT,  // Harmonics CV input
        DRIVE_INPUT,      // Drive CV input
        NUM_INPUTS
    };
    
    enum OutputId {
        AUDIO_OUTPUT,     // Main audio output
        NUM_OUTPUTS
    };
    
    enum LightId {
        NUM_LIGHTS
    };
    
    // DSP state variables
    float phase = 0.0f;           // Main oscillator phase (0.0 to 1.0)
    float frequency = 440.0f;     // Current frequency in Hz
    
    // Algorithm-specific state
    float fractalPhase = 0.0f;    // For fractal saw algorithm
    float polygonAngle = 0.0f;    // For polygon wave algorithm
    float stringBuffer[256] = {}; // For Karplus-Strong string solver
    int stringWritePos = 0;
    float neuronCharge = 0.0f;    // For neuron spike algorithm
    
    NeoGenModule();
    void process(const ProcessArgs& args) override;
    
    // Waveform generation algorithms (0.0-1.0 phase input -> audio sample output)
    float generateFractalSaw(float phase, float harmonics);
    float generatePolygonWave(float phase, float harmonics);
    float generateStringSolver(float phase, float harmonics);
    float generateNeuronSpike(float phase, float harmonics);
    
    // Processing effects
    float applyArtifact(float sample, float amount);
    float applyDrive(float sample, float amount);
};
