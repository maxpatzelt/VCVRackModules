#include "NeoGenModule.hpp"
#include <cmath>

NeoGenModule::NeoGenModule() {
    config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);
    
    configParam(SHAPE_PARAM, 0.0f, 1.0f, 0.0f, "Shape", "", 0.f, 100.f);
    configParam(ARTIFACT_PARAM, 0.0f, 1.0f, 0.0f, "Artifact", "%", 0.f, 100.f);
    configParam(HARMONICS_PARAM, 0.0f, 1.0f, 0.5f, "Harmonics", "%", 0.f, 100.f);
    configParam(DRIVE_PARAM, 0.0f, 1.0f, 0.0f, "Drive", "%", 0.f, 100.f);
    
    configInput(VOCT_INPUT, "V/Oct");
    configInput(SHAPE_INPUT, "Shape CV");
    configInput(ARTIFACT_INPUT, "Artifact CV");
    configInput(HARMONICS_INPUT, "Harmonics CV");
    configInput(DRIVE_INPUT, "Drive CV");
    configOutput(AUDIO_OUTPUT, "Audio");
}

void NeoGenModule::process(const ProcessArgs& args) {
    // Get parameter values and add CV inputs
    float shape = params[SHAPE_PARAM].getValue();
    if (inputs[SHAPE_INPUT].isConnected()) {
        shape = clamp(shape + inputs[SHAPE_INPUT].getVoltage() / 10.0f, 0.0f, 1.0f);
    }
    
    float artifact = params[ARTIFACT_PARAM].getValue();
    if (inputs[ARTIFACT_INPUT].isConnected()) {
        artifact = clamp(artifact + inputs[ARTIFACT_INPUT].getVoltage() / 10.0f, 0.0f, 1.0f);
    }
    
    float harmonics = params[HARMONICS_PARAM].getValue();
    if (inputs[HARMONICS_INPUT].isConnected()) {
        harmonics = clamp(harmonics + inputs[HARMONICS_INPUT].getVoltage() / 10.0f, 0.0f, 1.0f);
    }
    
    float drive = params[DRIVE_PARAM].getValue();
    if (inputs[DRIVE_INPUT].isConnected()) {
        drive = clamp(drive + inputs[DRIVE_INPUT].getVoltage() / 10.0f, 0.0f, 1.0f);
    }
    
    // Calculate frequency from V/Oct input
    float pitch = 0.0f; // Default to C4 (MIDI 60)
    if (inputs[VOCT_INPUT].isConnected()) {
        pitch = inputs[VOCT_INPUT].getVoltage();
    }
    
    // Convert V/Oct to frequency (A4 = 440Hz at 0V, or C4 = 261.626Hz at -0.75V)
    // Using dsp::FREQ_C4 as reference
    frequency = dsp::FREQ_C4 * std::pow(2.0f, pitch);
    
    // Update phase
    phase += frequency * args.sampleTime;
    while (phase >= 1.0f) {
        phase -= 1.0f;
    }
    
    // ==================================================================
    // WAVEFORM MORPHING SECTION
    // Shape parameter selects and blends between 4 algorithms:
    //   0.00–0.25 → Fractal Saw
    //   0.25–0.50 → Polygon Wave
    //   0.50–0.75 → String-Solver
    //   0.75–1.00 → Neuron Spike
    // ==================================================================
    
    float output = 0.0f;
    
    if (shape < 0.25f) {
        // Region 1: Fractal Saw (pure at 0.0, blending at 0.25)
        float blend = shape / 0.25f;
        float fractal = generateFractalSaw(phase, harmonics);
        
        if (blend < 1.0f) {
            output = fractal;
        } else {
            float polygon = generatePolygonWave(phase, harmonics);
            output = fractal + (polygon - fractal) * (blend - 1.0f) / 0.0f;
        }
    }
    else if (shape < 0.5f) {
        // Region 2: Polygon Wave (pure at 0.375, blending edges)
        float localShape = (shape - 0.25f) / 0.25f;
        float polygon = generatePolygonWave(phase, harmonics);
        
        if (localShape < 0.5f) {
            float fractal = generateFractalSaw(phase, harmonics);
            output = fractal + (polygon - fractal) * localShape * 2.0f;
        } else {
            float string = generateStringSolver(phase, harmonics);
            output = polygon + (string - polygon) * (localShape - 0.5f) * 2.0f;
        }
    }
    else if (shape < 0.75f) {
        // Region 3: String-Solver (pure at 0.625, blending edges)
        float localShape = (shape - 0.5f) / 0.25f;
        float string = generateStringSolver(phase, harmonics);
        
        if (localShape < 0.5f) {
            float polygon = generatePolygonWave(phase, harmonics);
            output = polygon + (string - polygon) * localShape * 2.0f;
        } else {
            float neuron = generateNeuronSpike(phase, harmonics);
            output = string + (neuron - string) * (localShape - 0.5f) * 2.0f;
        }
    }
    else {
        // Region 4: Neuron Spike (pure at 1.0, blending at 0.75)
        float localShape = (shape - 0.75f) / 0.25f;
        float neuron = generateNeuronSpike(phase, harmonics);
        
        if (localShape < 0.5f) {
            float string = generateStringSolver(phase, harmonics);
            output = string + (neuron - string) * localShape * 2.0f;
        } else {
            output = neuron;
        }
    }
    
    // ==================================================================
    // ARTIFACT PROCESSING
    // ==================================================================
    output = applyArtifact(output, artifact);
    
    // ==================================================================
    // DRIVE / WAVESHAPING
    // ==================================================================
    output = applyDrive(output, drive);
    
    // Output to audio jack (scale to ±5V)
    outputs[AUDIO_OUTPUT].setVoltage(output * 5.0f);
}

