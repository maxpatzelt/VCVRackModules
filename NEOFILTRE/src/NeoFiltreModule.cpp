#include "NeoFiltreModule.hpp"
#include <cmath>

NeoFiltreModule::NeoFiltreModule() {
	config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);
	
	configParam(COUPURE_PARAM, 0.0f, 1.0f, 0.5f, "Coupure (Cutoff)", " Hz", 0.f, 20000.f, 20.f);
	configParam(RESONANCE_PARAM, 0.0f, 1.0f, 0.0f, "Résonance", "%", 0.f, 100.f);
	configParam(GAUSS_PARAM, 0.0f, 1.0f, 0.5f, "Gauss (Spectral Curvature)", "%", 0.f, 100.f);
	configParam(SATINE_PARAM, 0.0f, 1.0f, 0.0f, "Satine (Saturation)", "%", 0.f, 100.f);
	
	configInput(AUDIO_INPUT, "Audio");
	configInput(COUPURE_INPUT, "Coupure CV");
	configInput(RESONANCE_INPUT, "Résonance CV");
	configInput(GAUSS_INPUT, "Gauss CV");
	configInput(SATINE_INPUT, "Satine CV");
	configOutput(AUDIO_OUTPUT, "Audio");
}

void NeoFiltreModule::process(const ProcessArgs& args) {
	// Get input signal
	if (!inputs[AUDIO_INPUT].isConnected() || !outputs[AUDIO_OUTPUT].isConnected()) {
		outputs[AUDIO_OUTPUT].setVoltage(0.f);
		return;
	}
	
	float input = inputs[AUDIO_INPUT].getVoltage();
	
	// Get parameter values and add CV inputs
	float coupure = params[COUPURE_PARAM].getValue();
	if (inputs[COUPURE_INPUT].isConnected()) {
		coupure = clamp(coupure + inputs[COUPURE_INPUT].getVoltage() / 10.0f, 0.0f, 1.0f);
	}
	
	float resonance = params[RESONANCE_PARAM].getValue();
	if (inputs[RESONANCE_INPUT].isConnected()) {
		resonance = clamp(resonance + inputs[RESONANCE_INPUT].getVoltage() / 10.0f, 0.0f, 1.0f);
	}
	
	float gauss = params[GAUSS_PARAM].getValue();
	if (inputs[GAUSS_INPUT].isConnected()) {
		gauss = clamp(gauss + inputs[GAUSS_INPUT].getVoltage() / 10.0f, 0.0f, 1.0f);
	}
	
	float satine = params[SATINE_PARAM].getValue();
	if (inputs[SATINE_INPUT].isConnected()) {
		satine = clamp(satine + inputs[SATINE_INPUT].getVoltage() / 10.0f, 0.0f, 1.0f);
	}
	
	// ==================================================================
	// SIGNAL CHAIN
	// ==================================================================
	
	// Stage 1: Pre-filter saturation (gentle analog warmth)
	float preSat = input;
	if (satine > 0.01f) {
		preSat = std::tanh(input * (1.0f + satine * 0.5f)) / (1.0f + satine * 0.3f);
	}
	
	// Stage 2: State-variable filter
	// Convert coupure parameter to frequency (20 Hz - 20 kHz, exponential)
	float cutoffFreq = 20.0f * std::pow(1000.0f, coupure);
	
	// At very high cutoff, bypass the filter to avoid cutting the signal
	float filtered;
	if (cutoffFreq > args.sampleRate * 0.4f) {
		// Above Nyquist limit, just pass through with minimal filtering
		filtered = preSat;
	} else {
		filtered = processStateVariableFilter(preSat, cutoffFreq, resonance, args.sampleRate);
	}
	
	// Stage 3: Gauss spectral shaping
	float shaped = applyGaussShaping(filtered, gauss);
	
	// Stage 4: Satine dual-stage saturation
	float output = applySatine(shaped, satine);
	
	// Output
	outputs[AUDIO_OUTPUT].setVoltage(output);
}

// ==================================================================
// DSP HELPER FUNCTIONS
// ==================================================================