// ==================================================================
// WAVEFORM ALGORITHMS
// Each returns a sample in range approximately -1.0 to +1.0
// ==================================================================

float NeoGenModule::generateFractalSaw(float phase, float harmonics) {
    // TODO: Implement proper fractal sawtooth with iterated function system
    // For now: basic sawtooth with harmonic-dependent octave blending
    
    float saw1 = phase * 2.0f - 1.0f;  // Fundamental
    float saw2 = std::fmod(phase * 2.0f, 1.0f) * 2.0f - 1.0f;  // Octave up
    float saw3 = std::fmod(phase * 4.0f, 1.0f) * 2.0f - 1.0f;  // Two octaves up
    
    // Blend octaves based on harmonics parameter
    return saw1 * (1.0f - harmonics) + (saw2 * 0.5f + saw3 * 0.25f) * harmonics;
}

float NeoGenModule::generatePolygonWave(float phase, float harmonics) {
    // TODO: Implement morphing N-gon oscillator (triangle→square→pentagon→etc)
    // For now: blend between triangle and square based on harmonics
    
    float triangle = (phase < 0.5f) 
        ? (phase * 4.0f - 1.0f) 
        : (3.0f - phase * 4.0f);
    
    float square = (phase < 0.5f) ? 1.0f : -1.0f;
    
    // Morph from triangle to square as harmonics increases
    return triangle * (1.0f - harmonics) + square * harmonics;
}

float NeoGenModule::generateStringSolver(float phase, float harmonics) {
    // TODO: Implement continuous Karplus-Strong with feedback and damping
    // For now: delayed sine with feedback simulation
    
    float excitation = std::sin(2.0f * M_PI * phase);
    
    // Simulate string resonance with feedback
    float feedback = stringBuffer[stringWritePos] * 0.99f * (1.0f - harmonics * 0.5f);
    float newSample = excitation * 0.1f + feedback;
    
    stringBuffer[stringWritePos] = newSample;
    stringWritePos = (stringWritePos + 1) % 256;
    
    return newSample;
}

float NeoGenModule::generateNeuronSpike(float phase, float harmonics) {
    // TODO: Implement biological spike-shape (Hodgkin-Huxley style)
    // For now: exponential attack + decay envelope on sine
    
    float spike;
    if (phase < 0.1f) {
        // Fast attack
        float t = phase / 0.1f;
        spike = std::sin(2.0f * M_PI * phase) * t * t;
    } else {
        // Exponential decay
        float t = (phase - 0.1f) / 0.9f;
        spike = std::sin(2.0f * M_PI * phase) * std::exp(-t * 5.0f * (1.0f + harmonics * 4.0f));
    }
    
    return spike;
}

// ==================================================================
// EFFECTS PROCESSING
// ==================================================================

float NeoGenModule::applyArtifact(float sample, float amount) {
    // TODO: Implement bit crushing, aliasing, foldback, asymmetry
    // For now: simple bit reduction + sample rate reduction
    
    if (amount < 0.01f) return sample;
    
    // Bit reduction (quantization)
    float bits = 16.0f - amount * 12.0f;  // 16-bit down to 4-bit
    float steps = std::pow(2.0f, bits);
    float quantized = std::round(sample * steps) / steps;
    
    // Blend original and quantized
    return sample * (1.0f - amount) + quantized * amount;
}

float NeoGenModule::applyDrive(float sample, float amount) {
    // TODO: Implement multi-stage waveshaping and DC offset
    // For now: soft clipping with tanh
    
    if (amount < 0.01f) return sample;
    
    float gain = 1.0f + amount * 9.0f;  // 1x to 10x gain
    float driven = std::tanh(sample * gain);
    
    // Blend dry and driven signal
    return sample * (1.0f - amount) + driven * amount;
}