float NeoFiltreModule::processStateVariableFilter(float input, float cutoff, float resonance, float sampleRate) {
	// State-variable filter with stable cutoff range and resonance
	
	// Calculate filter coefficient with safety limits
	// Clamp cutoff to prevent instability
	float safeCutoff = clamp(cutoff, 20.0f, sampleRate * 0.45f);
	float freq = clamp(safeCutoff / sampleRate, 0.0001f, 0.45f);
	float f = 2.0f * std::sin(M_PI * freq);
	
	// Clamp f to prevent instability
	f = clamp(f, 0.0001f, 1.99f);
	
	// Resonance with warm tape-like character
	// Higher resonance = lower damping (more feedback)
	float q = clamp(1.0f - resonance * 0.95f, 0.01f, 0.99f);
	
	// State-variable filter equations
	lowpass += f * bandpass;
	highpass = input - lowpass - q * bandpass;
	bandpass += f * highpass;
	
	// Clamp state variables to prevent NaN
	lowpass = clamp(lowpass, -10.0f, 10.0f);
	bandpass = clamp(bandpass, -10.0f, 10.0f);
	highpass = clamp(highpass, -10.0f, 10.0f);
	
	// Gentle nonlinearity for analog character (only when not near extremes)
	if (std::abs(lowpass) < 5.0f) {
		lowpass = std::tanh(lowpass * 1.1f) / 1.1f;
	}
	
	// Add resonance feedback to make it more audible
	float resonantLP = lowpass + bandpass * resonance * 0.5f;
	
	return clamp(resonantLP, -10.0f, 10.0f);
}

float NeoFiltreModule::applyGaussShaping(float sample, float gauss) {
	// Experimental spectral curvature / formant morphing
	
	if (gauss < 0.01f) return sample;
	
	// Map gauss to -1 to +1 for bidirectional morphing
	float morph = gauss * 2.0f - 1.0f;
	
	// Stage 1: Spectral tilt (0.0 - 0.4)
	if (gauss < 0.4f) {
		float tiltAmount = gauss * 2.5f;
		if (morph < 0.0f) {
			// Dark: heavy lowpass character
			sample = std::tanh(sample * (1.0f - tiltAmount * 0.7f));
		} else {
			// Bright: harmonic emphasis
			sample = sample * (1.0f + tiltAmount * 1.5f);
		}
	}
	
	// Stage 2: Formant-like resonance (0.3 - 0.7)
	if (gauss > 0.3f && gauss < 0.7f) {
		float formantAmount = (gauss - 0.3f) * 2.5f;
		// Create vowel-like resonance peaks
		float emphasis = 1.0f + std::sin(sample * 10.0f * M_PI) * formantAmount * 0.8f;
		sample = sample * emphasis;
	}
	
	// Stage 3: Spectral folding (0.6 - 1.0)
	if (gauss > 0.6f) {
		float foldAmount = (gauss - 0.6f) * 2.5f;
		// Frequency domain folding simulation
		float folded = std::sin(sample * (1.0f + foldAmount * 8.0f) * M_PI);
		sample = sample * (1.0f - foldAmount) + folded * foldAmount;
	}
	
	return clamp(sample, -10.0f, 10.0f);
}

float NeoFiltreModule::applySatine(float sample, float satine) {
	// Extreme dual-stage saturation: velvet overdrive + asymmetric diode lift
	
	if (satine < 0.01f) return sample;
	
	float processed = sample;
	
	// Stage A: Velvet saturation (0.0 - 0.5)
	if (satine > 0.0f) {
		float gain = 1.0f + satine * 8.0f;  // Up to 9x gain
		processed = std::tanh(sample * gain) / (1.0f + satine * 0.3f);
	}
	
	// Stage B: Asymmetric diode clipping (0.3 - 0.7)
	if (satine > 0.3f) {
		float asymAmount = (satine - 0.3f) * 2.5f;
		
		if (processed > 0.0f) {
			// Positive: hard diode clip
			processed = processed / (1.0f + processed * asymAmount * 4.0f);
		} else {
			// Negative: soft fold
			processed = processed / (1.0f + std::abs(processed) * asymAmount);
		}
	}
	
	// Stage C: Harmonic distortion (0.5 - 1.0)
	if (satine > 0.5f) {
		float harmAmount = (satine - 0.5f) * 2.0f;
		// Add harmonic overtones
		float harmonic = std::sin(processed * 3.0f * M_PI) * harmAmount * 0.5f;
		processed = processed + harmonic;
	}
	
	// Stage D: Wave folding (0.7 - 1.0)
	if (satine > 0.7f) {
		float foldAmount = (satine - 0.7f) * 3.33f;
		float gain = 1.0f + foldAmount * 4.0f;
		processed = processed * gain;
		
		// Multiple folding passes for extreme effect
		for (int i = 0; i < 3; i++) {
			if (processed > 1.0f) processed = 2.0f - processed;
			if (processed < -1.0f) processed = -2.0f - processed;
		}
	}
	
	return clamp(processed, -10.0f, 10.0f);
}
